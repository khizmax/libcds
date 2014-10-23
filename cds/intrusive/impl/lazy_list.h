//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_IMPL_LAZY_LIST_H
#define __CDS_INTRUSIVE_IMPL_LAZY_LIST_H

#include <mutex>        // unique_lock
#include <cds/intrusive/details/lazy_list_base.h>
#include <cds/gc/guarded_ptr.h>


namespace cds { namespace intrusive {

    /// Lazy ordered single-linked list
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_LazyList_hp

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>.

        Source:
            - [2005] Steve Heller, Maurice Herlihy, Victor Luchangco, Mark Moir, William N. Scherer III, and Nir Shavit
              "A Lazy Concurrent List-Based Set Algorithm"

        The lazy list is based on an optimistic locking scheme for inserts and removes,
        eliminating the need to use the equivalent of an atomically markable
        reference. It also has a novel wait-free membership \p find operation
        that does not need to perform cleanup operations and is more efficient.

        Template arguments:
        - \p GC - Garbage collector used. Note the \p GC must be the same as the GC used for item type \p T (see lazy_list::node).
        - \p T - type to be stored in the list. The type must be based on lazy_list::node (for lazy_list::base_hook)
            or it must have a member of type lazy_list::node (for lazy_list::member_hook).
        - \p Traits - type traits. See lazy_list::type_traits for explanation.

        It is possible to declare option-based list with cds::intrusive::lazy_list::make_traits metafunction istead of \p Traits template
        argument. For example, the following traits-based declaration of gc::HP lazy list
        \code
        #include <cds/intrusive/lazy_list_hp.h>
        // Declare item stored in your list
        struct item: public cds::intrusive::lazy_list::node< cds::gc::HP >
        { ... };

        // Declare comparator for the item
        struct my_compare { ... }

        // Declare type_traits
        struct my_traits: public cds::intrusive::lazy_list::type_traits
        {
            typedef cds::intrusive::lazy_list::base_hook< cds::opt::gc< cds::gc::HP > >   hook;
            typedef my_compare compare;
        };

        // Declare traits-based list
        typedef cds::intrusive::LazyList< cds::gc::HP, item, my_traits >     traits_based_list;
        \endcode

        is equivalent for the following option-based list
        \code
        #include <cds/intrusive/lazy_list_hp.h>

        // item struct and my_compare are the same

        // Declare option-based list
        typedef cds::intrusive::LazyList< cds::gc::HP, item,
            typename cds::intrusive::lazy_list::make_traits<
                cds::intrusive::opt::hook< cds::intrusive::lazy_list::base_hook< cds::opt::gc< cds::gc::HP > > >    // hook option
                ,cds::intrusive::opt::compare< my_compare >     // item comparator option
            >::type
        >     option_based_list;
        \endcode

        Template argument list \p Options of cds::intrusive::lazy_list::make_traits metafunction are:
        - opt::hook - hook used. Possible values are: lazy_list::base_hook, lazy_list::member_hook, lazy_list::traits_hook.
            If the option is not specified, <tt>lazy_list::base_hook<></tt> and gc::HP is used.
        - opt::compare - key comparison functor. No default functor is provided.
            If the option is not specified, the opt::less is used.
        - opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::Default is used.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. Due the nature
            of GC schema the disposer may be called asynchronously.
        - opt::link_checker - the type of node's link fields checking. Default is \ref opt::debug_check_link
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter that means no item counting.
        - opt::allocator - an allocator needed for dummy head and tail nodes. Default is \ref CDS_DEFAULT_ALLOCATOR.
            The option applies only to gc::HRC garbage collector.
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).

        \par Usage
        There are different specializations of this template for each garbage collecting schema used.
        You should select GC needed and include appropriate .h-file:
        - for gc::HP: \code #include <cds/intrusive/lazy_list_hp.h> \endcode
        - for gc::DHP: \code #include <cds/intrusive/lazy_list_dhp.h> \endcode
        - for gc::nogc: \code #include <cds/intrusive/lazy_list_nogc.h> \endcode
        - for \ref cds_urcu_type "RCU" - see \ref cds_intrusive_LazyList_rcu "LazyList RCU specialization"

