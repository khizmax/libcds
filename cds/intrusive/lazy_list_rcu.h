// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_LAZY_LIST_RCU_H
#define CDSLIB_INTRUSIVE_LAZY_LIST_RCU_H

#include <mutex> // unique_lock
#include <cds/intrusive/details/lazy_list_base.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/urcu/exempt_ptr.h>

namespace cds { namespace intrusive {
    namespace lazy_list {
        /// Lazy list node for \ref cds_urcu_desc "RCU"
        /**
            Template parameters:
            - Tag - a tag used to distinguish between different implementation
        */
        template <class RCU, typename Lock, typename Tag>
        struct node<cds::urcu::gc<RCU>, Lock, Tag>
        {
            typedef cds::urcu::gc<RCU>  gc  ;   ///< RCU schema
            typedef Lock        lock_type   ;   ///< Lock type
            typedef Tag         tag         ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>   marked_ptr          ;   ///< marked pointer
            typedef atomics::atomic<marked_ptr>      atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC

            atomic_marked_ptr   m_pNext ; ///< pointer to the next node in the list
            mutable lock_type   m_Lock  ; ///< Node lock

            /// Checks if node is marked
            bool is_marked() const
            {
                return m_pNext.load(atomics::memory_order_relaxed).bits() != 0;
            }

            /// Default ctor
            node()
                : m_pNext( nullptr )
            {}

            /// Clears internal fields
            void clear()
            {
                m_pNext.store( marked_ptr(), atomics::memory_order_release );
            }
        };
    }   // namespace lazy_list


    /// Lazy ordered single-linked list (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_LazyList_rcu

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>.

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p T - type to be stored in the list
        - \p Traits - type traits. See \p lazy_list::traits for explanation.
            It is possible to declare option-based list with \p %cds::intrusive::lazy_list::make_traits metafunction instead of \p Traits template
            argument.

        \par Usage
            Before including <tt><cds/intrusive/lazy_list_rcu.h></tt> you should include appropriate RCU header file,
            see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.
            For example, for \ref cds_urcu_general_buffered_gc "general-purpose buffered RCU" you should include:
            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/lazy_list_rcu.h>

            // Now, you can declare lazy list for type Foo and default traits:
            typedef cds::intrusive::LazyList<cds::urcu::gc< cds::urcu::general_buffered<> >, Foo > rcu_lazy_list;
            \endcode

    */
    template <
        typename RCU
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = lazy_list::traits
#else
        ,class Traits
#endif
    >
    class LazyList<cds::urcu::gc<RCU>, T, Traits>
    {
    public:
        typedef cds::urcu::gc<RCU> gc;      ///< RCU schema
        typedef T                  value_type;   ///< type of value stored in the list
        typedef Traits             traits;  ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key compare functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif

        typedef typename traits::disposer  disposer;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;    ///< node traits
        typedef typename lazy_list::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

        typedef typename traits::back_off     back_off;     ///< back-off strategy (not used)
        typedef typename traits::item_counter item_counter; ///< Item counting policy used
        typedef typename traits::memory_model memory_model; ///< C++ memory ordering (see \p lazy_list::traits::memory_model)
        typedef typename traits::stat         stat;         ///< Internal statistics
        typedef typename traits::rcu_check_deadlock rcu_check_deadlock; ///< Deadlock checking policy

        typedef typename gc::scoped_lock    rcu_lock ; ///< RCU scoped lock
        static constexpr const bool c_bExtractLockExternal = true; ///< Group of \p extract_xxx functions require external locking

