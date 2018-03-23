// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_MICHAEL_LIST_RCU_H
#define CDSLIB_INTRUSIVE_MICHAEL_LIST_RCU_H

#include <cds/intrusive/details/michael_list_base.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/details/make_const_type.h>
#include <cds/urcu/exempt_ptr.h>
#include <cds/urcu/raw_ptr.h>
#include <cds/intrusive/details/raw_ptr_disposer.h>

namespace cds { namespace intrusive {

    //@cond
    namespace michael_list {

        /// Node specialization for uRCU
        template <class RCU, typename Tag>
        struct node< cds::urcu::gc< RCU >, Tag >
        {
            typedef cds::urcu::gc< RCU > gc;   ///< Garbage collector
            typedef Tag                  tag;  ///< tag

            typedef cds::details::marked_ptr<node, 3>   marked_ptr; ///< marked pointer
            typedef typename gc::template atomic_marked_ptr<marked_ptr> atomic_marked_ptr; ///< atomic marked pointer specific for GC

            atomic_marked_ptr m_pNext; ///< pointer to the next node in the container
            node *            m_pDelChain; ///< Deleted node chain (local for a thread)

            constexpr node() noexcept
                : m_pNext( nullptr )
                , m_pDelChain( nullptr )
            {}
        };
    } // namespace michael_list
    //@endcond

    /// Michael's lock-free ordered single-linked list (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_MichaelList_rcu

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>.

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p T - type to be stored in the list; the type \p T should be based on (or has a member of type)
            cds::intrusive::micheal_list::node
        - \p Traits - type traits. See \p michael_list::traits for explanation. It is possible to declare option-based
             list with \p cds::intrusive::michael_list::make_traits metafunction,
             see \ref cds_intrusive_MichaelList_hp "here" for explanations.

        \par Usage
            Before including <tt><cds/intrusive/michael_list_rcu.h></tt> you should include appropriate RCU header file,
            see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
            For example, for \ref cds_urcu_general_buffered_gc "general-purpose buffered RCU" you should include:
            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/michael_list_rcu.h>

            // Now, you can declare Michael's list for type Foo and default traits:
            typedef cds::intrusive::MichaelList<cds::urcu::gc< cds::urcu::general_buffered<> >, Foo > rcu_michael_list;
            \endcode
    */
    template < typename RCU, typename T,
#ifdef CDS_DOXYGEN_INVOKED
    class Traits = michael_list::traits
#else
    class Traits
#endif
    >
    class MichaelList<cds::urcu::gc<RCU>, T, Traits>
    {
    public:
        typedef T       value_type; ///< type of value stored in the list
        typedef Traits  traits;     ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key comparison functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif

        typedef typename traits::disposer  disposer;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename michael_list::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

        typedef cds::urcu::gc<RCU>                  gc;           ///< RCU schema
        typedef typename traits::back_off           back_off;     ///< back-off strategy
        typedef typename traits::item_counter       item_counter; ///< Item counting policy used
        typedef typename traits::memory_model       memory_model; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::rcu_check_deadlock rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename traits::stat               stat;     ///< Internal statistics

        typedef typename gc::scoped_lock    rcu_lock ;  ///< RCU scoped lock
        static constexpr const bool c_bExtractLockExternal = false; ///< Group of \p extract_xxx functions do not require external locking

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef MichaelList<
                gc
                , value_type
                , typename cds::opt::make_options< traits, Options...>::type
            >   type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = michael_list::select_stat_wrapper< Stat >;
        //@endcond

    protected:
        typedef typename node_type::marked_ptr        marked_node_ptr; ///< Marked node pointer
        typedef typename node_type::atomic_marked_ptr atomic_node_ptr; ///< Atomic node pointer
        typedef atomic_node_ptr                       auxiliary_head;  ///< Auxiliary head type (for split-list support)

        atomic_node_ptr m_pHead;        ///< Head pointer
        item_counter    m_ItemCounter;  ///< Item counter
        stat            m_Stat;         ///< Internal statistics