        Then, you should incorporate lazy_list::node into your struct \p T and provide
        appropriate lazy_list::type_traits::hook in your \p Traits template parameters. Usually, for \p Traits
        a struct based on lazy_list::type_traits should be defined.

        Example for gc::PTB and base hook:
        \code
        // Include GC-related lazy list specialization
        #include <cds/intrusive/lazy_list_dhp.h>

        // Data stored in lazy list
        struct my_data: public cds::intrusive::lazy_list::node< cds::gc::PTB >
        {
            // key field
            std::string     strKey;

            // other data
            // ...
        };

        // my_data comparing functor
        struct compare {
            int operator()( const my_data& d1, const my_data& d2 )
            {
                return d1.strKey.compare( d2.strKey );
            }

            int operator()( const my_data& d, const std::string& s )
            {
                return d.strKey.compare(s);
            }

            int operator()( const std::string& s, const my_data& d )
            {
                return s.compare( d.strKey );
            }
        };


        // Declare type_traits
        struct my_traits: public cds::intrusive::lazy_list::type_traits
        {
            typedef cds::intrusive::lazy_list::base_hook< cds::opt::gc< cds::gc::PTB > >   hook;
            typedef my_data_cmp compare;
        };

        // Declare list type
        typedef cds::intrusive::LazyList< cds::gc::PTB, my_data, my_traits >     traits_based_list;
        \endcode

        Equivalent option-based code:
        \code
        // GC-related specialization
        #include <cds/intrusive/lazy_list_dhp.h>

        struct my_data {
            // see above
        };
        struct compare {
            // see above
        };

        // Declare option-based list
        typedef cds::intrusive::LazyList< cds::gc::PTB
            ,my_data
            , typename cds::intrusive::lazy_list::make_traits<
                cds::intrusive::opt::hook< cds::intrusive::lazy_list::base_hook< cds::opt::gc< cds::gc::PTB > > >
                ,cds::intrusive::opt::compare< my_data_cmp >
            >::type
        > option_based_list;

        \endcode
    */
    template <
        class GC
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = lazy_list::type_traits
#else
        ,class Traits
#endif
    >
    class LazyList
    {
    public:
        typedef T       value_type      ;   ///< type of value stored in the list
        typedef Traits  options         ;   ///< Traits template parameter

        typedef typename options::hook      hook        ;   ///< hook type
        typedef typename hook::node_type    node_type   ;   ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key comparison functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, options >::type key_comparator;
#   endif

        typedef typename options::disposer  disposer    ;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename lazy_list::get_link_checker< node_type, options::link_checker >::type link_checker   ;   ///< link checker

        typedef GC  gc          ;   ///< Garbage collector
        typedef typename options::back_off  back_off    ;   ///< back-off strategy
        typedef typename options::item_counter item_counter ;   ///< Item counting policy used
        typedef typename options::memory_model  memory_model;   ///< C++ memory ordering (see lazy_list::type_traits::memory_model)

        typedef cds::gc::guarded_ptr< gc, value_type > guarded_ptr; ///< Guarded pointer

        //@cond
        // Rebind options (split-list support)
        template <typename... Options>
        struct rebind_options {
            typedef LazyList<
                gc
                , value_type
                , typename cds::opt::make_options< options, Options...>::type
            >   type;
        };
        //@endcond

    protected:
        typedef typename node_type::marked_ptr          marked_node_ptr ;   ///< Node marked pointer
        typedef node_type *     auxiliary_head   ;   ///< Auxiliary head type (for split-list support)

    protected:
        //@cond
        typedef lazy_list::boundary_nodes<
            gc
            ,typename opt::select_default< typename options::boundary_node_type, node_type >::type
            ,typename options::allocator
        >   boundary_nodes;
        boundary_nodes  m_Boundary    ;   ///< Head & tail dummy nodes

