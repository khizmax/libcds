// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_IMPL_MICHAEL_LIST_H
#define CDSLIB_INTRUSIVE_IMPL_MICHAEL_LIST_H

#include <cds/intrusive/details/michael_list_base.h>
#include <cds/details/make_const_type.h>

namespace cds { namespace intrusive {

    /// Michael's lock-free ordered single-linked list
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_MichaelList_hp

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>.

        Source:
            - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        Template arguments:
        - \p GC - Garbage collector used. Note the \p GC must be the same as the GC used for item type \p T (see \p michael_list::node).
        - \p T - type to be stored in the list. The type must be based on \p michael_list::node (for \p michael_list::base_hook)
            or it must have a member of type \p michael_list::node (for \p michael_list::member_hook).
        - \p Traits - type traits, default is \p michael_list::traits. It is possible to declare option-based
             list with \p cds::intrusive::michael_list::make_traits metafunction:
            For example, the following traits-based declaration of \p gc::HP Michael's list
            \code
            #include <cds/intrusive/michael_list_hp.h>
            // Declare item stored in your list
            struct item: public cds::intrusive::michael_list::node< cds::gc::HP >
            {
                int nKey;
                // .... other data
            };

            // Declare comparator for the item
            struct my_compare {
                int operator()( item const& i1, item const& i2 ) const
                {
                    return i1.nKey - i2.nKey;
                }
            };

            // Declare traits
            struct my_traits: public cds::intrusive::michael_list::traits
            {
                typedef cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::HP > >   hook;
                typedef my_compare compare;
            };

            // Declare traits-based list
            typedef cds::intrusive::MichaelList< cds::gc::HP, item, my_traits >     traits_based_list;
            \endcode
            is equivalent for the following option-based list
            \code
            #include <cds/intrusive/michael_list_hp.h>

            // item struct and my_compare are the same

            // Declare option-based list
            typedef cds::intrusive::MichaelList< cds::gc::HP, item,
                typename cds::intrusive::michael_list::make_traits<
                    cds::intrusive::opt::hook< cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::HP > > >    // hook option
                    ,cds::intrusive::opt::compare< my_compare >     // item comparator option
                >::type
            >     option_based_list;
            \endcode

        \par Usage
        There are different specializations of this template for each garbage collecting schema.
        You should select GC needed and include appropriate .h-file:
        - for \p gc::HP: <tt> <cds/intrusive/michael_list_hp.h> </tt>
        - for \p gc::DHP: <tt> <cds/intrusive/michael_list_dhp.h> </tt>
        - for \ref cds_urcu_gc "RCU type" - see \ref cds_intrusive_MichaelList_rcu "RCU-based MichaelList"
        - for \p gc::nogc: <tt> <cds/intrusive/michael_list_nogc.h> </tt>
            See \ref cds_intrusive_MichaelList_nogc "non-GC MichaelList"

        Then, you should incorporate \p michael_list::node into your struct \p T and provide
        appropriate \p michael_list::traits::hook in your \p Traits template parameters. Usually, for \p Traits you
        define a struct based on \p michael_list::traits.

        Example for \p gc::DHP and base hook:
        \code
        // Include GC-related Michael's list specialization
        #include <cds/intrusive/michael_list_dhp.h>

        // Data stored in Michael's list
        struct my_data: public cds::intrusive::michael_list::node< cds::gc::DHP >
        {
            // key field
            std::string     strKey;

            // other data
            // ...
        };

        // my_data comparing functor
        struct my_data_cmp {
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


        // Declare traits
        struct my_traits: public cds::intrusive::michael_list::traits
        {
            typedef cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::DHP > >   hook;
            typedef my_data_cmp compare;
        };

        // Declare list type
        typedef cds::intrusive::MichaelList< cds::gc::DHP, my_data, my_traits >     traits_based_list;
        \endcode

        Equivalent option-based code:
        \code
        // GC-related specialization
        #include <cds/intrusive/michael_list_dhp.h>

        struct my_data {
            // see above
        };
        struct compare {
            // see above
        };

        // Declare option-based list
        typedef cds::intrusive::MichaelList< cds::gc::DHP
            ,my_data
            , typename cds::intrusive::michael_list::make_traits<
                cds::intrusive::opt::hook< cds::intrusive::michael_list::base_hook< cds::opt::gc< cds::gc::DHP > > >
                ,cds::intrusive::opt::compare< my_data_cmp >
            >::type
        > option_based_list;