    protected:
        //@cond
        enum erase_node_mask
        {
            erase_mask   = 1,
            extract_mask = 3
        };

        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock>   check_deadlock_policy;

        struct clear_and_dispose {
            void operator()( value_type * p )
            {
                assert( p != nullptr );
                clear_links( node_traits::to_node_ptr(p));
                disposer()( p );
            }
        };

        /// Position pointer for item search
        struct position {
            atomic_node_ptr * pPrev ;   ///< Previous node
            node_type * pCur        ;   ///< Current node
            node_type * pNext       ;   ///< Next node

            atomic_node_ptr& refHead;
            node_type * pDelChain; ///< Head of deleted node chain

            position( atomic_node_ptr& head )
                : refHead( head )
                , pDelChain( nullptr )
            {}

            ~position()
            {
                dispose_chain( pDelChain );
            }
        };
        //@endcond

    public:
        using exempt_ptr = cds::urcu::exempt_ptr< gc, value_type, value_type, clear_and_dispose, void >; ///< pointer to extracted node

    private:
        //@cond
        struct chain_disposer {
            void operator()( node_type * pChain ) const
            {
                dispose_chain( pChain );
            }
        };
        typedef cds::intrusive::details::raw_ptr_disposer< gc, node_type, chain_disposer> raw_ptr_disposer;
        //@endcond

    public:
        /// Result of \p get(), \p get_with() functions - pointer to the node found
        typedef cds::urcu::raw_ptr< gc, value_type, raw_ptr_disposer > raw_ptr;

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class MichaelList;
            value_type * m_pNode;

            void next()
            {
                if ( m_pNode ) {
                    node_type * p = node_traits::to_node_ptr( *m_pNode )->m_pNext.load(memory_model::memory_order_relaxed).ptr();
                    m_pNode = p ? node_traits::to_value_ptr( p ) : nullptr;
                }
            }

        protected:
            explicit iterator_type( node_type * pNode)
            {
                if ( pNode )
                    m_pNode = node_traits::to_value_ptr( *pNode );
                else
                    m_pNode = nullptr;
            }
            explicit iterator_type( atomic_node_ptr const& refNode)
            {
                node_type * pNode = refNode.load(memory_model::memory_order_relaxed).ptr();
                m_pNode = pNode ? node_traits::to_value_ptr( *pNode ) : nullptr;
            }

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
                : m_pNode( nullptr )
            {}

            iterator_type( const iterator_type& src )
                : m_pNode( src.m_pNode )
            {}

            value_ptr operator ->() const
            {
                return m_pNode;
            }

            value_ref operator *() const
            {
                assert( m_pNode != nullptr );
                return *m_pNode;
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                next();
                return *this;
            }

            /// Post-increment
            iterator_type operator ++(int)
            {
                iterator_type i(*this);
                next();
                return i;
            }

            iterator_type& operator = (const iterator_type& src)
            {
                m_pNode = src.m_pNode;
                return *this;
            }

            template <bool C>
            bool operator ==(iterator_type<C> const& i ) const
            {
                return m_pNode == i.m_pNode;
            }
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return m_pNode != i.m_pNode;
            }
        };
        //@endcond

    public:
    ///@name Forward iterators (thread-safe only under RCU lock)
    //@{
        /// Forward iterator
        /**
            You may safely use iterators in multi-threaded environment only under RCU lock.
            Otherwise, a crash is possible if another thread deletes the item the iterator points to.
        */
        typedef iterator_type<false>    iterator;