        node_type *     head()
        {
            return m_Boundary.head();
        }
        node_type const * head() const
        {
            return m_Boundary.head();
        }
        node_type * tail()
        {
            return m_Boundary.tail();
        }
        node_type const * tail() const
        {
            return m_Boundary.tail();
        }
        //@endcond

        item_counter    m_ItemCounter   ;   ///< Item counter

        //@cond
        struct clean_disposer {
            void operator()( value_type * p )
            {
                lazy_list::node_cleaner<gc, node_type, memory_model>()( node_traits::to_node_ptr( p ) );
                disposer()( p );
            }
        };

        /// Position pointer for item search
        struct position {
            node_type *     pPred   ;    ///< Previous node
            node_type *     pCur    ;    ///< Current node

            typename gc::template GuardArray<2> guards  ;   ///< Guards array

            enum {
                guard_prev_item,
                guard_current_item
            };

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

        class auto_lock_position {
            position&   m_pos;
        public:
            auto_lock_position( position& pos )
                : m_pos(pos)
            {
                pos.lock();
            }
            ~auto_lock_position()
            {
                m_pos.unlock();
            }
        };
        //@endcond

    protected:
        //@cond
        void link_node( node_type * pNode, node_type * pPred, node_type * pCur )
        {
            assert( pPred->m_pNext.load(memory_model::memory_order_relaxed).ptr() == pCur );

            pNode->m_pNext.store( marked_node_ptr(pCur), memory_model::memory_order_release );
            pPred->m_pNext.store( marked_node_ptr(pNode), memory_model::memory_order_release );
        }

        void unlink_node( node_type * pPred, node_type * pCur, node_type * pHead )
        {
            assert( pPred->m_pNext.load(memory_model::memory_order_relaxed).ptr() == pCur );

            node_type * pNext = pCur->m_pNext.load(memory_model::memory_order_relaxed).ptr();
            //pCur->m_pNext.store( marked_node_ptr( pNext, 1), memory_model::memory_order_release) ;   // logically deleting
            pCur->m_pNext.store( marked_node_ptr( pHead, 1 ), memory_model::memory_order_release )    ; // logical removal + back-link for search
            pPred->m_pNext.store( marked_node_ptr( pNext ), memory_model::memory_order_release); // physically deleting
            //pCur->m_pNext.store( marked_node_ptr( pHead, 1 ), memory_model::memory_order_release )    ; // back-link for search
        }

        void retire_node( node_type * pNode )
        {
            assert( pNode != nullptr );
            gc::template retire<clean_disposer>( node_traits::to_value_ptr( *pNode ) );
        }
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class LazyList;

        protected:
            value_type * m_pNode;
            typename gc::Guard  m_Guard;

            void next()
            {
                assert( m_pNode != nullptr );

                if ( m_pNode ) {
                    typename gc::Guard g;
                    node_type * pCur = node_traits::to_node_ptr( m_pNode );
                    if ( pCur->m_pNext.load( memory_model::memory_order_relaxed ).ptr() != nullptr ) {      // if pCur is not tail node
                        node_type * pNext;
                        do {
                            pNext = pCur->m_pNext.load(memory_model::memory_order_relaxed).ptr();
                            g.assign( node_traits::to_value_ptr( pNext ));
                        } while ( pNext != pCur->m_pNext.load(memory_model::memory_order_relaxed).ptr() );

                        m_pNode = m_Guard.assign( g.template get<value_type>() );
                    }
                }
            }

            void skip_deleted()
            {
                if ( m_pNode != nullptr ) {
                    typename gc::Guard g;
                    node_type * pNode = node_traits::to_node_ptr( m_pNode );

                    // Dummy tail node could not be marked
                    while ( pNode->is_marked() ) {
                        node_type * p = pNode->m_pNext.load(memory_model::memory_order_relaxed).ptr();
                        g.assign( node_traits::to_value_ptr( p ));
                        if ( p == pNode->m_pNext.load(memory_model::memory_order_relaxed).ptr() )
                            pNode = p;
                    }
                    if ( pNode != node_traits::to_node_ptr( m_pNode ) )
                        m_pNode = m_Guard.assign( g.template get<value_type>() );
                }
            }

