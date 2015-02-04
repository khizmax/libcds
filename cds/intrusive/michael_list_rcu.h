//$$CDS-header$$

#ifndef CDSLIB_INTRUSIVE_MICHAEL_LIST_RCU_H
#define CDSLIB_INTRUSIVE_MICHAEL_LIST_RCU_H

#include <cds/intrusive/details/michael_list_base.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/details/make_const_type.h>
#include <cds/urcu/exempt_ptr.h>

namespace cds { namespace intrusive {

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

        typedef cds::urcu::gc<RCU>                     gc;           ///< RCU schema
        typedef typename traits::back_off              back_off;     ///< back-off strategy
        typedef typename traits::item_counter          item_counter; ///< Item counting policy used
        typedef typename traits::memory_model          memory_model; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::rcu_check_deadlock    rcu_check_deadlock; ///< Deadlock checking policy

        typedef typename gc::scoped_lock    rcu_lock ;  ///< RCU scoped lock
        static CDS_CONSTEXPR const bool c_bExtractLockExternal = true; ///< Group of \p extract_xxx functions require external locking

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
        //@endcond

    protected:
        typedef typename node_type::marked_ptr         marked_node_ptr ; ///< Marked node pointer
        typedef typename node_type::atomic_marked_ptr  atomic_node_ptr ; ///< Atomic node pointer
        typedef atomic_node_ptr                 auxiliary_head      ;   ///< Auxiliary head type (for split-list support)

        atomic_node_ptr     m_pHead         ;   ///< Head pointer
        item_counter        m_ItemCounter   ;   ///< Item counter

        //@cond
        /// Position pointer for item search
        struct position {
            atomic_node_ptr * pPrev ;   ///< Previous node
            node_type * pCur        ;   ///< Current node
            node_type * pNext       ;   ///< Next node
        };

        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock>   check_deadlock_policy;

        static void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
        }

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
        using exempt_ptr = cds::urcu::exempt_ptr< gc, value_type, value_type, clear_and_dispose, void >; ///< pointer to extracted node

    protected:
        //@cond

        static void dispose_node( node_type * pNode )
        {
            assert( pNode );
            assert( !gc::is_locked() );

            gc::template retire_ptr<clear_and_dispose>( node_traits::to_value_ptr( *pNode ) );
        }

        bool link_node( node_type * pNode, position& pos )
        {
            assert( pNode != nullptr );
            link_checker::is_empty( pNode );

            marked_node_ptr p( pos.pCur );
            pNode->m_pNext.store( p, memory_model::memory_order_relaxed );
            return pos.pPrev->compare_exchange_strong( p, marked_node_ptr(pNode), memory_model::memory_order_release, atomics::memory_order_relaxed );
        }