        /// Const forward iterator
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( m_pHead );
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            Internally, <tt>end</tt> returning value equals to \p nullptr.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator();
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return const_iterator(m_pHead );
        }
        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return const_iterator(m_pHead );
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return const_iterator();
        }
        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return const_iterator();
        }
    //@}

    public:
        /// Default constructor initializes empty list
        MichaelList()
            : m_pHead( nullptr )
        {
            static_assert( (std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type" );
        }

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, michael_list::wrapped_stat<Stat>>::value >>
        explicit MichaelList( Stat& st )
            : m_pHead( nullptr )
            , m_Stat( st )
        {}
        //@endcond

        /// Destroy list
        ~MichaelList()
        {
            clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the list if the list does not contain
            an item with key equal to \p val.

            The function makes RCU lock internally.

            Returns \p true if \p val is linked into the list, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_at( m_pHead, val );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split new item creating into two part:
            - create item with key only
            - insert new item into the list
            - if inserting is success, calls \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this list's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.

            The function makes RCU lock internally.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            return insert_at( m_pHead, val, f );
        }

        /// Updates the item
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val not found in the list, then \p val is inserted into the list
            iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
            struct functor {
                void operator()( bool bNew, value_type& item, value_type& val );
            };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list
            - \p val - argument \p val passed into the \p update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns <tt> std::pair<bool, bool>  </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the list.

            The function makes RCU lock internally.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            return update_at( m_pHead, val, func, bAllowInsert );
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Unlinks the item \p val from the list
        /**
            The function searches the item \p val in the list and unlink it from the list
            if it is found and it is equal to \p val.

            Difference between \p erase() and \p %unlink() functions: \p %erase() finds <i>a key</i>
            and deletes the item found. \p %unlink() finds an item by key and deletes it
            only if \p val is an item of that list, i.e. the pointer to the item found
            is equal to <tt> &val </tt>.

            The function returns \p true if success and \p false otherwise.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            \p disposer specified in \p Traits is called for unlinked item.

            The function can throw cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is opt::v::rcu_throw_deadlock.
        */
        bool unlink( value_type& val )
        {
            return unlink_at( m_pHead, val );
        }

        /// Deletes the item from the list
        /**
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \p true.
            If the item with the key equal to \p key is not found the function return \p false.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            \p disposer specified in \p Traits is called for deleted item.

            The function can throw \ref cds_urcu_rcu_deadlock "cds::urcu::rcu_deadlock" exception if a deadlock is detected and
            the deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( m_pHead, key, key_comparator());
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \p erase(Q const&)
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Deletes the item from the list
        /**
            The function searches an item with key equal to \p key in the list,
            call \p func functor with item found, unlinks it from the list, and returns \p true.
            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If the item with the key equal to \p key is not found the function return \p false.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            \p disposer specified in \p Traits is called for deleted item.

            The function can throw \ref cds_urcu_rcu_deadlock "cds::urcu::rcu_deadlock" exception if a deadlock is detected and
            the deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func func )
        {
            return erase_at( m_pHead, key, key_comparator(), func );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \p erase(Q const&, Func)
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func func )
        {
            CDS_UNUSED( pred );
            return erase_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), func );
        }

        /// Extracts an item from the list
        /**
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If \p key is not found the function returns an empty \p exempt_ptr.

            @note The function does NOT dispose the item found. It just unlinks the item from the list
            and returns a pointer to item found.
            You shouldn't lock RCU for current thread before calling this function, and you should manually release
            the returned exempt pointer before reusing it.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/michael_list_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::intrusive::MichaelList< rcu, Foo > rcu_michael_list;

            rcu_michael_list theList;
            // ...

            rcu_michael_list::exempt_ptr p1;

            // The RCU should NOT be locked when extract() is called!
            assert( !rcu::is_locked());

            // You can call extract() function
            p1 = theList.extract( 10 );
            if ( p1 ) {
                // do something with p1
                ...
            }

            // We may safely release p1 here
            // release() passes the pointer to RCU reclamation cycle:
            // it invokes RCU retire_ptr function with the disposer you provided for the list.
            p1.release();
            \endcode
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return exempt_ptr( extract_at( m_pHead, key, key_comparator()));
        }

        /// Extracts an item from the list using \p pred predicate for searching
        /**
            This function is the analog for \p extract(Q const&)

            The \p pred is a predicate used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as \ref key_comparator.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( extract_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>()));
        }

        /// Find the key \p val
        /**
            The function searches the item with key equal to \p key
            and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item.
            The function \p find does not serialize simultaneous access to the list \p item. If such access is
            possible you must provide your own synchronization schema to exclude unsafe item modifications.

            The function makes RCU lock internally.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_at( m_pHead, key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_at( m_pHead, key, key_comparator(), f );
        }
        //@endcond

        /// Finds \p key using \p pred predicate for searching
        /**
            The function is an analog of \p find(Q&, Func)
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@endcond

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return find_at( m_pHead, key, key_comparator());
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_intrusive_MichaelList_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns empty \p raw_ptr object.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::intrusive::MichaelList< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > ord_list;
            ord_list theList;
            // ...
            typename ord_list::raw_ptr rp;
            {
                // Lock RCU
                ord_list::rcu_lock lock;

                rp = theList.get( 5 );
                if ( rp ) {
                    // Deal with rp
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // Node owned by rp can be retired by disposer at any time after RCU has been unlocked
            }
            // You can manually release rp after RCU-locked section
            rp.release();
            \endcode
        */
        template <typename Q>
        raw_ptr get( Q const& key )
        {
            return get_at( m_pHead, key, key_comparator());
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \p value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        raw_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return get_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Clears the list using default disposer
        /**
            The function clears the list using default (provided by \p Traits class template argument) disposer functor.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer invocation can be deferred.

            The function can throw \p cds::urcu::rcu_deadlock exception if a deadlock is encountered and
            deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        void clear()
        {
            if( !empty()) {
                check_deadlock_policy::check();

                marked_node_ptr pHead;
                for (;;) {
                    {
                        rcu_lock l;
                        pHead = m_pHead.load(memory_model::memory_order_acquire);
                        if ( !pHead.ptr())
                            break;
                        marked_node_ptr pNext( pHead->m_pNext.load(memory_model::memory_order_relaxed));
                        if ( cds_unlikely( !pHead->m_pNext.compare_exchange_weak( pNext, pNext | 1, memory_model::memory_order_acquire, memory_model::memory_order_relaxed )))
                            continue;
                        if ( cds_unlikely( !m_pHead.compare_exchange_weak( pHead, marked_node_ptr(pNext.ptr()), memory_model::memory_order_release, memory_model::memory_order_relaxed )))
                            continue;
                    }

                    --m_ItemCounter;
                    dispose_node( pHead.ptr());
                }
            }
        }

        /// Check if the list is empty
        bool empty() const
        {
            return m_pHead.load( memory_model::memory_order_relaxed ).all() == nullptr;
        }

        /// Returns list's item count
        /**
            The value returned depends on item counter provided by \p Traits. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact does not mean that the list
            is empty. To check list emptyness use \p empty() method.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

    protected:
        //@cond
        static void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( marked_node_ptr(), memory_model::memory_order_release );
            pNode->m_pDelChain = nullptr;
        }

        static void dispose_node( node_type * pNode )
        {
            assert( pNode );
            assert( !gc::is_locked());

            gc::template retire_ptr<clear_and_dispose>( node_traits::to_value_ptr( *pNode ));
        }

        static void dispose_chain( node_type * pChain )
        {
            if ( pChain ) {
                assert( !gc::is_locked());

                auto f = [&pChain]() -> cds::urcu::retired_ptr {
                    node_type * p = pChain;
                    if ( p ) {
                        pChain = p->m_pDelChain;
                        return cds::urcu::make_retired_ptr<clear_and_dispose>( node_traits::to_value_ptr( p ));
                    }
                    return cds::urcu::make_retired_ptr<clear_and_dispose>( static_cast<value_type *>(nullptr));
                };
                gc::batch_retire( std::ref( f ));
            }
        }

        bool link_node( node_type * pNode, position& pos )
        {
            assert( pNode != nullptr );
            link_checker::is_empty( pNode );

            marked_node_ptr p( pos.pCur );
            pNode->m_pNext.store( p, memory_model::memory_order_release );
            if ( cds_likely( pos.pPrev->compare_exchange_strong( p, marked_node_ptr( pNode ), memory_model::memory_order_release, atomics::memory_order_relaxed )))
                return true;

            pNode->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
            return false;
        }

        static void link_to_remove_chain( position& pos, node_type * pDel )
        {
            assert( pDel->m_pDelChain == nullptr );

            pDel->m_pDelChain = pos.pDelChain;
            pos.pDelChain = pDel;
        }

        bool unlink_node( position& pos, erase_node_mask nMask )
        {
            assert( gc::is_locked());

            // Mark the node (logical deletion)
            marked_node_ptr next( pos.pNext, 0 );

            if ( cds_likely( pos.pCur->m_pNext.compare_exchange_strong( next, next | nMask, memory_model::memory_order_release, atomics::memory_order_relaxed ))) {

                // Try physical removal - fast path
                marked_node_ptr cur( pos.pCur );
                if ( cds_likely( pos.pPrev->compare_exchange_strong( cur, marked_node_ptr( pos.pNext ), memory_model::memory_order_acquire, atomics::memory_order_relaxed ))) {
                    if ( nMask == erase_mask )
                        link_to_remove_chain( pos, pos.pCur );
                }
                else {
                    // Slow path
                    search( pos.refHead, *node_traits::to_value_ptr( pos.pCur ), pos, key_comparator());
                }
                return true;
            }
            return false;
        }

        // split-list support
        bool insert_aux_node( node_type * pNode )
        {
            return insert_aux_node( m_pHead, pNode );
        }

        // split-list support
        bool insert_aux_node( atomic_node_ptr& refHead, node_type * pNode )
        {
            assert( pNode != nullptr );

            // Hack: convert node_type to value_type.
            // In principle, auxiliary node can be non-reducible to value_type
            // We assume that comparator can correctly distinguish between aux and regular node.
            return insert_at( refHead, *node_traits::to_value_ptr( pNode ));
        }

        bool insert_at( atomic_node_ptr& refHead, value_type& val )
        {
            position pos( refHead );
            {
                rcu_lock l;
                return insert_at_locked( pos, val );
            }
        }

        template <typename Func>
        bool insert_at( atomic_node_ptr& refHead, value_type& val, Func f )
        {
            position pos( refHead );

            {
                rcu_lock l;
                while ( true ) {
                    if ( search( refHead, val, pos, key_comparator())) {
                        m_Stat.onInsertFailed();
                        return false;
                    }

                    if ( link_node( node_traits::to_node_ptr( val ), pos )) {
                        f( val );
                        ++m_ItemCounter;
                        m_Stat.onInsertSuccess();
                        return true;
                    }

                    // clear next field
                    node_traits::to_node_ptr( val )->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
                    m_Stat.onInsertRetry();
                }
            }

        }

        iterator insert_at_( atomic_node_ptr& refHead, value_type& val )
        {
            rcu_lock l;
            if ( insert_at_locked( refHead, val ))
                return iterator( node_traits::to_node_ptr( val ));
            return end();
        }

        template <typename Func>
        std::pair<iterator, bool> update_at_( atomic_node_ptr& refHead, value_type& val, Func func, bool bInsert )
        {
            position pos( refHead );
            {
                rcu_lock l;
                return update_at_locked( pos, val, func, bInsert );
            }
        }

        template <typename Func>
        std::pair<bool, bool> update_at( atomic_node_ptr& refHead, value_type& val, Func func, bool bInsert )
        {
            position pos( refHead );
            {
                rcu_lock l;
                std::pair<iterator, bool> ret = update_at_locked( pos, val, func, bInsert );
                return std::make_pair( ret.first != end(), ret.second );
            }
        }

        bool unlink_at( atomic_node_ptr& refHead, value_type& val )
        {
            position pos( refHead );
            back_off bkoff;
            check_deadlock_policy::check();

            for (;;) {
                {
                    rcu_lock l;
                    if ( !search( refHead, val, pos, key_comparator()) || node_traits::to_value_ptr( *pos.pCur ) != &val ) {
                        m_Stat.onEraseFailed();
                        return false;
                    }
                    if ( !unlink_node( pos, erase_mask )) {
                        bkoff();
                        m_Stat.onEraseRetry();
                        continue;
                    }
                }

                --m_ItemCounter;
                m_Stat.onEraseSuccess();
                return true;
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( position& pos, Q const& val, Compare cmp, Func f )
        {
            back_off bkoff;
            check_deadlock_policy::check();

            node_type * pDel;
            for (;;) {
                {
                    rcu_lock l;
                    if ( !search( pos.refHead, val, pos, cmp )) {
                        m_Stat.onEraseFailed();
                        return false;
                    }

                    // store pCur since it may be changed by unlink_node() slow path
                    pDel = pos.pCur;
                    if ( !unlink_node( pos, erase_mask )) {
                        bkoff();
                        m_Stat.onEraseRetry();
                        continue;
                    }
                }
                assert( pDel );
                f( *node_traits::to_value_ptr( pDel ));
                --m_ItemCounter;
                m_Stat.onEraseSuccess();
                return true;
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( atomic_node_ptr& refHead, Q const& val, Compare cmp, Func f )
        {
            position pos( refHead );
            return erase_at( pos, val, cmp, f );
        }

        template <typename Q, typename Compare>
        bool erase_at( atomic_node_ptr& refHead, const Q& val, Compare cmp )
        {
            position pos( refHead );
            return erase_at( pos, val, cmp, [](value_type const&){} );
        }

        template <typename Q, typename Compare >
        value_type * extract_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos( refHead );
            back_off bkoff;
            assert( !gc::is_locked())  ;   // RCU must not be locked!!!

            node_type * pExtracted;
            {
                rcu_lock l;
                for (;;) {
                    if ( !search( refHead, val, pos, cmp )) {
                        m_Stat.onEraseFailed();
                        return nullptr;
                    }

                    // store pCur since it may be changed by unlink_node() slow path
                    pExtracted = pos.pCur;
                    if ( !unlink_node( pos, extract_mask )) {
                        bkoff();
                        m_Stat.onEraseRetry();
                        continue;
                    }

                    --m_ItemCounter;
                    value_type * pRet = node_traits::to_value_ptr( pExtracted );
                    assert( pExtracted->m_pDelChain == nullptr );
                    m_Stat.onEraseSuccess();
                    return pRet;
                }
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( atomic_node_ptr& refHead, Q& val, Compare cmp, Func f )
        {
            position pos( refHead );

            {
                rcu_lock l;
                if ( search( refHead, val, pos, cmp )) {
                    assert( pos.pCur != nullptr );
                    f( *node_traits::to_value_ptr( *pos.pCur ), val );
                    m_Stat.onFindSuccess();
                    return true;
                }
           }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare>
        bool find_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos( refHead );
            {
                rcu_lock l;
                return find_at_locked( pos, val, cmp ) != cend();
            }
        }

        template <typename Q, typename Compare>
        raw_ptr get_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            // RCU should be locked!
            assert(gc::is_locked());

            position pos( refHead );

            if ( search( refHead, val, pos, cmp )) {
                m_Stat.onFindSuccess();
                return raw_ptr( node_traits::to_value_ptr( pos.pCur ), raw_ptr_disposer( pos ));
            }

            m_Stat.onFindFailed();
            return raw_ptr( raw_ptr_disposer( pos ));
        }
        //@endcond

    protected:

        //@cond
        template <typename Q, typename Compare>
        bool search( atomic_node_ptr& refHead, const Q& val, position& pos, Compare cmp )
        {
            // RCU lock should be locked!!!
            assert( gc::is_locked());

            atomic_node_ptr * pPrev;
            marked_node_ptr pNext;
            marked_node_ptr pCur;

            back_off        bkoff;

        try_again:
            pPrev = &refHead;
            pCur = pPrev->load(memory_model::memory_order_acquire);
            pNext = nullptr;

            while ( true ) {
                if ( !pCur.ptr()) {
                    pos.pPrev = pPrev;
                    pos.pCur = nullptr;
                    pos.pNext = nullptr;
                    return false;
                }

                pNext = pCur->m_pNext.load(memory_model::memory_order_acquire);

                if ( cds_unlikely( pPrev->load(memory_model::memory_order_acquire) != pCur
                    || pNext != pCur->m_pNext.load(memory_model::memory_order_acquire )))
                {
                    bkoff();
                    goto try_again;
                }

                if ( pNext.bits()) {
                    // pCur is marked as deleted. Try to unlink it from the list
                    if ( cds_likely( pPrev->compare_exchange_weak( pCur, marked_node_ptr( pNext.ptr()), memory_model::memory_order_acquire, atomics::memory_order_relaxed ))) {
                        if ( pNext.bits() == erase_mask )
                            link_to_remove_chain( pos, pCur.ptr());
                        m_Stat.onHelpingSuccess();
                    }

                    m_Stat.onHelpingFailed();
                    goto try_again;
                }

                assert( pCur.ptr() != nullptr );
                int nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                if ( nCmp >= 0 ) {
                    pos.pPrev = pPrev;
                    pos.pCur = pCur.ptr();
                    pos.pNext = pNext.ptr();
                    return nCmp == 0;
                }
                pPrev = &( pCur->m_pNext );
                pCur = pNext;
            }
        }
        //@endcond

    private:
        //@cond
        bool insert_at_locked( position& pos, value_type& val )
        {
            // RCU lock should be locked!!!
            assert( gc::is_locked());

            while ( true ) {
                if ( search( pos.refHead, val, pos, key_comparator())) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_node( node_traits::to_node_ptr( val ), pos )) {
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                // clear next field
                node_traits::to_node_ptr( val )->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
                m_Stat.onInsertRetry();
            }
        }

        template <typename Func>
        std::pair<iterator, bool> update_at_locked( position& pos, value_type& val, Func func, bool bInsert )
        {
            // RCU should be locked!!!
            assert( gc::is_locked());

            while ( true ) {
                if ( search( pos.refHead, val, pos, key_comparator())) {
                    assert( key_comparator()( val, *node_traits::to_value_ptr( *pos.pCur )) == 0 );

                    func( false, *node_traits::to_value_ptr( *pos.pCur ), val );
                    m_Stat.onUpdateExisting();
                    return std::make_pair( iterator( pos.pCur ), false );
                }
                else {
                    if ( !bInsert ) {
                        m_Stat.onUpdateFailed();
                        return std::make_pair( end(), false );
                    }

                    if ( link_node( node_traits::to_node_ptr( val ), pos )) {
                        ++m_ItemCounter;
                        func( true, val , val );
                        m_Stat.onUpdateNew();
                        return std::make_pair( iterator( node_traits::to_node_ptr( val )), true );
                    }

                    // clear the next field
                    node_traits::to_node_ptr( val )->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
                    m_Stat.onUpdateRetry();
                }
            }
        }

        template <typename Q, typename Compare>
        const_iterator find_at_locked( position& pos, Q const& val, Compare cmp )
        {
            assert( gc::is_locked());

            if ( search( pos.refHead, val, pos, cmp )) {
                assert( pos.pCur != nullptr );
                m_Stat.onFindSuccess();
                return const_iterator( pos.pCur );
            }

            m_Stat.onFindFailed();
            return cend();
        }
        //@endcond
    };

}}  // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_MICHAEL_LIST_NOGC_H