        static_assert((std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type");

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef LazyList<
                gc
                , value_type
                , typename cds::opt::make_options< traits, Options...>::type
            >   type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = lazy_list::select_stat_wrapper< Stat >;
        //@endcond

    protected:
        node_type       m_Head;        ///< List head (dummy node)
        node_type       m_Tail;        ///< List tail (dummy node)
        item_counter    m_ItemCounter; ///< Item counter
        mutable stat    m_Stat;        ///< Internal statistics

        //@cond
        typedef typename node_type::marked_ptr  marked_node_ptr;  ///< Node marked pointer
        typedef node_type *                     auxiliary_head;   ///< Auxiliary head type (for split-list support)

        /// Position pointer for item search
        struct position {
            node_type *     pPred; ///< Previous node
            node_type *     pCur;  ///< Current node

            /// Locks nodes \p pPred and \p pCur
            void lock()
            {
                pPred->m_Lock.lock();
                pCur->m_Lock.lock();
            }

            /// Unlocks nodes \p pPred and \p pCur
            void unlock()
            {
                pCur->m_Lock.unlock();
                pPred->m_Lock.unlock();
            }
        };

        typedef std::unique_lock< position > scoped_position_lock;

        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock>   deadlock_policy;

        struct clear_and_dispose {
            void operator()( value_type * p )
            {
                assert( p != nullptr );
                clear_links( node_traits::to_node_ptr(p));
                disposer()( p );
            }
        };
        //@endcond

    public:
        /// pointer to extracted node
        using exempt_ptr = cds::urcu::exempt_ptr< gc, value_type, value_type, clear_and_dispose, void >;
        /// Type of \p get() member function return value
        typedef value_type * raw_ptr;

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class LazyList;

        protected:
            value_type * m_pNode;

            void next()
            {
                assert( m_pNode != nullptr );

                node_type * pNode = node_traits::to_node_ptr( m_pNode );
                node_type * pNext = pNode->m_pNext.load(memory_model::memory_order_acquire).ptr();
                if ( pNext != nullptr )
                    m_pNode = node_traits::to_value_ptr( pNext );
            }

            void skip_deleted()
            {
                if ( m_pNode != nullptr ) {
                    node_type * pNode = node_traits::to_node_ptr( m_pNode );

                    // Dummy tail node could not be marked
                    while ( pNode->is_marked())
                        pNode = pNode->m_pNext.load(memory_model::memory_order_acquire).ptr();

                    if ( pNode != node_traits::to_node_ptr( m_pNode ))
                        m_pNode = node_traits::to_value_ptr( pNode );
                }
            }

            iterator_type( node_type * pNode )
            {
                m_pNode = node_traits::to_value_ptr( pNode );
                skip_deleted();
            }

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
                : m_pNode( nullptr )
            {}

            iterator_type( iterator_type const& src )
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
                skip_deleted();
                return *this;
            }

            /// Post-increment
            iterator_type operator ++(int)
            {
                iterator_type i(*this);
                next();
                skip_deleted();
                return i;
            }

            iterator_type& operator = (iterator_type const& src)
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
            iterator it( &m_Head );
            ++it        ;   // skip dummy head
            return it;
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( &m_Tail );
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return get_const_begin();
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return get_const_begin();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return get_const_end();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return get_const_end();
        }
    //@}

    public:
        /// Default constructor initializes empty list
        LazyList()
        {
            m_Head.m_pNext.store( marked_node_ptr( &m_Tail ), memory_model::memory_order_relaxed );
        }

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, lazy_list::wrapped_stat<Stat>>::value >>
        explicit LazyList( Stat& st )
            : m_Stat( st )
        {
            m_Head.m_pNext.store( marked_node_ptr( &m_Tail ), memory_model::memory_order_relaxed );
        }
        //@endcond

        /// Destroys the list object
        ~LazyList()
        {
            clear();

            assert( m_Head.m_pNext.load(memory_model::memory_order_relaxed).ptr() == &m_Tail );
            m_Head.m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
        }