        \endcode
    */
    template <
        class GC
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = michael_list::traits
#else
        ,class Traits
#endif
    >
    class MichaelList
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

        typedef typename traits::disposer  disposer; ///< disposer used
        typedef typename traits::stat      stat;     ///< Internal statistics
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename michael_list::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

        typedef GC  gc          ;   ///< Garbage collector
        typedef typename traits::back_off  back_off;   ///< back-off strategy
        typedef typename traits::item_counter item_counter;   ///< Item counting policy used
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_model option

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        static constexpr const size_t c_nHazardPtrCount = 4; ///< Count of hazard pointer required for the algorithm

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
        typedef typename node_type::atomic_marked_ptr   atomic_node_ptr;   ///< Atomic node pointer
        typedef typename node_type::marked_ptr          marked_node_ptr;   ///< Node marked pointer

        typedef atomic_node_ptr     auxiliary_head;   ///< Auxiliary head type (for split-list support)

        atomic_node_ptr m_pHead;        ///< Head pointer
        item_counter    m_ItemCounter;  ///< Item counter
        stat            m_Stat;         ///< Internal statistics

        //@cond
        /// Position pointer for item search
        struct position {
            atomic_node_ptr * pPrev ;   ///< Previous node
            node_type * pCur        ;   ///< Current node
            node_type * pNext       ;   ///< Next node

            typename gc::template GuardArray<3> guards  ;   ///< Guards array

            enum {
                guard_prev_item,
                guard_current_item,
                guard_next_item
            };
        };

        struct clean_disposer {
            void operator()( value_type * p )
            {
                michael_list::node_cleaner<gc, node_type, memory_model>()( node_traits::to_node_ptr( p ));
                disposer()( p );
            }
        };
        //@endcond

    protected:
        //@cond
        static void retire_node( node_type * pNode )
        {
            assert( pNode != nullptr );
            gc::template retire<clean_disposer>( node_traits::to_value_ptr( *pNode ));
        }

        static bool link_node( node_type * pNode, position& pos )
        {
            assert( pNode != nullptr );
            link_checker::is_empty( pNode );

            marked_node_ptr cur(pos.pCur);
            pNode->m_pNext.store( cur, memory_model::memory_order_release );
            if ( cds_likely( pos.pPrev->compare_exchange_strong( cur, marked_node_ptr(pNode), memory_model::memory_order_release, atomics::memory_order_relaxed )))
                return true;

            pNode->m_pNext.store( marked_node_ptr(), memory_model::memory_order_relaxed );
            return false;
        }

