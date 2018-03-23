// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_IMPL_SKIP_LIST_H
#define CDSLIB_INTRUSIVE_IMPL_SKIP_LIST_H

#include <type_traits>
#include <memory>
#include <functional>   // ref
#include <cds/intrusive/details/skip_list_base.h>
#include <cds/opt/compare.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace intrusive {

    //@cond
    namespace skip_list { namespace details {

        template <class GC, typename NodeTraits, typename BackOff, bool IsConst>
        class iterator {
        public:
            typedef GC                                  gc;
            typedef NodeTraits                          node_traits;
            typedef BackOff                             back_off;
            typedef typename node_traits::node_type     node_type;
            typedef typename node_traits::value_type    value_type;
            static constexpr bool const c_isConst = IsConst;

            typedef typename std::conditional< c_isConst, value_type const&, value_type&>::type   value_ref;

        protected:
            typedef typename node_type::marked_ptr          marked_ptr;
            typedef typename node_type::atomic_marked_ptr   atomic_marked_ptr;

            typename gc::Guard      m_guard;
            node_type *             m_pNode;

        protected:
            static value_type * gc_protect( marked_ptr p )
            {
                return node_traits::to_value_ptr( p.ptr());
            }

            void next()
            {
                typename gc::Guard g;
                g.copy( m_guard );
                back_off bkoff;

                for (;;) {
                    if ( m_pNode->next( m_pNode->height() - 1 ).load( atomics::memory_order_acquire ).bits()) {
                        // Current node is marked as deleted. So, its next pointer can point to anything
                        // In this case we interrupt our iteration and returns end() iterator.
                        *this = iterator();
                        return;
                    }

                    marked_ptr p = m_guard.protect( (*m_pNode)[0], gc_protect );
                    node_type * pp = p.ptr();
                    if ( p.bits()) {
                        // p is marked as deleted. Spin waiting for physical removal
                        bkoff();
                        continue;
                    }
                    else if ( pp && pp->next( pp->height() - 1 ).load( atomics::memory_order_relaxed ).bits()) {
                        // p is marked as deleted. Spin waiting for physical removal
                        bkoff();
                        continue;
                    }

                    m_pNode = pp;
                    break;
                }
            }

        public: // for internal use only!!!
            iterator( node_type& refHead )
                : m_pNode( nullptr )
            {
                back_off bkoff;

                for (;;) {
                    marked_ptr p = m_guard.protect( refHead[0], gc_protect );
                    if ( !p.ptr()) {
                        // empty skip-list
                        m_guard.clear();
                        break;
                    }

                    node_type * pp = p.ptr();
                    // Logically deleted node is marked from highest level
                    if ( !pp->next( pp->height() - 1 ).load( atomics::memory_order_acquire ).bits()) {
                        m_pNode = pp;
                        break;
                    }

                    bkoff();
                }
            }

        public:
            iterator()
                : m_pNode( nullptr )
            {}

            iterator( iterator const& s)
                : m_pNode( s.m_pNode )
            {
                m_guard.assign( node_traits::to_value_ptr(m_pNode));
            }

            value_type * operator ->() const
            {
                assert( m_pNode != nullptr );
                assert( node_traits::to_value_ptr( m_pNode ) != nullptr );

                return node_traits::to_value_ptr( m_pNode );
            }

            value_ref operator *() const
            {
                assert( m_pNode != nullptr );
                assert( node_traits::to_value_ptr( m_pNode ) != nullptr );

                return *node_traits::to_value_ptr( m_pNode );
            }

            /// Pre-increment
            iterator& operator ++()
            {
                next();
                return *this;
            }

            iterator& operator =(const iterator& src)
            {
                m_pNode = src.m_pNode;
                m_guard.copy( src.m_guard );
                return *this;
            }

            template <typename Bkoff, bool C>
            bool operator ==(iterator<gc, node_traits, Bkoff, C> const& i ) const
            {
                return m_pNode == i.m_pNode;
            }
            template <typename Bkoff, bool C>
            bool operator !=(iterator<gc, node_traits, Bkoff, C> const& i ) const
            {
                return !( *this == i );
            }
        };
    }}  // namespace skip_list::details
    //@endcond

    /// Lock-free skip-list set
    /** @ingroup cds_intrusive_map
        @anchor cds_intrusive_SkipListSet_hp

        The implementation of well-known probabilistic data structure called skip-list
        invented by W.Pugh in his papers:
            - [1989] W.Pugh Skip Lists: A Probabilistic Alternative to Balanced Trees
            - [1990] W.Pugh A Skip List Cookbook

        A skip-list is a probabilistic data structure that provides expected logarithmic
        time search without the need of rebalance. The skip-list is a collection of sorted
        linked list. Nodes are ordered by key. Each node is linked into a subset of the lists.
        Each list has a level, ranging from 0 to 32. The bottom-level list contains
        all the nodes, and each higher-level list is a sublist of the lower-level lists.
        Each node is created with a random top level (with a random height), and belongs
        to all lists up to that level. The probability that a node has the height 1 is 1/2.
        The probability that a node has the height N is 1/2 ** N (more precisely,
        the distribution depends on an random generator provided, but our generators
        have this property).

        The lock-free variant of skip-list is implemented according to book
            - [2008] M.Herlihy, N.Shavit "The Art of Multiprocessor Programming",
                chapter 14.4 "A Lock-Free Concurrent Skiplist".

        <b>Template arguments</b>:
            - \p GC - Garbage collector used. Note the \p GC must be the same as the GC used for item type \p T, see \p skip_list::node.
            - \p T - type to be stored in the list. The type must be based on \p skip_list::node (for \p skip_list::base_hook)
                or it must have a member of type \p skip_list::node (for \p skip_list::member_hook).
            - \p Traits - skip-list traits, default is \p skip_list::traits.
                It is possible to declare option-based list with \p cds::intrusive::skip_list::make_traits metafunction istead of \p Traits
                template argument.

        @warning The skip-list requires up to 67 hazard pointers that may be critical for some GCs for which
            the guard count is limited (like as \p gc::HP). Those GCs should be explicitly initialized with
            hazard pointer enough: \code cds::gc::HP myhp( 67 ) \endcode. Otherwise an run-time exception may be raised
            when you try to create skip-list object.

        There are several specializations of \p %SkipListSet for each \p GC. You should include:
        - <tt><cds/intrusive/skip_list_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/intrusive/skip_list_dhp.h></tt> for \p gc::DHP garbage collector
        - <tt><cds/intrusive/skip_list_nogc.h></tt> for \ref cds_intrusive_SkipListSet_nogc for append-only set
        - <tt><cds/intrusive/skip_list_rcu.h></tt> for \ref cds_intrusive_SkipListSet_rcu "RCU type"

        <b>Iterators</b>

        The class supports a forward iterator (\ref iterator and \ref const_iterator).
        The iteration is ordered.
        The iterator object is thread-safe: the element pointed by the iterator object is guarded,
        so, the element cannot be reclaimed while the iterator object is alive.
        However, passing an iterator object between threads is dangerous.

        @warning Due to concurrent nature of skip-list set it is not guarantee that you can iterate
        all elements in the set: any concurrent deletion can exclude the element
        pointed by the iterator from the set, and your iteration can be terminated
        before end of the set. Therefore, such iteration is more suitable for debugging purpose only

        Remember, each iterator object requires 2 additional hazard pointers, that may be
        a limited resource for \p GC like as \p gc::HP (for \p gc::DHP the count of
        guards is unlimited).

        The iterator class supports the following minimalistic interface:
        \code
        struct iterator {
            // Default ctor
            iterator();

            // Copy ctor
            iterator( iterator const& s);

            value_type * operator ->() const;
            value_type& operator *() const;

            // Pre-increment
            iterator& operator ++();

            // Copy assignment
            iterator& operator = (const iterator& src);

            bool operator ==(iterator const& i ) const;
            bool operator !=(iterator const& i ) const;
        };
        \endcode
        Note, the iterator object returned by \p end(), \p cend() member functions points to \p nullptr and should not be dereferenced.

        <b>How to use</b>

        You should incorporate \p skip_list::node into your struct \p T and provide
        appropriate \p skip_list::traits::hook in your \p Traits template parameters. Usually, for \p Traits you
        define a struct based on \p skip_list::traits.

        Example for \p gc::HP and base hook:
        \code
        // Include GC-related skip-list specialization
        #include <cds/intrusive/skip_list_hp.h>

        // Data stored in skip list
        struct my_data: public cds::intrusive::skip_list::node< cds::gc::HP >
        {
            // key field
            std::string     strKey;

            // other data
            // ...
        };

        // my_data compare functor
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


        // Declare your traits
        struct my_traits: public cds::intrusive::skip_list::traits
        {
            typedef cds::intrusive::skip_list::base_hook< cds::opt::gc< cds::gc::HP > >   hook;
            typedef my_data_cmp compare;
        };

        // Declare skip-list set type
        typedef cds::intrusive::SkipListSet< cds::gc::HP, my_data, my_traits >     traits_based_set;
        \endcode

        Equivalent option-based code:
        \code
        // GC-related specialization
        #include <cds/intrusive/skip_list_hp.h>

        struct my_data {
            // see above
        };
        struct compare {
            // see above
        };

        // Declare option-based skip-list set
        typedef cds::intrusive::SkipListSet< cds::gc::HP
            ,my_data
            , typename cds::intrusive::skip_list::make_traits<
                cds::intrusive::opt::hook< cds::intrusive::skip_list::base_hook< cds::opt::gc< cds::gc::HP > > >
                ,cds::intrusive::opt::compare< my_data_cmp >
            >::type
        > option_based_set;

        \endcode
    */
    template <
        class GC
       ,typename T