        /// Inserts new node
        /**
            The function inserts \p val in the list if the list does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is linked into the list, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_at( &m_Head, val );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split new item creating into two part:
            - create item with key only
            - insert new item into the list
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted.
            While the functor \p f is working the item \p val is locked.
            The user-defined functor is called only if the inserting is success.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            return insert_at( &m_Head, val, f );
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

            The functor may change non-key fields of the \p item.
            While the functor \p f is calling the item \p item is locked.

            Returns <tt> std::pair<bool, bool>  </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the list.

            The function makes RCU lock internally.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            return update_at( &m_Head, val, func, bAllowInsert );
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
            only if \p val is an item of that list, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            The function returns \p true if success and \p false otherwise.

            RCU \p synchronize method can be called. The RCU should not be locked.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            \p disposer specified in \p Traits is called for unlinked item.

            The function can throw \p cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        bool unlink( value_type& val )
        {
            return unlink_at( &m_Head, val );
        }

        /// Deletes the item from the list
        /**
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \p true.
            If the item with the key equal to \p key is not found the function return \p false.

            RCU \p synchronize method can be called. The RCU should not be locked.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            \p disposer specified in \p Traits is called for deleted item.

            The function can throw \ref cds_urcu_rcu_deadlock "cds::urcu::rcu_deadlock" exception if deadlock is encountered and
            deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( &m_Head, key, key_comparator());
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
            return erase_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>());
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

            RCU \p synchronize method can be called. The RCU should not be locked.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            \p disposer specified in \p Traits is called for deleted item.

            The function can throw \ref cds_urcu_rcu_deadlock "cds::urcu::rcu_deadlock" exception if deadlock is encountered and
            deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func func )
        {
            return erase_at( &m_Head, key, key_comparator(), func );
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
            return erase_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>(), func );
        }

        /// Extracts an item from the list
        /**
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to an item found.
            If the item is not found the function returns empty \p exempt_ptr.

            @note The function does NOT call RCU read-side lock or synchronization,
            and does NOT dispose the item found. It just unlinks the item from the list
            and returns a pointer to it.
            You should manually lock RCU before calling this function, and you should manually release
            the returned exempt pointer outside the RCU lock region before reusing returned pointer.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/lazy_list_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::intrusive::LazyList< rcu, Foo > rcu_lazy_list;

            rcu_lazy_list theList;
            // ...

            rcu_lazy_list::exempt_ptr p1;
            {
                // first, we should lock RCU
                rcu::scoped_lock sl;

                // Now, you can apply extract function
                // Note that you must not delete the item found inside the RCU lock
                p1 = theList.extract(  10 )
                if ( p1 ) {
                    // do something with p1
                    ...
                }
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
            return exempt_ptr( extract_at( &m_Head, key, key_comparator()));
        }

        /// Extracts an item from the list using \p pred predicate for searching
        /**
            This function is the analog for \p extract(Q const&).

            The \p pred is a predicate used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as \ref key_comparator.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( extract_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>()));
        }

        /// Finds the key \p key
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

            The functor may change non-key fields of \p item.
            While the functor \p f is calling the item found \p item is locked.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_at( const_cast<node_type *>(&m_Head), key, key_comparator(), f );
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \p find( Q&, Func ) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( const_cast<node_type *>( &m_Head ), key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( const_cast<node_type *>(&m_Head), key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@endcond

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key ) const
        {
            return find_at( const_cast<node_type *>( &m_Head ), key, key_comparator());
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key ) const
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is an analog of \p contains( Q const& ) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return find_at( const_cast<node_type *>( &m_Head ), key, cds::opt::details::make_comparator_from_less<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred ) const
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key and return the item found
        /** \anchor cds_intrusive_LazyList_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::intrusive::LazyList< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > ord_list;
            ord_list theList;
            // ...
            {
                // Lock RCU
                typename ord_list::rcu_lock lock;

                foo * pVal = theList.get( 5 );
                if ( pVal ) {
                    // Deal with pVal
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // pVal can be retired by disposer at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename Q>
        value_type * get( Q const& key ) const
        {
            return get_at( const_cast<node_type *>( &m_Head ), key, key_comparator());
        }

        /// Finds the key \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_LazyList_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        value_type * get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return get_at( const_cast<node_type *>( &m_Head ), key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Clears the list using default disposer
        /**
            The function clears the list using default (provided in class template) disposer functor.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            The function can throw \p cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        void clear()
        {
            if( !empty()) {
                deadlock_policy::check();

                node_type * pHead;
                for (;;) {
                    {
                        rcu_lock l;
                        pHead = m_Head.m_pNext.load(memory_model::memory_order_acquire).ptr();
                        if ( pHead == &m_Tail )
                            break;

                        m_Head.m_Lock.lock();
                        pHead->m_Lock.lock();

                        if ( m_Head.m_pNext.load(memory_model::memory_order_relaxed).all() == pHead )
                            unlink_node( &m_Head, pHead, &m_Head );

                        pHead->m_Lock.unlock();
                        m_Head.m_Lock.unlock();
                    }

                    --m_ItemCounter;
                    dispose_node( pHead );
                }
            }
        }

        /// Checks if the list is empty
        bool empty() const
        {
            return m_Head.m_pNext.load(memory_model::memory_order_relaxed).ptr() == &m_Tail;
        }

        /// Returns list's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the list
            is empty. To check list emptiness use \ref empty() method.
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
            pNode->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
        }

        static void dispose_node( node_type * pNode )
        {
            assert( pNode );
            assert( !gc::is_locked());

            gc::template retire_ptr<clear_and_dispose>( node_traits::to_value_ptr( *pNode ));
        }

        static void link_node( node_type * pNode, node_type * pPred, node_type * pCur )
        {
            assert( pPred->m_pNext.load( memory_model::memory_order_relaxed ).ptr() == pCur );
            link_checker::is_empty( pNode );

            pNode->m_pNext.store( marked_node_ptr( pCur ), memory_model::memory_order_relaxed );
            pPred->m_pNext.store( marked_node_ptr( pNode ), memory_model::memory_order_release );
        }

        void unlink_node( node_type * pPred, node_type * pCur, node_type * pHead )
        {
            assert( pPred->m_pNext.load( memory_model::memory_order_relaxed ).ptr() == pCur );
            assert( pCur != &m_Tail );

            node_type * pNext = pCur->m_pNext.load( memory_model::memory_order_relaxed ).ptr();
            pCur->m_pNext.store( marked_node_ptr( pHead, 1 ), memory_model::memory_order_relaxed ); // logical deletion + back-link for search
            pPred->m_pNext.store( marked_node_ptr( pNext ), memory_model::memory_order_release ); // physically deleting
        }

        // split-list support
        bool insert_aux_node( node_type * pNode )
        {
            return insert_aux_node( &m_Head, pNode );
        }

        // split-list support
        bool insert_aux_node( node_type * pHead, node_type * pNode )
        {
            assert( pHead != nullptr );
            assert( pNode != nullptr );

            // Hack: convert node_type to value_type.
            // Actually, an auxiliary node should not be converted to value_type
            // We assume that comparator can correctly distinguish aux and regular node.
            return insert_at( pHead, *node_traits::to_value_ptr( pNode ));
        }

        bool insert_at( node_type * pHead, value_type& val )
        {
            rcu_lock l;
            return insert_at_locked( pHead, val );
        }

        template <typename Func>
        bool insert_at( node_type * pHead, value_type& val, Func f )
        {
            position pos;
            key_comparator  cmp;

            rcu_lock l;
            while ( true ) {
                search( pHead, val, pos );
                {
                    scoped_position_lock sl( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // failed: key already in list
                            m_Stat.onInsertFailed();
                            return false;
                        }

                        f( val );
                        link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                        break;
                    }
                }

                m_Stat.onInsertRetry();
            }

            ++m_ItemCounter;
            m_Stat.onInsertSuccess();
            return true;
        }

        iterator insert_at_( node_type * pHead, value_type& val )
        {
            rcu_lock l;
            if ( insert_at_locked( pHead, val ))
                return iterator( node_traits::to_node_ptr( val ));
            return end();
        }


        template <typename Func>
        std::pair<iterator, bool> update_at_( node_type * pHead, value_type& val, Func func, bool bAllowInsert )
        {
            rcu_lock l;
            return update_at_locked( pHead, val, func, bAllowInsert );
        }

        template <typename Func>
        std::pair<bool, bool> update_at( node_type * pHead, value_type& val, Func func, bool bAllowInsert )
        {
            rcu_lock l;
            std::pair<iterator, bool> ret = update_at_locked( pHead, val, func, bAllowInsert );
            return std::make_pair( ret.first != end(), ret.second );
        }

        bool unlink_at( node_type * pHead, value_type& val )
        {
            position pos;
            key_comparator  cmp;
            deadlock_policy::check();

            while ( true ) {
                int nResult = 0;
                {
                    rcu_lock l;
                    search( pHead, val, pos );
                    {
                        scoped_position_lock alp( pos );
                        if ( validate( pos.pPred, pos.pCur )) {
                            if ( pos.pCur != &m_Tail
                                && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0
                                && node_traits::to_value_ptr( pos.pCur ) == &val )
                            {
                                // item found
                                unlink_node( pos.pPred, pos.pCur, pHead );
                                nResult = 1;
                            }
                            else
                                nResult = -1;
                        }
                    }
                }

                if ( nResult ) {
                    if ( nResult > 0 ) {
                        --m_ItemCounter;
                        dispose_node( pos.pCur );
                        m_Stat.onEraseSuccess();
                        return true;
                    }

                    m_Stat.onEraseFailed();
                    return false;
                }

                m_Stat.onEraseRetry();
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type * const pHead, Q const& val, Compare cmp, Func f, position& pos )
        {
            deadlock_policy::check();

            while ( true ) {
                int nResult = 0;
                {
                    rcu_lock l;
                    search( pHead, val, pos, cmp );
                    {
                        scoped_position_lock alp( pos );
                        if ( validate( pos.pPred, pos.pCur )) {
                            if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                                // key found
                                unlink_node( pos.pPred, pos.pCur, pHead );
                                f( *node_traits::to_value_ptr( *pos.pCur ));
                                nResult = 1;
                            }
                            else
                                nResult = -1;
                        }
                    }
                }

                if ( nResult ) {
                    if ( nResult > 0 ) {
                        --m_ItemCounter;
                        dispose_node( pos.pCur );
                        m_Stat.onEraseSuccess();
                        return true;
                    }

                    m_Stat.onEraseFailed();
                    return false;
                }

                m_Stat.onEraseRetry();
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type * pHead, Q const& val, Compare cmp, Func f )
        {
            position pos;
            return erase_at( pHead, val, cmp, f, pos );
        }

        template <typename Q, typename Compare>
        bool erase_at( node_type * pHead, Q const& val, Compare cmp )
        {
            position pos;
            return erase_at( pHead, val, cmp, [](value_type const&){}, pos );
        }

        template <typename Q, typename Compare>
        value_type * extract_at( node_type * const pHead, Q const& val, Compare cmp )
        {
            position pos;
            assert( gc::is_locked()) ; // RCU must be locked

            while ( true ) {
                search( pHead, val, pos, cmp );
                int nResult = 0;
                {
                    scoped_position_lock alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // key found
                            unlink_node( pos.pPred, pos.pCur, pHead );
                            nResult = 1;
                        }
                        else {
                            nResult = -1;
                        }
                    }
                }

                if ( nResult ) {
                    if ( nResult > 0 ) {
                        --m_ItemCounter;
                        m_Stat.onEraseSuccess();
                        return node_traits::to_value_ptr( pos.pCur );
                    }

                    m_Stat.onEraseFailed();
                    return nullptr;
                }

                m_Stat.onEraseRetry();
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( node_type * pHead, Q& val, Compare cmp, Func f ) const
        {
            position pos;

            rcu_lock l;
            search( pHead, val, pos, cmp );
            if ( pos.pCur != &m_Tail ) {
                std::unique_lock< typename node_type::lock_type> al( pos.pCur->m_Lock );
                if ( cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                    f( *node_traits::to_value_ptr( *pos.pCur ), val );
                    m_Stat.onFindSuccess();
                    return true;
                }
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare>
        bool find_at( node_type * pHead, Q& val, Compare cmp ) const
        {
            rcu_lock l;
            return find_at_( pHead, val, cmp ) != end();
        }

        template <typename Q, typename Compare>
        const_iterator find_at_( node_type * pHead, Q& val, Compare cmp ) const
        {
            assert( gc::is_locked());

            position pos;

            search( pHead, val, pos, cmp );
            if ( pos.pCur != &m_Tail ) {
                if ( cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                    m_Stat.onFindSuccess();
                    return const_iterator( pos.pCur );
                }
            }

            m_Stat.onFindFailed();
            return end();
        }

        template <typename Q, typename Compare>
        value_type * get_at( node_type * pHead, Q const& val, Compare cmp ) const
        {
            value_type * pFound = nullptr;
            return find_at( pHead, val, cmp, [&pFound](value_type& found, Q const& ) { pFound = &found; } )
                ? pFound : nullptr;
        }

        //@endcond

    protected:
        //@cond
        template <typename Q>
        void search( node_type * const pHead, Q const& key, position& pos ) const
        {
            search( pHead, key, pos, key_comparator());
        }

        template <typename Q, typename Compare>
        void search( node_type * const pHead, Q const& key, position& pos, Compare cmp ) const
        {
            // RCU should be locked
            assert( gc::is_locked());

            node_type const* pTail = &m_Tail;

            marked_node_ptr pCur(pHead);
            marked_node_ptr pPrev(pHead);

            while ( pCur != pTail && ( pCur == pHead || cmp( *node_traits::to_value_ptr( *pCur.ptr()), key ) < 0 )) {
                pPrev = pCur;
                pCur = pCur->m_pNext.load(memory_model::memory_order_acquire);
                if ( pCur.bits())
                    pPrev = pCur = pHead;
            }

            pos.pCur = pCur.ptr();
            pos.pPred = pPrev.ptr();
        }

        bool validate( node_type * pPred, node_type * pCur ) noexcept
        {
            if ( validate_link( pPred, pCur )) {
                m_Stat.onValidationSuccess();
                return true;
            }

            m_Stat.onValidationFailed();
            return false;
        }

        static bool validate_link( node_type * pPred, node_type * pCur ) noexcept
        {
            // RCU lock should be locked
            assert( gc::is_locked());

            return !pPred->is_marked()
                && !pCur->is_marked()
                && pPred->m_pNext.load(memory_model::memory_order_relaxed) == pCur;
        }

        //@endcond

    private:
        //@cond
        bool insert_at_locked( node_type * pHead, value_type& val )
        {
            // RCU lock should be locked
            assert( gc::is_locked());

            position pos;
            key_comparator  cmp;

            while ( true ) {
                search( pHead, val, pos );
                {
                    scoped_position_lock alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // failed: key already in list
                            m_Stat.onInsertFailed();
                            return false;
                        }

                        link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                        break;
                    }
                }

                m_Stat.onInsertRetry();
            }

            ++m_ItemCounter;
            m_Stat.onInsertSuccess();
            return true;

        }

        template <typename Func>
        std::pair<iterator, bool> update_at_locked( node_type * pHead, value_type& val, Func func, bool bAllowInsert )
        {
            // RCU lock should be locked
            assert( gc::is_locked());

            position pos;
            key_comparator  cmp;

            while ( true ) {
                search( pHead, val, pos );
                {
                    scoped_position_lock alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // key already in the list

                            func( false, *node_traits::to_value_ptr( *pos.pCur ), val );
                            m_Stat.onUpdateExisting();
                            return std::make_pair( iterator( pos.pCur ), false );
                        }
                        else {
                            // new key
                            if ( !bAllowInsert ) {
                                m_Stat.onUpdateFailed();
                                return std::make_pair( end(), false );
                            }

                            func( true, val, val );
                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                            break;
                        }
                    }
                }

                m_Stat.onUpdateRetry();
            }

            ++m_ItemCounter;
            m_Stat.onUpdateNew();
            return std::make_pair( iterator( node_traits::to_node_ptr( val )), true );
        }
        //@endcond

    private:
        //@cond
        const_iterator get_const_begin() const
        {
            const_iterator it( const_cast<node_type *>(&m_Head));
            ++it;   // skip dummy head
            return it;
        }
        const_iterator get_const_end() const
        {
            return const_iterator( const_cast<node_type *>(&m_Tail));
        }
        //@endcond
    };

}}  // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_LAZY_LIST_RCU_H