            iterator_type( node_type * pNode )
            {
                m_pNode = m_Guard.assign( node_traits::to_value_ptr( pNode ));
                skip_deleted();
            }

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
                : m_pNode( nullptr )
            {}

            iterator_type( iterator_type const& src )
            {
                if ( src.m_pNode ) {
                    m_pNode = m_Guard.assign( src.m_pNode );
                }
                else
                    m_pNode = nullptr;
            }

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

            iterator_type& operator = (iterator_type const& src)
            {
                m_pNode = src.m_pNode;
                m_Guard.assign( m_pNode );
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
        /**
            The forward iterator for lazy list has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (\p gc::HP), a guard is limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if a limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary since it contains GC's guard that is thread-private GC data.
            - Iterator ensures thread-safety even if you delete the item that iterator points to. However, in case of concurrent
              deleting operations it is no guarantee that you iterate all item in the list.

            Therefore, the use of iterators in concurrent environment is not good idea. Use the iterator on the concurrent container
            for debug purpose only.
        */
        typedef iterator_type<false>    iterator;
        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            iterator it( head() );
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
            return iterator( tail() );
        }

        /// Returns a forward const iterator addressing the first element in a list
        //@{
        const_iterator begin() const
        {
            return get_const_begin();
        }
        const_iterator cbegin()
        {
            return get_const_begin();
        }
        //@}

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        //@{
        const_iterator end() const
        {
            return get_const_end();
        }
        const_iterator cend()
        {
            return get_const_end();
        }
        //@}

    private:
        //@cond
        const_iterator get_const_begin() const
        {
            const_iterator it( const_cast<node_type *>( head() ));
            ++it        ;   // skip dummy head
            return it;
        }
        const_iterator get_const_end() const
        {
            return const_iterator( const_cast<node_type *>( tail() ));
        }
        //@endcond