#ifdef CDS_DOXYGEN_INVOKED
       ,typename Traits = skip_list::traits
#else
       ,typename Traits
#endif
    >
    class SkipListSet
    {
    public:
        typedef GC      gc;         ///< Garbage collector
        typedef T       value_type; ///< type of value stored in the skip-list
        typedef Traits  traits;     ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key comparison functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif

        typedef typename traits::disposer  disposer;   ///< item disposer
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits; ///< node traits

        typedef typename traits::item_counter  item_counter;   ///< Item counting policy
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering, see \p cds::opt::memory_model option
        typedef typename traits::random_level_generator random_level_generator; ///< random level generator
        typedef typename traits::allocator     allocator_type;   ///< allocator for maintaining array of next pointers of the node
        typedef typename traits::back_off      back_off;   ///< Back-off strategy
        typedef typename traits::stat          stat;       ///< internal statistics type

    public:
        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        /// Max node height. The actual node height should be in range <tt>[0 .. c_nMaxHeight)</tt>
        /**
            The max height is specified by \ref skip_list::random_level_generator "random level generator" constant \p m_nUpperBound
            but it should be no more than 32 (\p skip_list::c_nHeightLimit).
        */
        static unsigned int const c_nMaxHeight = std::conditional<
            (random_level_generator::c_nUpperBound <= skip_list::c_nHeightLimit),
            std::integral_constant< unsigned int, random_level_generator::c_nUpperBound >,
            std::integral_constant< unsigned int, skip_list::c_nHeightLimit >
        >::type::value;

        //@cond
        static unsigned int const c_nMinHeight = 5;
        //@endcond

        // c_nMaxHeight * 2 - pPred/pSucc guards
        // + 1 - for erase, unlink
        // + 1 - for clear
        // + 1 - for help_remove()
        static size_t const c_nHazardPtrCount = c_nMaxHeight * 2 + 3; ///< Count of hazard pointer required for the skip-list

    protected:
        typedef typename node_type::atomic_marked_ptr   atomic_node_ptr;   ///< Atomic marked node pointer
        typedef typename node_type::marked_ptr          marked_node_ptr;   ///< Node marked pointer

    protected:
        //@cond
        typedef skip_list::details::intrusive_node_builder< node_type, atomic_node_ptr, allocator_type > intrusive_node_builder;

        typedef typename std::conditional<
            std::is_same< typename traits::internal_node_builder, cds::opt::none >::value
            ,intrusive_node_builder
            ,typename traits::internal_node_builder
        >::type node_builder;

        typedef std::unique_ptr< node_type, typename node_builder::node_disposer > scoped_node_ptr;

        struct position {
            node_type *   pPrev[ c_nMaxHeight ];
            node_type *   pSucc[ c_nMaxHeight ];

            typename gc::template GuardArray< c_nMaxHeight * 2 > guards;   ///< Guards array for pPrev/pSucc
            node_type *   pCur;   // guarded by one of guards
        };
        //@endcond

    public:
        /// Default constructor
        /**
            The constructor checks whether the count of guards is enough
            for skip-list and may raise an exception if not.
        */
        SkipListSet()
            : m_Head( c_nMaxHeight )
            , m_nHeight( c_nMinHeight )
        {
            static_assert( (std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type" );

            gc::check_available_guards( c_nHazardPtrCount );

            // Barrier for head node
            atomics::atomic_thread_fence( memory_model::memory_order_release );
        }

        /// Clears and destructs the skip-list
        ~SkipListSet()
        {
            destroy();
        }

    public:
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Iterator type
        /**
            The forward iterator has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (like as \p gc::HP), a guard is a limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if the limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary because it contains thread-private GC's guard.
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
        typedef skip_list::details::iterator< gc, node_traits, back_off, false >  iterator;

        /// Const iterator type
        typedef skip_list::details::iterator< gc, node_traits, back_off, true >   const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        iterator begin()
        {
            return iterator( *m_Head.head());
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator begin() const
        {
            return const_iterator( *m_Head.head());
        }
        /// Returns a forward const iterator addressing the first element in a set
        const_iterator cbegin() const
        {
            return const_iterator( *m_Head.head());
        }

        /// Returns a forward iterator that addresses the location succeeding the last element in a set.
        iterator end()
        {
            return iterator();
        }

        /// Returns a forward const iterator that addresses the location succeeding the last element in a set.
        const_iterator end() const
        {
            return const_iterator();
        }
        /// Returns a forward const iterator that addresses the location succeeding the last element in a set.
        const_iterator cend() const
        {
            return const_iterator();
        }
    //@}

    public:
        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            typename gc::Guard gNew;
            gNew.assign( &val );

            node_type * pNode = node_traits::to_node_ptr( val );
            scoped_node_ptr scp( pNode );
            unsigned int nHeight = pNode->height();
            bool bTowerOk = pNode->has_tower(); // nHeight > 1 && pNode->get_tower() != nullptr;
            bool bTowerMade = false;

            position pos;
            while ( true )
            {
                if ( find_position( val, pos, key_comparator(), true )) {
                    // scoped_node_ptr deletes the node tower if we create it
                    if ( !bTowerMade )
                        scp.release();

                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( !bTowerOk ) {
                    build_node( pNode );
                    nHeight = pNode->height();
                    bTowerMade = pNode->has_tower();
                    bTowerOk = true;
                }

                if ( !insert_at_position( val, pNode, pos, f )) {
                    m_Stat.onInsertRetry();
                    continue;
                }

                increase_height( nHeight );
                ++m_ItemCounter;
                m_Stat.onAddNode( nHeight );
                m_Stat.onInsertSuccess();
                scp.release();
                return true;
            }
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted into the set
            iff \p bInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor \p func signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            i.e. the node has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bInsert = true )
        {
            typename gc::Guard gNew;
            gNew.assign( &val );

            node_type * pNode = node_traits::to_node_ptr( val );
            scoped_node_ptr scp( pNode );
            unsigned int nHeight = pNode->height();
            bool bTowerOk = pNode->has_tower();
            bool bTowerMade = false;

            position pos;
            while ( true )
            {
                bool bFound = find_position( val, pos, key_comparator(), true );
                if ( bFound ) {
                    // scoped_node_ptr deletes the node tower if we create it before
                    if ( !bTowerMade )
                        scp.release();

                    func( false, *node_traits::to_value_ptr(pos.pCur), val );
                    m_Stat.onUpdateExist();
                    return std::make_pair( true, false );
                }

                if ( !bInsert ) {
                    scp.release();
                    return std::make_pair( false, false );
                }

                if ( !bTowerOk ) {
                    build_node( pNode );
                    nHeight = pNode->height();
                    bTowerMade = pNode->has_tower();
                    bTowerOk = true;
                }

                if ( !insert_at_position( val, pNode, pos, [&func]( value_type& item ) { func( true, item, item ); })) {
                    m_Stat.onInsertRetry();
                    continue;
                }

                increase_height( nHeight );
                ++m_ItemCounter;
                scp.release();
                m_Stat.onAddNode( nHeight );
                m_Stat.onUpdateNew();
                return std::make_pair( true, true );
            }
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it from the set
            if it is found and is equal to \p val.

            Difference between \p erase() and \p %unlink() functions: \p %erase() finds <i>a key</i>
            and deletes the item found. \p %unlink() finds an item by key and deletes it
            only if \p val is an item of that set, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            The \p disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            position pos;

            if ( !find_position( val, pos, key_comparator(), false )) {
                m_Stat.onUnlinkFailed();
                return false;
            }

            node_type * pDel = pos.pCur;
            assert( key_comparator()( *node_traits::to_value_ptr( pDel ), val ) == 0 );

            unsigned int nHeight = pDel->height();
            typename gc::Guard gDel;
            gDel.assign( node_traits::to_value_ptr(pDel));

            if ( node_traits::to_value_ptr( pDel ) == &val && try_remove_at( pDel, pos, [](value_type const&) {} )) {
                --m_ItemCounter;
                m_Stat.onRemoveNode( nHeight );
                m_Stat.onUnlinkSuccess();
                return true;
            }

            m_Stat.onUnlinkFailed();
            return false;
        }

        /// Extracts the item from the set with specified \p key
        /** \anchor cds_intrusive_SkipListSet_hp_extract
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns it as \p guarded_ptr object.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            The \p disposer specified in \p Traits class template parameter is called automatically
            by garbage collector \p GC specified in class' template parameters when returned \p guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::SkipListSet< cds::gc::HP, foo, my_traits >  skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp(theList.extract( 5 ));
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
            return extract_( key, key_comparator());
        }

        /// Extracts the item from the set with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_hp_extract "extract(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return extract_( key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Extracts an item with minimal key from the list
        /**
            The function searches an item with minimal key, unlinks it, and returns it as \p guarded_ptr object.
            If the skip-list is empty the function returns an empty guarded pointer.

            @note Due the concurrent nature of the list, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost item and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of list traversing.

            The \p disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC automatically when returned \p guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::SkipListSet< cds::gc::HP, foo, my_traits >  skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp(theList.extract_min());
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        guarded_ptr extract_min()
        {
            return extract_min_();
        }

        /// Extracts an item with maximal key from the list
        /**
            The function searches an item with maximal key, unlinks it, and returns the pointer to item
            as \p guarded_ptr object.
            If the skip-list is empty the function returns an empty \p guarded_ptr.

            @note Due the concurrent nature of the list, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost item and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key greater than rightmost item's key.
            So, the function returns the item with maximum key at the moment of list traversing.

            The \p disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously when returned \ref guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::SkipListSet< cds::gc::HP, foo, my_traits > skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp( theList.extract_max( gp ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        guarded_ptr extract_max()
        {
            return extract_max_();
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_SkipListSet_hp_erase
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_( key, key_comparator(), [](value_type const&) {} );
        }

        /// Deletes the item from the set with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_hp_erase "erase(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_( key, cds::opt::details::make_comparator_from_less<Less>(), [](value_type const&) {} );
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_SkipListSet_hp_erase_func
            The function searches an item with key equal to \p key in the set,
            call \p f functor with item found, unlinks it from the set, and returns \p true.
            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If the item with key equal to \p key is not found the function return \p false.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_( key, key_comparator(), f );
        }

        /// Deletes the item from the set with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_hp_erase_func "erase(Q const&, Func)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        /// Finds \p key
        /** \anchor cds_intrusive_SkipListSet_hp_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set \p item. If such access is
            possible you must provide your own synchronization on item level to exclude unsafe item modifications.

            Note the compare functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_with_( key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_with_( key, key_comparator(), f );
        }
        //@endcond

        /// Finds the key \p key with \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_hp_find_func "find(Q&, Func)"
            but \p pred is used for key compare.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return find_with_( key, key_comparator(), [](value_type& , Q const& ) {} );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), [](value_type& , Q const& ) {} );
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
        /** \anchor cds_intrusive_SkipListSet_hp_get
            The function searches the item with key equal to \p key
            and returns the pointer to the item found as \p guarded_ptr.
            If \p key is not found the function returns an empt guarded pointer.

            The \p disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously when returned \ref guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::SkipListSet< cds::gc::HP, foo, my_traits >  skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp(theList.get( 5 ));
                if ( gp ) {
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
        guarded_ptr get( Q const& key )
        {
            return get_with_( key, key_comparator());
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_hp_get "get( Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return get_with_( key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Returns item count in the set
        /**
            The value returned depends on item counter type provided by \p Traits template parameter.
            If it is \p atomicity::empty_item_counter this function always returns 0.
            Therefore, the function is not suitable for checking the set emptiness, use \p empty()
            for this purpose.
        */
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Checks if the set is empty
        bool empty() const
        {
            return m_Head.head()->next( 0 ).load( memory_model::memory_order_relaxed ) == nullptr;
        }

        /// Clears the set (not atomic)
        /**
            The function unlink all items from the set.
            The function is not atomic, i.e., in multi-threaded environment with parallel insertions
            this sequence
            \code
            set.clear();
            assert( set.empty());
            \endcode
            the assertion could be raised.

            For each item the \ref disposer will be called after unlinking.
        */
        void clear()
        {
            while ( extract_min_());
        }

        /// Returns maximum height of skip-list. The max height is a constant for each object and does not exceed 32.
        static constexpr unsigned int max_height() noexcept
        {
            return c_nMaxHeight;
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

    protected:
        //@cond
        unsigned int random_level()
        {
            // Random generator produces a number from range [0..31]
            // We need a number from range [1..32]
            return m_RandomLevelGen() + 1;
        }

        template <typename Q>
        node_type * build_node( Q v )
        {
            return node_builder::make_tower( v, m_RandomLevelGen );
        }

        static value_type * gc_protect( marked_node_ptr p )
        {
            return node_traits::to_value_ptr( p.ptr());
        }

        static void dispose_node( void* p )
        {
            assert( p != nullptr );
            value_type* pVal = reinterpret_cast<value_type*>( p );
            typename node_builder::node_disposer()( node_traits::to_node_ptr( pVal ));
            disposer()( pVal );
        }

        void help_remove( int nLevel, node_type* pPred, marked_node_ptr pCur )
        {
            if ( pCur->is_upper_level( nLevel )) {
                marked_node_ptr p( pCur.ptr());
                typename gc::Guard hp;
                marked_node_ptr pSucc = hp.protect( pCur->next( nLevel ), gc_protect );

                if ( pSucc.bits() &&
                     pPred->next( nLevel ).compare_exchange_strong( p, marked_node_ptr( pSucc.ptr()),
                        memory_model::memory_order_acquire, atomics::memory_order_relaxed ))
                {
                    if ( pCur->level_unlinked()) {
                        gc::retire( node_traits::to_value_ptr( pCur.ptr()), dispose_node );
                        m_Stat.onEraseWhileFind();
                    }
                }
            }
        }

        template <typename Q, typename Compare >
        bool find_position( Q const& val, position& pos, Compare cmp, bool bStopIfFound )
        {
            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;

            // Hazard pointer array:
            //  pPred: [nLevel * 2]
            //  pSucc: [nLevel * 2 + 1]

        retry:
            pPred = m_Head.head();
            int nCmp = 1;

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {
                pos.guards.assign( nLevel * 2, node_traits::to_value_ptr( pPred ));
                while ( true ) {
                    pCur = pos.guards.protect( nLevel * 2 + 1, pPred->next( nLevel ), gc_protect );
                    if ( pCur.bits()) {
                        // pCur.bits() means that pPred is logically deleted
                        goto retry;
                    }

                    if ( pCur.ptr() == nullptr ) {
                        // end of list at level nLevel - goto next level
                        break;
                    }

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted
                        // try to help deleting pCur
                        help_remove( nLevel, pPred, pCur );
                        goto retry;
                    }
                    else {
                        nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                        if ( nCmp < 0 ) {
                            pPred = pCur.ptr();
                            pos.guards.copy( nLevel * 2, nLevel * 2 + 1 );   // pPrev guard := cur guard
                        }
                        else if ( nCmp == 0 && bStopIfFound )
                            goto found;
                        else
                            break;
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }

            if ( nCmp != 0 )
                return false;

        found:
            pos.pCur = pCur.ptr();
            return pCur.ptr() && nCmp == 0;
        }

        bool find_min_position( position& pos )
        {
            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;

            // Hazard pointer array:
            //  pPred: [nLevel * 2]
            //  pSucc: [nLevel * 2 + 1]

        retry:
            pPred = m_Head.head();

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {
                pos.guards.assign( nLevel * 2, node_traits::to_value_ptr( pPred ));
                pCur = pos.guards.protect( nLevel * 2 + 1, pPred->next( nLevel ), gc_protect );

                // pCur.bits() means that pPred is logically deleted
                // head cannot be deleted
                assert( pCur.bits() == 0 );

                if ( pCur.ptr()) {

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted.
                        // try to help deleting pCur
                        help_remove( nLevel, pPred, pCur );
                        goto retry;
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }

            return ( pos.pCur = pCur.ptr()) != nullptr;
        }

        bool find_max_position( position& pos )
        {
            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;

            // Hazard pointer array:
            //  pPred: [nLevel * 2]
            //  pSucc: [nLevel * 2 + 1]

        retry:
            pPred = m_Head.head();

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {
                pos.guards.assign( nLevel * 2, node_traits::to_value_ptr( pPred ));
                while ( true ) {
                    pCur = pos.guards.protect( nLevel * 2 + 1, pPred->next( nLevel ), gc_protect );
                    if ( pCur.bits()) {
                        // pCur.bits() means that pPred is logically deleted
                        goto retry;
                    }

                    if ( pCur.ptr() == nullptr ) {
                        // end of the list at level nLevel - goto next level
                        break;
                    }

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted.
                        // try to help deleting pCur
                        help_remove( nLevel, pPred, pCur );
                        goto retry;
                    }
                    else {
                        if ( !pSucc.ptr())
                            break;

                        pPred = pCur.ptr();
                        pos.guards.copy( nLevel * 2, nLevel * 2 + 1 );
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }

            return ( pos.pCur = pCur.ptr()) != nullptr;
        }

        bool renew_insert_position( value_type& val, node_type * pNode, position& pos )
        {
            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;
            key_comparator cmp;

            // Hazard pointer array:
            //  pPred: [nLevel * 2]
            //  pSucc: [nLevel * 2 + 1]

        retry:
            pPred = m_Head.head();
            int nCmp = 1;

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {
                pos.guards.assign( nLevel * 2, node_traits::to_value_ptr( pPred ));
                while ( true ) {
                    pCur = pos.guards.protect( nLevel * 2 + 1, pPred->next( nLevel ), gc_protect );
                    if ( pCur.bits()) {
                        // pCur.bits() means that pPred is logically deleted
                        goto retry;
                    }

                    if ( pCur.ptr() == nullptr ) {
                        // end of list at level nLevel - goto next level
                        break;
                    }

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted
                        if ( pCur.ptr() == pNode ) {
                            // Node is removing while we are inserting it
                            return false;
                        }
                        // try to help deleting pCur
                        help_remove( nLevel, pPred, pCur );
                        goto retry;
                    }
                    else {
                        nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                        if ( nCmp < 0 ) {
                            pPred = pCur.ptr();
                            pos.guards.copy( nLevel * 2, nLevel * 2 + 1 );   // pPrev guard := cur guard
                        }
                        else
                            break;
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }

            return nCmp == 0;
        }

        template <typename Func>
        bool insert_at_position( value_type& val, node_type * pNode, position& pos, Func f )
        {
            unsigned int const nHeight = pNode->height();

            for ( unsigned int nLevel = 1; nLevel < nHeight; ++nLevel )
                pNode->next( nLevel ).store( marked_node_ptr(), memory_model::memory_order_relaxed );

            // Insert at level 0
            {
                marked_node_ptr p( pos.pSucc[0] );
                pNode->next( 0 ).store( p, memory_model::memory_order_release );
                if ( !pos.pPrev[0]->next( 0 ).compare_exchange_strong( p, marked_node_ptr( pNode ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    return false;

                f( val );
            }

            // Insert at level 1..max
            for ( unsigned int nLevel = 1; nLevel < nHeight; ++nLevel ) {
                marked_node_ptr p;
                while ( true ) {
                    marked_node_ptr pSucc( pos.pSucc[nLevel] );

                    // Set pNode->next
                    // pNode->next can have "logical deleted" flag if another thread is removing pNode right now
                    if ( !pNode->next( nLevel ).compare_exchange_strong( p, pSucc,
                        memory_model::memory_order_release, atomics::memory_order_acquire ))
                    {
                        // pNode has been marked as removed while we are inserting it
                        // Stop inserting
                        assert( p.bits() != 0 );

                        // Here pNode is linked at least level 0 so level_unlinked() cannot returns true
                        CDS_VERIFY_FALSE( pNode->level_unlinked( nHeight - nLevel ));

                        // pNode is linked up to nLevel - 1
                        // Remove it via find_position()
                        find_position( val, pos, key_comparator(), false );

                        m_Stat.onLogicDeleteWhileInsert();
                        return true;
                    }
                    p = pSucc;

                    // Link pNode into the list at nLevel
                    if ( pos.pPrev[nLevel]->next( nLevel ).compare_exchange_strong( pSucc, marked_node_ptr( pNode ),
                        memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    {
                        // go to next level
                        break;
                    }

                    // Renew insert position
                    m_Stat.onRenewInsertPosition();

                    if ( !renew_insert_position( val, pNode, pos )) {
                        // The node has been deleted while we are inserting it
                        // Update current height for concurent removing
                        CDS_VERIFY_FALSE( pNode->level_unlinked( nHeight - nLevel ));

                        m_Stat.onRemoveWhileInsert();

                        // help to removing val
                        find_position( val, pos, key_comparator(), false );
                        return true;
                    }
                }
            }
            return true;
        }

        template <typename Func>
        bool try_remove_at( node_type * pDel, position& pos, Func f )
        {
            assert( pDel != nullptr );

            marked_node_ptr pSucc;
            back_off bkoff;

            // logical deletion (marking)
            for ( unsigned int nLevel = pDel->height() - 1; nLevel > 0; --nLevel ) {
                pSucc = pDel->next( nLevel ).load( memory_model::memory_order_relaxed );
                if ( pSucc.bits() == 0 ) {
                    bkoff.reset();
                    while ( !( pDel->next( nLevel ).compare_exchange_weak( pSucc, pSucc | 1,
                        memory_model::memory_order_release, atomics::memory_order_acquire )
                        || pSucc.bits() != 0 ))
                    {
                        bkoff();
                        m_Stat.onMarkFailed();
                    }
                }
            }

            marked_node_ptr p( pDel->next( 0 ).load( memory_model::memory_order_relaxed ).ptr());
            while ( true ) {
                if ( pDel->next( 0 ).compare_exchange_strong( p, p | 1, memory_model::memory_order_release, atomics::memory_order_acquire ))
                {
                    f( *node_traits::to_value_ptr( pDel ));

                    // Physical deletion
                    // try fast erase
                    p = pDel;

                    for ( int nLevel = static_cast<int>( pDel->height() - 1 ); nLevel >= 0; --nLevel ) {

                        pSucc = pDel->next( nLevel ).load( memory_model::memory_order_acquire );
                        if ( pos.pPrev[nLevel]->next( nLevel ).compare_exchange_strong( p, marked_node_ptr( pSucc.ptr()),
                            memory_model::memory_order_acq_rel, atomics::memory_order_relaxed ))
                        {
                            pDel->level_unlinked();
                        }
                        else {
                            // Make slow erase
#       ifdef CDS_DEBUG
                            if ( find_position( *node_traits::to_value_ptr( pDel ), pos, key_comparator(), false ))
                                assert( pDel != pos.pCur );
#       else
                            find_position( *node_traits::to_value_ptr( pDel ), pos, key_comparator(), false );
#       endif
                            m_Stat.onSlowErase();
                            return true;
                        }
                    }

                    // Fast erasing success
                    gc::retire( node_traits::to_value_ptr( pDel ), dispose_node );
                    m_Stat.onFastErase();
                    return true;
                }
                else if ( p.bits()) {
                    // Another thread is deleting pDel right now
                    m_Stat.onEraseContention();
                    return false;
                }
                m_Stat.onEraseRetry();
                bkoff();
            }
        }

        enum finsd_fastpath_result {
            find_fastpath_found,
            find_fastpath_not_found,
            find_fastpath_abort
        };
        template <typename Q, typename Compare, typename Func>
        finsd_fastpath_result find_fastpath( Q& val, Compare cmp, Func f )
        {
            node_type * pPred;
            marked_node_ptr pCur;
            marked_node_ptr pNull;

            // guard array:
            // 0 - pPred on level N
            // 1 - pCur on level N
            typename gc::template GuardArray<2> guards;
            back_off bkoff;
            unsigned attempt = 0;

        try_again:
            pPred = m_Head.head();
            for ( int nLevel = static_cast<int>( m_nHeight.load( memory_model::memory_order_relaxed ) - 1 ); nLevel >= 0; --nLevel ) {
                pCur = guards.protect( 1, pPred->next( nLevel ), gc_protect );

                while ( pCur != pNull ) {
                    if ( pCur.bits()) {
                        // pPred is being removed
                        if ( ++attempt < 4 ) {
                            bkoff();
                            goto try_again;
                        }

                        return find_fastpath_abort;
                    }

                    if ( pCur.ptr()) {
                        int nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                        if ( nCmp < 0 ) {
                            guards.copy( 0, 1 );
                            pPred = pCur.ptr();
                            pCur = guards.protect( 1, pCur->next( nLevel ), gc_protect );
                        }
                        else if ( nCmp == 0 ) {
                            // found
                            f( *node_traits::to_value_ptr( pCur.ptr()), val );
                            return find_fastpath_found;
                        }
                        else {
                            // pCur > val - go down
                            break;
                        }
                    }
                }
            }

            return find_fastpath_not_found;
        }

        template <typename Q, typename Compare, typename Func>
        bool find_slowpath( Q& val, Compare cmp, Func f )
        {
            position pos;
            if ( find_position( val, pos, cmp, true )) {
                assert( cmp( *node_traits::to_value_ptr( pos.pCur ), val ) == 0 );

                f( *node_traits::to_value_ptr( pos.pCur ), val );
                return true;
            }
            else
                return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool find_with_( Q& val, Compare cmp, Func f )
        {
            switch ( find_fastpath( val, cmp, f )) {
            case find_fastpath_found:
                m_Stat.onFindFastSuccess();
                return true;
            case find_fastpath_not_found:
                m_Stat.onFindFastFailed();
                return false;
            default:
                break;
            }

            if ( find_slowpath( val, cmp, f )) {
                m_Stat.onFindSlowSuccess();
                return true;
            }

            m_Stat.onFindSlowFailed();
            return false;
        }

        template <typename Q, typename Compare>
        guarded_ptr get_with_( Q const& val, Compare cmp )
        {
            guarded_ptr gp;
            if ( find_with_( val, cmp, [&gp]( value_type& found, Q const& ) { gp.reset( &found ); } ))
                return gp;
            return guarded_ptr();
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_( Q const& val, Compare cmp, Func f )
        {
            position pos;

            if ( !find_position( val, pos, cmp, false )) {
                m_Stat.onEraseFailed();
                return false;
            }

            node_type * pDel = pos.pCur;
            typename gc::Guard gDel;
            gDel.assign( node_traits::to_value_ptr( pDel ));
            assert( cmp( *node_traits::to_value_ptr( pDel ), val ) == 0 );

            unsigned int nHeight = pDel->height();
            if ( try_remove_at( pDel, pos, f )) {
                --m_ItemCounter;
                m_Stat.onRemoveNode( nHeight );
                m_Stat.onEraseSuccess();
                return true;
            }

            m_Stat.onEraseFailed();
            return false;
        }

        template <typename Q, typename Compare>
        guarded_ptr extract_( Q const& val, Compare cmp )
        {
            position pos;

            guarded_ptr gp;
            for (;;) {
                if ( !find_position( val, pos, cmp, false )) {
                    m_Stat.onExtractFailed();
                    return guarded_ptr();
                }

                node_type * pDel = pos.pCur;
                gp.reset( node_traits::to_value_ptr( pDel ));
                assert( cmp( *node_traits::to_value_ptr( pDel ), val ) == 0 );

                unsigned int nHeight = pDel->height();
                if ( try_remove_at( pDel, pos, []( value_type const& ) {} )) {
                    --m_ItemCounter;
                    m_Stat.onRemoveNode( nHeight );
                    m_Stat.onExtractSuccess();
                    return gp;
                }
                m_Stat.onExtractRetry();
            }
        }

        guarded_ptr extract_min_()
        {
            position pos;

            guarded_ptr gp;
            for ( ;;) {
                if ( !find_min_position( pos )) {
                    // The list is empty
                    m_Stat.onExtractMinFailed();
                    return guarded_ptr();
                }

                node_type * pDel = pos.pCur;

                unsigned int nHeight = pDel->height();
                gp.reset( node_traits::to_value_ptr( pDel ));

                if ( try_remove_at( pDel, pos, []( value_type const& ) {} )) {
                    --m_ItemCounter;
                    m_Stat.onRemoveNode( nHeight );
                    m_Stat.onExtractMinSuccess();
                    return gp;
                }

                m_Stat.onExtractMinRetry();
            }
        }

        guarded_ptr extract_max_()
        {
            position pos;

            guarded_ptr gp;
            for ( ;;) {
                if ( !find_max_position( pos )) {
                    // The list is empty
                    m_Stat.onExtractMaxFailed();
                    return guarded_ptr();
                }

                node_type * pDel = pos.pCur;

                unsigned int nHeight = pDel->height();
                gp.reset( node_traits::to_value_ptr( pDel ));

                if ( try_remove_at( pDel, pos, []( value_type const& ) {} )) {
                    --m_ItemCounter;
                    m_Stat.onRemoveNode( nHeight );
                    m_Stat.onExtractMaxSuccess();
                    return gp;
                }

                m_Stat.onExtractMaxRetry();
            }
        }

        void increase_height( unsigned int nHeight )
        {
            unsigned int nCur = m_nHeight.load( memory_model::memory_order_relaxed );
            if ( nCur < nHeight )
                m_nHeight.compare_exchange_strong( nCur, nHeight, memory_model::memory_order_relaxed, atomics::memory_order_relaxed );
        }

        void destroy()
        {
            node_type* p = m_Head.head()->next( 0 ).load( atomics::memory_order_relaxed ).ptr();
            while ( p ) {
                node_type* pNext = p->next( 0 ).load( atomics::memory_order_relaxed ).ptr();
                dispose_node( node_traits::to_value_ptr( p ));
                p = pNext;
            }
        }

        //@endcond

    private:
        //@cond
        skip_list::details::head_node< node_type > m_Head;   ///< head tower (max height)

        random_level_generator      m_RandomLevelGen; ///< random level generator instance
        atomics::atomic<unsigned int> m_nHeight;      ///< estimated high level
        item_counter                m_ItemCounter;    ///< item counter
        mutable stat                m_Stat;           ///< internal statistics
        //@endcond
    };

}} // namespace cds::intrusive


#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_SKIP_LIST_H