        bool unlink_node( position& pos )
        {
            // Mark the node (logical deleting)
            marked_node_ptr next(pos.pNext, 0);
            if ( pos.pCur->m_pNext.compare_exchange_strong( next, marked_node_ptr(pos.pNext, 1), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                marked_node_ptr cur(pos.pCur);
                if ( pos.pPrev->compare_exchange_strong( cur, marked_node_ptr( pos.pNext ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    return true;
                next |= 1;
                CDS_VERIFY( pos.pCur->m_pNext.compare_exchange_strong( next, next ^ 1, memory_model::memory_order_release, atomics::memory_order_relaxed ));
            }
            return false;
        }
        //@endcond

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
        /// Forward iterator
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

    public:
        /// Default constructor initializes empty list
        MichaelList()
            : m_pHead( nullptr )
        {
            static_assert( (std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type" );
        }

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

        /// Ensures that the \p item exists in the list
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val not found in the list, then \p val is inserted into the list.
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
            - \p val - argument \p val passed into the \p ensure function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns <tt> std::pair<bool, bool>  </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the list.

            The function makes RCU lock internally.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return ensure_at( m_pHead, val, func );
        }

        /// Unlinks the item \p val from the list
        /**
            The function searches the item \p val in the list and unlink it from the list
            if it is found and it is equal to \p val.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p val is an item of that list, i.e. the pointer to the item found
            is equal to <tt> &val </tt>.

            The function returns \p true if success and \p false otherwise.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            The function can throw cds::urcu::rcu_deadlock exception if deadlock is encountered and
            deadlock checking policy is opt::v::rcu_throw_deadlock.
        */
        bool unlink( value_type& val )
        {
            return unlink_at( m_pHead, val );
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_MichaelList_rcu_erase_val
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \p true.
            If the item with the key equal to \p key is not found the function return \p false.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer call can be deferred.

            The function can throw \ref cds_urcu_rcu_deadlock "cds::urcu::rcu_deadlock" exception if a deadlock is detected and
            the deadlock checking policy is \p opt::v::rcu_throw_deadlock.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( m_pHead, key, key_comparator() );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_rcu_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>() );
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_MichaelList_rcu_erase_func
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
            The function is an analog of \ref cds_intrusive_MichaelList_rcu_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func func )
        {
            CDS_UNUSED( pred );
            return erase_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), func );
        }

        /// Extracts an item from the list
        /**
        @anchor cds_intrusive_MichaelList_rcu_extract
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If \p key is not found the function returns an empty \p exempt_ptr.

            @note The function does NOT call RCU read-side lock or synchronization,
            and does NOT dispose the item found. It just unlinks the item from the list
            and returns a pointer to item found.
            You should lock RCU before calling this function, and you should manually release
            \p dest exempt pointer outside the RCU lock before reusing the pointer.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/intrusive/michael_list_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::intrusive::MichaelList< rcu, Foo > rcu_michael_list;

            rcu_michael_list theList;
            // ...

            rcu_michael_list::exempt_ptr p1;
            {
                // first, we should lock RCU
                rcu::scoped_lock sl;

                // Now, you can apply extract function
                // Note that you must not delete the item found inside the RCU lock
                p1 = theList.extract( 10 )
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
            return exempt_ptr( extract_at( m_pHead, key, key_comparator() ));
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
            return exempt_ptr( extract_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>() ));
        }

        /// Find the key \p val
        /** \anchor cds_intrusive_MichaelList_rcu_find_func
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
        bool find( Q& key, Func f ) const
        {
            return find_at( const_cast<atomic_node_ptr&>(m_pHead), key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_at( const_cast<atomic_node_ptr&>(m_pHead), key, key_comparator(), f );
        }
        //@endcond

        /// Finds \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( const_cast<atomic_node_ptr&>( m_pHead ), key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( const_cast<atomic_node_ptr&>(m_pHead), key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@endcond

        /// Finds \p key
        /** \anchor cds_intrusive_MichaelList_rcu_find_val
            The function searches the item with key equal to \p key
            and returns \p true if \p val found or \p false otherwise.
        */
        template <typename Q>
        bool find( Q const& key ) const
        {
            return find_at( const_cast<atomic_node_ptr&>( m_pHead ), key, key_comparator() );
        }

        /// Finds \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_rcu_find_val "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool find_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return find_at( const_cast<atomic_node_ptr&>( m_pHead ), key, cds::opt::details::make_comparator_from_less<Less>() );
        }

        /// Finds \p key and return the item found
        /** \anchor cds_intrusive_MichaelList_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::intrusive::MichaelList< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > ord_list;
            ord_list theList;
            // ...
            {
                // Lock RCU
                ord_list::rcu_lock lock;

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
            return get_at( const_cast<atomic_node_ptr&>( m_pHead ), key, key_comparator());
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
        value_type * get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return get_at( const_cast<atomic_node_ptr&>( m_pHead ), key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Clears the list using default disposer
        /**
            The function clears the list using default (provided by \p Traits class template argument) disposer functor.

            RCU \p synchronize method can be called.
            Note that depending on RCU type used the \ref disposer invocation can be deferred.

            The function can throw cds::urcu::rcu_deadlock exception if an deadlock is encountered and
            deadlock checking policy is opt::v::rcu_throw_deadlock.
        */
        void clear()
        {
            if( !empty() ) {
                check_deadlock_policy::check();

                marked_node_ptr pHead;
                for (;;) {
                    {
                        rcu_lock l;
                        pHead = m_pHead.load(memory_model::memory_order_consume);
                        if ( !pHead.ptr() )
                            break;
                        marked_node_ptr pNext( pHead->m_pNext.load(memory_model::memory_order_relaxed) );
                        if ( !pHead->m_pNext.compare_exchange_weak( pNext, pNext | 1, memory_model::memory_order_acquire, memory_model::memory_order_relaxed ))
                            continue;
                        if ( !m_pHead.compare_exchange_weak( pHead, marked_node_ptr(pNext.ptr()), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                            continue;
                    }

                    --m_ItemCounter;
                    dispose_node( pHead.ptr() );
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

    protected:
        //@cond
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
            return insert_at( refHead, *node_traits::to_value_ptr( pNode ) );
        }

        bool insert_at( atomic_node_ptr& refHead, value_type& val, bool bLock = true )
        {
            link_checker::is_empty( node_traits::to_node_ptr( val ) );
            position pos;

            rcu_lock l( bLock );
            while ( true ) {
                if ( search( refHead, val, pos, key_comparator() ) )
                    return false;

                if ( link_node( node_traits::to_node_ptr( val ), pos ) ) {
                    ++m_ItemCounter;
                    return true;
                }

                // clear next field
                node_traits::to_node_ptr( val )->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
            }
        }

        template <typename Func>
        bool insert_at( atomic_node_ptr& refHead, value_type& val, Func f )
        {
            link_checker::is_empty( node_traits::to_node_ptr( val ) );
            position pos;

            rcu_lock l;
            while ( true ) {
                if ( search( refHead, val, pos, key_comparator() ) )
                    return false;

                if ( link_node( node_traits::to_node_ptr( val ), pos ) ) {
                    f( val );
                    ++m_ItemCounter;
                    return true;
                }

                // clear next field
                node_traits::to_node_ptr( val )->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
            }
        }

        iterator insert_at_( atomic_node_ptr& refHead, value_type& val, bool bLock = true )
        {
            rcu_lock l( bLock );
            if ( insert_at( refHead, val, false ))
                return iterator( node_traits::to_node_ptr( val ));
            return end();
        }

        template <typename Func>
        std::pair<iterator, bool> ensure_at_( atomic_node_ptr& refHead, value_type& val, Func func, bool bLock = true )
        {
            position pos;

            rcu_lock l( bLock );
            while ( true ) {
                if ( search( refHead, val, pos, key_comparator() ) ) {
                    assert( key_comparator()( val, *node_traits::to_value_ptr( *pos.pCur ) ) == 0 );

                    func( false, *node_traits::to_value_ptr( *pos.pCur ), val );
                    return std::make_pair( iterator( pos.pCur ), false );
                }
                else {
                    link_checker::is_empty( node_traits::to_node_ptr( val ) );

                    if ( link_node( node_traits::to_node_ptr( val ), pos ) ) {
                        ++m_ItemCounter;
                        func( true, val , val );
                        return std::make_pair( iterator( node_traits::to_node_ptr( val )), true );
                    }

                    // clear the next field
                    node_traits::to_node_ptr( val )->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
                }
            }
        }

        template <typename Func>
        std::pair<bool, bool> ensure_at( atomic_node_ptr& refHead, value_type& val, Func func, bool bLock = true )
        {
            rcu_lock l( bLock );
            std::pair<iterator, bool> ret = ensure_at_( refHead, val, func, false );
            return std::make_pair( ret.first != end(), ret.second );
        }

        bool unlink_at( atomic_node_ptr& refHead, value_type& val )
        {
            position pos;
            back_off bkoff;
            check_deadlock_policy::check();

            for (;;) {
                {
                    rcu_lock l;
                    if ( !search( refHead, val, pos, key_comparator() ) || node_traits::to_value_ptr( *pos.pCur ) != &val )
                        return false;
                    if ( !unlink_node( pos )) {
                        bkoff();
                        continue;
                    }
                }

                --m_ItemCounter;
                dispose_node( pos.pCur );
                return true;
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( atomic_node_ptr& refHead, Q const& val, Compare cmp, Func f, position& pos )
        {
            back_off bkoff;
            check_deadlock_policy::check();

            for (;;) {
                {
                    rcu_lock l;
                    if ( !search( refHead, val, pos, cmp ) )
                        return false;
                    if ( !unlink_node( pos )) {
                        bkoff();
                        continue;
                    }
                }

                f( *node_traits::to_value_ptr( *pos.pCur ) );
                --m_ItemCounter;
                dispose_node( pos.pCur );
                return true;
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( atomic_node_ptr& refHead, Q const& val, Compare cmp, Func f )
        {
            position pos;
            return erase_at( refHead, val, cmp, f, pos );
        }

        template <typename Q, typename Compare>
        bool erase_at( atomic_node_ptr& refHead, const Q& val, Compare cmp )
        {
            position pos;
            return erase_at( refHead, val, cmp, [](value_type const&){}, pos );
        }

        template <typename Q, typename Compare >
        value_type * extract_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos;
            back_off bkoff;
            assert( gc::is_locked() )  ;   // RCU must be locked!!!

            for (;;) {
                if ( !search( refHead, val, pos, cmp ) )
                    return nullptr;
                if ( !unlink_node( pos )) {
                    bkoff();
                    continue;
                }

                --m_ItemCounter;
                return node_traits::to_value_ptr( pos.pCur );
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( atomic_node_ptr& refHead, Q& val, Compare cmp, Func f, bool bLock = true ) const
        {
            position pos;

            rcu_lock l( bLock );
            if ( search( refHead, val, pos, cmp ) ) {
                assert( pos.pCur != nullptr );
                f( *node_traits::to_value_ptr( *pos.pCur ), val );
                return true;
            }
            return false;
        }

        template <typename Q, typename Compare>
        bool find_at( atomic_node_ptr& refHead, Q const& val, Compare cmp ) const
        {
            rcu_lock l;
            return find_at_( refHead, val, cmp ) != end();
        }

        template <typename Q, typename Compare>
        value_type * get_at( atomic_node_ptr& refHead, Q const& val, Compare cmp ) const
        {
            value_type * pFound = nullptr;
            return find_at( refHead, val, cmp,
                [&pFound](value_type& found, Q const& ) { pFound = &found; } )
                ? pFound : nullptr;
        }

        template <typename Q, typename Compare>
        const_iterator find_at_( atomic_node_ptr& refHead, Q const& val, Compare cmp ) const
        {
            assert( gc::is_locked() );
            position pos;

            if ( search( refHead, val, pos, cmp ) ) {
                assert( pos.pCur != nullptr );
                return const_iterator( pos.pCur );
            }
            return end();
        }

        //@endcond

    protected:

        //@cond
        template <typename Q, typename Compare>
        bool search( atomic_node_ptr& refHead, const Q& val, position& pos, Compare cmp ) const
        {
            // RCU lock should be locked!!!
            assert( gc::is_locked() );

            atomic_node_ptr * pPrev;
            marked_node_ptr pNext;
            marked_node_ptr pCur;

            back_off        bkoff;

        try_again:
            pPrev = &refHead;
            pCur = pPrev->load(memory_model::memory_order_acquire);
            pNext = nullptr;

            while ( true ) {
                if ( !pCur.ptr() ) {
                    pos.pPrev = pPrev;
                    pos.pCur = pCur.ptr();
                    pos.pNext = pNext.ptr();
                    return false;
                }

                pNext = pCur->m_pNext.load(memory_model::memory_order_relaxed);

                if ( pPrev->load(memory_model::memory_order_acquire) != pCur
                    || pNext != pCur->m_pNext.load(memory_model::memory_order_acquire)
                    || pNext.bits() != 0 )  // pNext contains deletion mark for pCur
                {
                    // if pCur is marked as deleted (pNext.bits() != 0)
                    // we wait for physical removal.
                    // Helping technique is not suitable for RCU since it requires
                    // that the RCU should be in unlocking state.
                    bkoff();
                    goto try_again;
                }

                assert( pCur.ptr() != nullptr );
                int nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr() ), val );
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
    };

}}  // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_MICHAEL_LIST_NOGC_H