    public:
        /// Default constructor initializes empty list
        LazyList()
        {
            static_assert( (std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type" );

            //m_pTail = cxx_allocator().New();
            head()->m_pNext.store( marked_node_ptr( tail() ), memory_model::memory_order_relaxed );
        }

        /// Destroys the list object
        ~LazyList()
        {
            clear();
            assert( head()->m_pNext.load(memory_model::memory_order_relaxed).ptr() == tail() );
            head()->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
        }

        /// Inserts new node
        /**
            The function inserts \p val in the list if the list does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is linked into the list, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_at( head(), val );
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
            The user-defined functor is called only if the inserting is success and may be passed by reference
            using \p std::ref
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            return insert_at( head(), val, f );
        }

        /// Ensures that the \p item exists in the list
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val not found in the list, then \p val is inserted into the list.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list
            - \p val - argument \p val passed into the \p ensure function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor may change non-key fields of the \p item.
            While the functor \p f is working the item \p item is locked.

            You may pass \p func argument by reference using \p std::ref.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the list.
        */
        template <typename Func>
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return ensure_at( head(), val, func );
        }

        /// Unlinks the item \p val from the list
        /**
            The function searches the item \p val in the list and unlink it from the list
            if it is found and it is equal to \p val.

            Difference between \ref erase and \p unlink functions: \p erase finds <i>a key</i>
            and deletes the item found. \p unlink finds an item by key and deletes it
            only if \p val is an item of that list, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            return unlink_at( head(), val );
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_LazyList_hp_erase_val
            The function searches an item with key equal to \p val in the list,
            unlinks it from the list, and returns \p true.
            If the item with the key equal to \p val is not found the function return \p false.
        */
        template <typename Q>
        bool erase( Q const& val )
        {
            return erase_at( head(), val, key_comparator() );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_hp_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& val, Less pred )
        {
            return erase_at( head(), val, cds::opt::details::make_comparator_from_less<Less>() );
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_LazyList_hp_erase_func
            The function searches an item with key equal to \p val in the list,
            call \p func functor with item found, unlinks it from the list, and returns \p true.
            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode
            The functor may be passed by reference using <tt>boost:ref</tt>

            If the item with the key equal to \p val is not found the function return \p false.
        */
        template <typename Q, typename Func>
        bool erase( const Q& val, Func func )
        {
            return erase_at( head(), val, key_comparator(), func );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_hp_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( const Q& val, Less pred, Func func )
        {
            return erase_at( head(), val, cds::opt::details::make_comparator_from_less<Less>(), func );
        }

        /// Extracts the item from the list with specified \p key
        /** \anchor cds_intrusive_LazyList_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the list, and returns it in \p dest parameter.
            If the item with key equal to \p key is not found the function returns \p false.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            The \ref disposer specified in \p Traits class template parameter is called automatically
            by garbage collector \p GC specified in class' template parameters when returned \ref guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::LazyList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp;
                theList.extract( gp, 5 );
                // Deal with gp
                // ...

                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        template <typename Q>
        bool extract( guarded_ptr& dest, Q const& key )
        {
            return extract_at( head(), dest.guard(), key, key_comparator() );
        }

        /// Extracts the item from the list with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_LazyList_hp_extract "extract(guarded_ptr&, Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool extract_with( guarded_ptr& dest, Q const& key, Less pred )
        {
            return extract_at( head(), dest.guard(), key, cds::opt::details::make_comparator_from_less<Less>() );
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_LazyList_hp_find
            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You may pass \p f argument by reference using \p std::ref.

            The functor may change non-key fields of \p item.
            While the functor \p f is calling the item \p item is locked.

            The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
            may modify both arguments.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f )
        {
            return find_at( head(), val, key_comparator(), f );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_hp_find "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& val, Less pred, Func f )
        {
            return find_at( head(), val, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_LazyList_hp_find_const
            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val );
            };
            \endcode
            where \p item is the item found, \p val is the \p find function argument.

            You may pass \p f argument by reference using \p std::ref.

            The functor may change non-key fields of \p item.
            While the functor \p f is calling the item \p item is locked.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f )
        {
            return find_at( head(), val, key_comparator(), f );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_hp_find_const "find(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& val, Less pred, Func f )
        {
            return find_at( head(), val, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_LazyList_hp_find_val
            The function searches the item with key equal to \p val
            and returns \p true if it is found, and \p false otherwise
        */
        template <typename Q>
        bool find( Q const & val )
        {
            return find_at( head(), val, key_comparator() );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_LazyList_hp_find_val "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool find_with( Q const& val, Less pred )
        {
            return find_at( head(), val, cds::opt::details::make_comparator_from_less<Less>() );
        }

        /// Finds the key \p val and return the item found
        /** \anchor cds_intrusive_LazyList_hp_get
            The function searches the item with key equal to \p val
            and assigns the item found to guarded pointer \p ptr.
            The function returns \p true if \p val is found, and \p false otherwise.
            If \p val is not found the \p ptr parameter is not changed.

            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC automatically when returned \ref guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::LazyList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp;
                if ( theList.get( gp, 5 )) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode

            Note the compare functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        bool get( guarded_ptr& ptr, Q const& val )
        {
            return get_at( head(), ptr.guard(), val, key_comparator() );
        }

        /// Finds the key \p val and return the item found
        /**
            The function is an analog of \ref cds_intrusive_LazyList_hp_get "get( guarded_ptr& ptr, Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool get_with( guarded_ptr& ptr, Q const& val, Less pred )
        {
            return get_at( head(), ptr.guard(), val, cds::opt::details::make_comparator_from_less<Less>() );
        }

        /// Clears the list
        /**
            The function unlink all items from the list.
        */
        void clear()
        {
            typename gc::Guard guard;
            marked_node_ptr h;
            while ( !empty() ) {
                h = head()->m_pNext.load(memory_model::memory_order_relaxed);
                guard.assign( node_traits::to_value_ptr( h.ptr() ));
                if ( head()->m_pNext.load(memory_model::memory_order_acquire) == h ) {
                    head()->m_Lock.lock();
                    h->m_Lock.lock();

                    unlink_node( head(), h.ptr(), head() );

                    h->m_Lock.unlock();
                    head()->m_Lock.unlock();

                    retire_node( h.ptr() ) ; // free node
                }
            }
        }

        /// Checks if the list is empty
        bool empty() const
        {
            return head()->m_pNext.load(memory_model::memory_order_relaxed).ptr() == tail();
        }

        /// Returns list's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the list
            is empty. To check list emptyness use \ref empty() method.
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
            return insert_aux_node( head(), pNode );
        }

        // split-list support
        bool insert_aux_node( node_type * pHead, node_type * pNode )
        {
            assert( pNode != nullptr );

            // Hack: convert node_type to value_type.
            // In principle, auxiliary node can be non-reducible to value_type
            // We assume that comparator can correctly distinguish aux and regular node.
            return insert_at( pHead, *node_traits::to_value_ptr( pNode ) );
        }

        bool insert_at( node_type * pHead, value_type& val )
        {
            link_checker::is_empty( node_traits::to_node_ptr( val ) );
            position pos;
            key_comparator  cmp;

            while ( true ) {
                search( pHead, val, pos, key_comparator() );
                {
                    auto_lock_position alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != tail() && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // failed: key already in list
                            return false;
                        }
                        else {
                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                            ++m_ItemCounter;
                            return true;
                        }
                    }
                }
            }
        }

        template <typename Func>
        bool insert_at( node_type * pHead, value_type& val, Func f )
        {
            link_checker::is_empty( node_traits::to_node_ptr( val ) );
            position pos;
            key_comparator  cmp;

            while ( true ) {
                search( pHead, val, pos, key_comparator() );
                {
                    auto_lock_position alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != tail() && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // failed: key already in list
                            return false;
                        }
                        else {
                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                            f( val );
                            ++m_ItemCounter;
                            return true;
                        }
                    }
                }
            }
        }