        static bool unlink_node( position& pos )
        {
            assert( pos.pPrev != nullptr );
            assert( pos.pCur != nullptr );

            // Mark the node (logical deleting)
            marked_node_ptr next(pos.pNext, 0);
            if ( cds_likely( pos.pCur->m_pNext.compare_exchange_strong( next, marked_node_ptr(pos.pNext, 1), memory_model::memory_order_release, atomics::memory_order_relaxed ))) {
                // physical deletion may be performed by search function if it detects that a node is logically deleted (marked)
                // CAS may be successful here or in other thread that searching something
                marked_node_ptr cur(pos.pCur);
                if ( cds_likely( pos.pPrev->compare_exchange_strong( cur, marked_node_ptr( pos.pNext ), memory_model::memory_order_acquire, atomics::memory_order_relaxed )))
                    retire_node( pos.pCur );
                return true;
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

        protected:
            value_type * m_pNode;
            typename gc::Guard  m_Guard;

            void next()
            {
                if ( m_pNode ) {
                    typename gc::Guard g;
                    node_type * pCur = node_traits::to_node_ptr( *m_pNode );

                    marked_node_ptr pNext;
                    do {
                        pNext = pCur->m_pNext.load(memory_model::memory_order_relaxed);
                        g.assign( node_traits::to_value_ptr( pNext.ptr()));
                    } while ( cds_unlikely( pNext != pCur->m_pNext.load(memory_model::memory_order_acquire)));

                    if ( pNext.ptr())
                        m_pNode = m_Guard.assign( g.template get<value_type>());
                    else {
                        m_pNode = nullptr;
                        m_Guard.clear();
                    }
                }
            }

            iterator_type( atomic_node_ptr const& pNode )
            {
                for (;;) {
                    marked_node_ptr p = pNode.load(memory_model::memory_order_relaxed);
                    if ( p.ptr()) {
                        m_pNode = m_Guard.assign( node_traits::to_value_ptr( p.ptr()));
                    }
                    else {
                        m_pNode = nullptr;
                        m_Guard.clear();
                    }
                    if ( cds_likely( p == pNode.load(memory_model::memory_order_acquire)))
                        break;
                }
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
                return *this;
            }

            iterator_type& operator = (iterator_type const& src)
            {
                m_pNode = src.m_pNode;
                m_Guard.assign( m_pNode );
                return *this;
            }

            /*
            /// Post-increment
            void operator ++(int)
            {
                next();
            }
            */

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
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's list has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (like as \p gc::HP), a guard is a limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if the limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary since it contains thread-private GC's guard.
            - Iterator ensures thread-safety even if you delete the item the iterator points to. However, in case of concurrent
              deleting operations there is no guarantee that you iterate all item in the list.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.

            @warning Use this iterator on the concurrent container for debugging purpose only.

            The iterator interface:
            \code
            class iterator {
            public:
                // Default constructor
                iterator();

                // Copy construtor
                iterator( iterator const& src );

                // Dereference operator
                value_type * operator ->() const;

                // Dereference operator
                value_type& operator *() const;

                // Preincrement operator
                iterator& operator ++();

                // Assignment operator
                iterator& operator = (iterator const& src);

                // Equality operators
                bool operator ==(iterator const& i ) const;
                bool operator !=(iterator const& i ) const;
            };
            \endcode
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
            return iterator( m_pHead );
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            Internally, <tt>end</tt> returning value equals to \p nullptr.

            The returned value can be used only to control reaching the end of the list.
            For empty list <tt>begin() == end()</tt>
        */
        iterator end()
        {
            return iterator();
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return const_iterator( m_pHead );
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return const_iterator( m_pHead );
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

        /// Destroys the list object
        ~MichaelList()
        {
            clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val into the list if the list does not contain
            an item with key equal to \p val.

            Returns \p true if \p val has been linked to the list, \p false otherwise.
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
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this list's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            return insert_at( m_pHead, val, f );
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the list, then \p val is inserted
            iff \p bInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list
            - \p val - argument \p val passed into the \p update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refers to the same thing.

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with that key
            already in the list.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bInsert = true )
        {
            return update_at( m_pHead, val, func, bInsert );
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
            The function searches the item \p val in the list and unlinks it from the list
            if it is found and it is equal to \p val.

            Difference between \p erase() and \p %unlink(): \p %erase() finds <i>a key</i>
            and deletes the item found. \p %unlink() finds an item by key and deletes it
            only if \p val is an item of the list, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            \p disposer specified in \p Traits is called for deleted item.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            return unlink_at( m_pHead, val );
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_MichaelList_hp_erase_val
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \p true.
            If \p key is not found the function return \p false.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( m_pHead, key, key_comparator());
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_hp_erase_val "erase(Q const&)"
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
        /** \anchor cds_intrusive_MichaelList_hp_erase_func
            The function searches an item with key equal to \p key in the list,
            call \p func functor with item found, unlinks it from the list, and returns \p true.
            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode
            If \p key is not found the function return \p false, \p func is not called.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func func )
        {
            return erase_at( m_pHead, key, key_comparator(), func );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_hp_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        /// Extracts the item from the list with specified \p key
        /** \anchor cds_intrusive_MichaelList_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the list, and returns it as \p guarded_ptr.
            If \p key is not found returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            The \ref disposer specified in \p Traits class template parameter is called automatically
            by garbage collector \p GC when returned \ref guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::MichaelList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp(theList.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return extract_at( m_pHead, key, key_comparator());
        }

        /// Extracts the item using compare functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_hp_extract "extract(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return extract_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Finds \p key in the list
        /** \anchor cds_intrusive_MichaelList_hp_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item. Note that the function is only guarantee
            that \p item cannot be disposed during functor is executing.
            The function does not serialize simultaneous access to the \p item. If such access is
            possible you must provide your own synchronization schema to keep out unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            may modify both arguments.

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

        /// Finds the \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_hp_find_func "find(Q&, Func)"
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

        /// Checks whether the list contains \p key using \p pred predicate for searching
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

        /// Finds the \p key and return the item found
        /** \anchor cds_intrusive_MichaelList_hp_get
            The function searches the item with key equal to \p key
            and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC automatically when returned \ref guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::MichaelList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp(theList.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode

            Note the compare functor specified for \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        guarded_ptr get( Q const& key )
        {
            return get_at( m_pHead, key, key_comparator());
        }

        /// Finds the \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_hp_get "get( Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return get_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Clears the list
        /**
            The function unlink all items from the list.
        */
        void clear()
        {
            typename gc::Guard guard;
            marked_node_ptr head;
            while ( true ) {
                head = m_pHead.load(memory_model::memory_order_relaxed);
                if ( head.ptr())
                    guard.assign( node_traits::to_value_ptr( *head.ptr()));
                if ( cds_likely( m_pHead.load(memory_model::memory_order_acquire) == head )) {
                    if ( head.ptr() == nullptr )
                        break;
                    value_type& val = *node_traits::to_value_ptr( *head.ptr());
                    unlink( val );
                }
            }
        }

        /// Checks whether the list is empty
        bool empty() const
        {
            return m_pHead.load( memory_model::memory_order_relaxed ).all() == nullptr;
        }

        /// Returns list's item count
        /**
            The value returned depends on item counter provided by \p Traits. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact does not mean that the list
            is empty. To check list emptiness use \p empty() method.
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
            // We assume that comparator can correctly distinguish aux and regular node.
            return insert_at( refHead, *node_traits::to_value_ptr( pNode ));
        }

        bool insert_at( atomic_node_ptr& refHead, value_type& val )
        {
            node_type * pNode = node_traits::to_node_ptr( val );
            position pos;

            while ( true ) {
                if ( search( refHead, val, pos, key_comparator())) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_node( pNode, pos )) {
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        template <typename Func>
        bool insert_at( atomic_node_ptr& refHead, value_type& val, Func f )
        {
            node_type * pNode = node_traits::to_node_ptr( val );
            position pos;

            while ( true ) {
                if ( search( refHead, val, pos, key_comparator())) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                typename gc::Guard guard;
                guard.assign( &val );
                if ( link_node( pNode, pos )) {
                    f( val );
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        template <typename Func>
        std::pair<bool, bool> update_at( atomic_node_ptr& refHead, value_type& val, Func func, bool bInsert )
        {
            position pos;

            node_type * pNode = node_traits::to_node_ptr( val );
            while ( true ) {
                if ( search( refHead, val, pos, key_comparator())) {
                    if ( cds_unlikely( pos.pCur->m_pNext.load(memory_model::memory_order_acquire).bits())) {
                        back_off()();
                        m_Stat.onUpdateMarked();
                        continue;       // the node found is marked as deleted
                    }
                    assert( key_comparator()( val, *node_traits::to_value_ptr( *pos.pCur )) == 0 );

                    func( false, *node_traits::to_value_ptr( *pos.pCur ) , val );
                    m_Stat.onUpdateExisting();
                    return std::make_pair( true, false );
                }
                else {
                    if ( !bInsert ) {
                        m_Stat.onUpdateFailed();
                        return std::make_pair( false, false );
                    }

                    typename gc::Guard guard;
                    guard.assign( &val );
                    if ( link_node( pNode, pos )) {
                        ++m_ItemCounter;
                        func( true, val, val );
                        m_Stat.onUpdateNew();
                        return std::make_pair( true, true );
                    }
                }

                m_Stat.onUpdateRetry();
            }
        }

        bool unlink_at( atomic_node_ptr& refHead, value_type& val )
        {
            position pos;

            back_off bkoff;
            while ( search( refHead, val, pos, key_comparator())) {
                if ( node_traits::to_value_ptr( *pos.pCur ) == &val ) {
                    if ( unlink_node( pos )) {
                        --m_ItemCounter;
                        m_Stat.onEraseSuccess();
                        return true;
                    }
                    else
                        bkoff();
                }
                else {
                    m_Stat.onUpdateFailed();
                    break;
                }

                m_Stat.onEraseRetry();
            }

            m_Stat.onEraseFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( atomic_node_ptr& refHead, const Q& val, Compare cmp, Func f, position& pos )
        {
            back_off bkoff;
            while ( search( refHead, val, pos, cmp )) {
                if ( unlink_node( pos )) {
                    f( *node_traits::to_value_ptr( *pos.pCur ));
                    --m_ItemCounter;
                    m_Stat.onEraseSuccess();
                    return true;
                }
                else
                    bkoff();

                m_Stat.onEraseRetry();
            }

            m_Stat.onEraseFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( atomic_node_ptr& refHead, const Q& val, Compare cmp, Func f )
        {
            position pos;
            return erase_at( refHead, val, cmp, f, pos );
        }

        template <typename Q, typename Compare>
        bool erase_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos;
            return erase_at( refHead, val, cmp, [](value_type const&){}, pos );
        }

        template <typename Q, typename Compare>
        guarded_ptr extract_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos;
            back_off bkoff;
            while ( search( refHead, val, pos, cmp )) {
                if ( unlink_node( pos )) {
                    --m_ItemCounter;
                    m_Stat.onEraseSuccess();
                    return guarded_ptr( pos.guards.release( position::guard_current_item ));
                }
                else
                    bkoff();
                m_Stat.onEraseRetry();
            }

            m_Stat.onEraseFailed();
            return guarded_ptr();
        }

        template <typename Q, typename Compare>
        bool find_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos;
            if ( search( refHead, val, pos, cmp )) {
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( atomic_node_ptr& refHead, Q& val, Compare cmp, Func f )
        {
            position pos;
            if ( search( refHead, val, pos, cmp )) {
                f( *node_traits::to_value_ptr( *pos.pCur ), val );
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare>
        guarded_ptr get_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos;
            if ( search( refHead, val, pos, cmp )) {
                m_Stat.onFindSuccess();
                return guarded_ptr( pos.guards.release( position::guard_current_item ));
            }

            m_Stat.onFindFailed();
            return guarded_ptr();
        }

        // split-list support
        template <typename Predicate>
        void destroy( Predicate /*pred*/ )
        {
            clear();
        }

        //@endcond

    protected:

        //@cond
        template <typename Q, typename Compare >
        bool search( atomic_node_ptr& refHead, const Q& val, position& pos, Compare cmp )
        {
            atomic_node_ptr * pPrev;
            marked_node_ptr pNext;
            marked_node_ptr pCur;

            back_off        bkoff;

        try_again:
            pPrev = &refHead;
            pNext = nullptr;

            pCur = pos.guards.protect( position::guard_current_item, *pPrev,
                   [](marked_node_ptr p) -> value_type *
                    {
                        return node_traits::to_value_ptr( p.ptr());
                    });

            while ( true ) {
                if ( pCur.ptr() == nullptr ) {
                    pos.pPrev = pPrev;
                    pos.pCur = nullptr;
                    pos.pNext = nullptr;
                    return false;
                }

                pNext = pos.guards.protect( position::guard_next_item, pCur->m_pNext,
                        [](marked_node_ptr p ) -> value_type *
                        {
                            return node_traits::to_value_ptr( p.ptr());
                        });
                if ( cds_unlikely( pPrev->load(memory_model::memory_order_acquire).all() != pCur.ptr())) {
                    bkoff();
                    goto try_again;
                }

                // pNext contains deletion mark for pCur
                if ( pNext.bits() == 1 ) {
                    // pCur marked i.e. logically deleted. Help the erase/unlink function to unlink pCur node
                    marked_node_ptr cur( pCur.ptr());
                    if ( cds_unlikely( pPrev->compare_exchange_strong( cur, marked_node_ptr( pNext.ptr()), memory_model::memory_order_acquire, atomics::memory_order_relaxed ))) {
                        retire_node( pCur.ptr());
                        m_Stat.onHelpingSuccess();
                    }
                    else {
                        bkoff();
                        m_Stat.onHelpingFailed();
                        goto try_again;
                    }
                }
                else {
                    assert( pCur.ptr() != nullptr );
                    int nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                    if ( nCmp >= 0 ) {
                        pos.pPrev = pPrev;
                        pos.pCur = pCur.ptr();
                        pos.pNext = pNext.ptr();
                        return nCmp == 0;
                    }
                    pPrev = &( pCur->m_pNext );
                    pos.guards.copy( position::guard_prev_item, position::guard_current_item );
                }
                pCur = pNext;
                pos.guards.copy( position::guard_current_item, position::guard_next_item );
            }
        }
        //@endcond
    };
}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_MICHAEL_LIST_H