        template <typename Func>
        std::pair<bool, bool> ensure_at( node_type * pHead, value_type& val, Func func )
        {
            position pos;
            key_comparator  cmp;

            while ( true ) {
                search( pHead, val, pos, key_comparator() );
                {
                    auto_lock_position alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != tail() && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                            // key already in the list

                            func( false, *node_traits::to_value_ptr( *pos.pCur ) , val );
                            return std::make_pair( true, false );
                        }
                        else {
                            // new key
                            link_checker::is_empty( node_traits::to_node_ptr( val ) );

                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                            func( true, val, val );
                            ++m_ItemCounter;
                            return std::make_pair( true, true );
                        }
                    }
                }
            }
        }

        bool unlink_at( node_type * pHead, value_type& val )
        {
            position pos;
            key_comparator  cmp;

            while ( true ) {
                search( pHead, val, pos, key_comparator() );
                {
                    int nResult = 0;
                    {
                        auto_lock_position alp( pos );
                        if ( validate( pos.pPred, pos.pCur ) ) {
                            if ( pos.pCur != tail()
                                && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0
                                && node_traits::to_value_ptr( pos.pCur ) == &val )
                            {
                                // item found
                                unlink_node( pos.pPred, pos.pCur, pHead );
                                --m_ItemCounter;
                                nResult = 1;
                            }
                            else
                                nResult = -1;
                        }
                    }
                    if ( nResult ) {
                        if ( nResult > 0 ) {
                            retire_node( pos.pCur );
                            return true;
                        }
                        return false;
                    }
                }
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type * pHead, const Q& val, Compare cmp, Func f, position& pos )
        {
            while ( true ) {
                search( pHead, val, pos, cmp );
                {
                    int nResult = 0;
                    {
                        auto_lock_position alp( pos );
                        if ( validate( pos.pPred, pos.pCur )) {
                            if ( pos.pCur != tail() && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 ) {
                                // key found
                                unlink_node( pos.pPred, pos.pCur, pHead );
                                f( *node_traits::to_value_ptr( *pos.pCur ) );
                                --m_ItemCounter;
                                nResult = 1;
                            }
                            else {
                                nResult = -1;
                            }
                        }
                    }
                    if ( nResult ) {
                        if ( nResult > 0 ) {
                            retire_node( pos.pCur );
                            return true;
                        }
                        return false;
                    }
                }
            }
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type * pHead, const Q& val, Compare cmp, Func f )
        {
            position pos;
            return erase_at( pHead, val, cmp, f, pos );
        }

        template <typename Q, typename Compare>
        bool erase_at( node_type * pHead, const Q& val, Compare cmp )
        {
            position pos;
            return erase_at( pHead, val, cmp, [](value_type const &){}, pos );
        }

        template <typename Q, typename Compare>
        bool extract_at( node_type * pHead, typename gc::Guard& gp, const Q& val, Compare cmp )
        {
            position pos;
            if ( erase_at( pHead, val, cmp, [](value_type const &){}, pos )) {
                gp.assign( pos.guards.template get<value_type>(position::guard_current_item) );
                return true;
            }
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( node_type * pHead, Q& val, Compare cmp, Func f )
        {
            position pos;

            search( pHead, val, pos, cmp );
            if ( pos.pCur != tail() ) {
                std::unique_lock< typename node_type::lock_type> al( pos.pCur->m_Lock );
                if ( !pos.pCur->is_marked()
                    && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 )
                {
                    f( *node_traits::to_value_ptr( *pos.pCur ), val );
                    return true;
                }
            }
            return false;
        }

        template <typename Q, typename Compare>
        bool find_at( node_type * pHead, Q const& val, Compare cmp )
        {
            position pos;

            search( pHead, val, pos, cmp );
            return pos.pCur != tail()
                && !pos.pCur->is_marked()
                && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0;
        }

        template <typename Q, typename Compare>
        bool get_at( node_type * pHead, typename gc::Guard& gp, Q const& val, Compare cmp )
        {
            position pos;

            search( pHead, val, pos, cmp );
            if ( pos.pCur != tail()
                && !pos.pCur->is_marked()
                && cmp( *node_traits::to_value_ptr( *pos.pCur ), val ) == 0 )
            {
                gp.assign( pos.guards.template get<value_type>( position::guard_current_item ));
                return true;
            }
            return false;
        }

        //@endcond

    protected:
        //@cond
        template <typename Q, typename Compare>
        void search( node_type * pHead, const Q& key, position& pos, Compare cmp )
        {
            const node_type * pTail = tail();

            marked_node_ptr pCur( pHead );
            marked_node_ptr pPrev( pHead );

            back_off        bkoff;

            while ( pCur.ptr() != pTail )
            {
                if ( pCur.ptr() != pHead ) {
                    if ( cmp( *node_traits::to_value_ptr( *pCur.ptr() ), key ) >= 0 )
                        break;
                }

                pos.guards.copy( position::guard_prev_item, position::guard_current_item );
                pPrev = pCur;

                for (;;) {
                    pCur = pPrev->m_pNext.load(memory_model::memory_order_relaxed);
                    pos.guards.assign( position::guard_current_item, node_traits::to_value_ptr( pCur.ptr() ));
                    if ( pCur == pPrev->m_pNext.load(memory_model::memory_order_acquire) )
                        break;
                    bkoff();
                }
                assert( pCur.ptr() != nullptr );
            }

            pos.pCur = pCur.ptr();
            pos.pPred = pPrev.ptr();
        }

        static bool validate( node_type * pPred, node_type * pCur )
        {
            return !pPred->is_marked()
                && !pCur->is_marked()
                && pPred->m_pNext.load(memory_model::memory_order_relaxed) == pCur;
        }

        //@endcond
    };
}}  // namespace cds::intrusive

#endif // __CDS_INTRUSIVE_IMPL_LAZY_LIST_H
