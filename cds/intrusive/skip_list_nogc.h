// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_SKIP_LIST_NOGC_H
#define CDSLIB_INTRUSIVE_SKIP_LIST_NOGC_H

#include <type_traits>
#include <memory>
#include <cds/gc/nogc.h>
#include <cds/intrusive/details/skip_list_base.h>
#include <cds/opt/compare.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace intrusive {

    //@cond
    namespace skip_list {
        template <typename Tag>
        class node< cds::gc::nogc, Tag >
        {
        public:
            typedef cds::gc::nogc   gc;  ///< Garbage collector
            typedef Tag             tag; ///< tag

            typedef atomics::atomic<node * > atomic_ptr;
            typedef atomic_ptr tower_item_type;

        protected:
            atomic_ptr      m_pNext;   ///< Next item in bottom-list (list at level 0)
            unsigned int    m_nHeight; ///< Node height (size of m_arrNext array). For node at level 0 the height is 1.
            atomic_ptr *    m_arrNext; ///< Array of next items for levels 1 .. m_nHeight - 1. For node at level 0 \p m_arrNext is \p nullptr

        public:
            /// Constructs a node of height 1 (a bottom-list node)
            node()
                : m_pNext( nullptr )
                , m_nHeight(1)
                , m_arrNext( nullptr )
            {}

            /// Constructs a node of height \p nHeight
            void make_tower( unsigned int nHeight, atomic_ptr * nextTower )
            {
                assert( nHeight > 0 );
                assert( (nHeight == 1 && nextTower == nullptr)  // bottom-list node
                        || (nHeight > 1 && nextTower != nullptr)   // node at level of more than 0
                    );

                m_arrNext = nextTower;
                m_nHeight = nHeight;
            }

            atomic_ptr * release_tower()
            {
                atomic_ptr * pTower = m_arrNext;
                m_arrNext = nullptr;
                m_nHeight = 1;
                return pTower;
            }

            atomic_ptr * get_tower() const
            {
                return m_arrNext;
            }

            /// Access to element of next pointer array
            atomic_ptr& next( unsigned int nLevel )
            {
                assert( nLevel < height());
                assert( nLevel == 0 || (nLevel > 0 && m_arrNext != nullptr));

                return nLevel ? m_arrNext[ nLevel - 1] : m_pNext;
            }

            /// Access to element of next pointer array (const version)
            atomic_ptr const& next( unsigned int nLevel ) const
            {
                assert( nLevel < height());
                assert( nLevel == 0 || nLevel > 0 && m_arrNext != nullptr );

                return nLevel ? m_arrNext[ nLevel - 1] : m_pNext;
            }

            /// Access to element of next pointer array (same as \ref next function)
            atomic_ptr& operator[]( unsigned int nLevel )
            {
                return next( nLevel );
            }

            /// Access to element of next pointer array (same as \ref next function)
            atomic_ptr const& operator[]( unsigned int nLevel ) const
            {
                return next( nLevel );
            }

            /// Height of the node
            unsigned int height() const
            {
                return m_nHeight;
            }

            /// Clears internal links
            void clear()
            {
                assert( m_arrNext == nullptr );
                m_pNext.store( nullptr, atomics::memory_order_release );
            }

            bool is_cleared() const
            {
                return m_pNext.load( atomics::memory_order_relaxed ) == nullptr
                    && m_arrNext == nullptr
                    && m_nHeight <= 1
;
            }
        };
    } // namespace skip_list

    namespace skip_list { namespace details {

        template <typename NodeTraits, typename BackOff, bool IsConst>
        class iterator< cds::gc::nogc, NodeTraits, BackOff, IsConst>
        {
        public:
            typedef cds::gc::nogc                       gc;
            typedef NodeTraits                          node_traits;
            typedef BackOff                             back_off;
            typedef typename node_traits::node_type     node_type;
            typedef typename node_traits::value_type    value_type;
            static constexpr bool const c_isConst = IsConst;

            typedef typename std::conditional< c_isConst, value_type const &, value_type &>::type   value_ref;
            friend class iterator< gc, node_traits, back_off, !c_isConst >;

        protected:
            typedef typename node_type::atomic_ptr atomic_ptr;
            node_type * m_pNode;

        public: // for internal use only!!!
            iterator( node_type& refHead )
                : m_pNode( refHead[0].load( atomics::memory_order_relaxed ))
            {}

            static iterator from_node( node_type * pNode )
            {
                iterator it;
                it.m_pNode = pNode;
                return it;
            }

        public:
            iterator()
                : m_pNode( nullptr )
            {}

            iterator( iterator const& s)
                : m_pNode( s.m_pNode )
            {}

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
                if ( m_pNode )
                    m_pNode = m_pNode->next(0).load( atomics::memory_order_relaxed );
                return *this;
            }

            iterator& operator =(const iterator& src)
            {
                m_pNode = src.m_pNode;
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

    /// Lock-free skip-list set (template specialization for gc::nogc)
    /** @ingroup cds_intrusive_map
        @anchor cds_intrusive_SkipListSet_nogc

        This specialization is so-called append-only when no item
        reclamation may be performed. The class does not support deleting of list item.

        See \ref cds_intrusive_SkipListSet_hp "SkipListSet" for description of skip-list.

        <b>Template arguments</b> :
        - \p T - type to be stored in the set. The type must be based on \p skip_list::node (for \p skip_list::base_hook)
            or it must have a member of type \p skip_list::node (for \p skip_list::member_hook).
        - \p Traits - type traits, default is \p skip_list::traits.
            It is possible to declare option-based list with \p cds::intrusive::skip_list::make_traits metafunction
            istead of \p Traits template argument.

        <b>Iterators</b>

        The class supports a forward iterator (\ref iterator and \ref const_iterator).
        The iteration is ordered.

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
        Note, the iterator object returned by \ref end, \p cend member functions points to \p nullptr and should not be dereferenced.

        <b>How to use</b>

        You should incorporate \p skip_list::node into your struct \p T and provide
        appropriate \p skip_list::traits::hook in your \p Traits template parameters. Usually, for \p Traits you
        define a struct based on \p skip_list::traits.

        Example for base hook:
        \code
        #include <cds/intrusive/skip_list_nogc.h>

        // Data stored in skip list
        struct my_data: public cds::intrusive::skip_list::node< cds::gc::nogc >
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


        // Declare traits
        struct my_traits: public cds::intrusive::skip_list::traits
        {
            typedef cds::intrusive::skip_list::base_hook< cds::opt::gc< cds::gc::nogc > > hook;
            typedef my_data_cmp compare;
        };

        // Declare skip-list set type
        typedef cds::intrusive::SkipListSet< cds::gc::nogc, my_data, my_traits > traits_based_set;
        \endcode

        Equivalent option-based code:
        \code
        // GC-related specialization
        #include <cds/intrusive/skip_list_nogc.h>

        struct my_data {
            // see above
        };
        struct compare {
            // see above
        };

        // Declare option-based skip-list set
        typedef cds::intrusive::SkipListSet< cds::gc::nogc
            ,my_data
            , typename cds::intrusive::skip_list::make_traits<
                cds::intrusive::opt::hook< cds::intrusive::skip_list::base_hook< cds::opt::gc< cds::gc::nogc > > >
                ,cds::intrusive::opt::compare< my_data_cmp >
            >::type
        > option_based_set;
        \endcode
    */
    template <
       typename T
#ifdef CDS_DOXYGEN_INVOKED
       ,typename Traits = skip_list::traits
#else
       ,typename Traits
#endif
    >
    class SkipListSet< cds::gc::nogc, T, Traits >
    {
    public:
        typedef cds::gc::nogc  gc;   ///< No garbage collector is used
        typedef T       value_type;  ///< type of value stored in the skip-list
        typedef Traits  traits;      ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator; ///< key comparison functor based on \p Traits::compare and \p Traits::less
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits; ///< node traits

        typedef typename traits::item_counter  item_counter;   ///< Item counting policy
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::random_level_generator    random_level_generator  ;   ///< random level generator
        typedef typename traits::allocator     allocator_type;   ///< allocator for maintaining array of next pointers of the node
        typedef typename traits::back_off      back_off;   ///< Back-off strategy
        typedef typename traits::stat          stat;       ///< internal statistics type
        typedef typename traits::disposer      disposer;   ///< disposer

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
        static unsigned int const c_nMinHeight = 3;
        //@endcond

    protected:
        typedef typename node_type::atomic_ptr  atomic_node_ptr;   ///< Atomic node pointer

    protected:
        //@cond
        typedef skip_list::details::intrusive_node_builder< node_type, atomic_node_ptr, allocator_type > intrusive_node_builder;

        typedef typename std::conditional<
            std::is_same< typename traits::internal_node_builder, cds::opt::none >::value
            ,intrusive_node_builder
            ,typename traits::internal_node_builder
        >::type node_builder;

        typedef std::unique_ptr< node_type, typename node_builder::node_disposer >    scoped_node_ptr;

        struct position {
            node_type *   pPrev[ c_nMaxHeight ];
            node_type *   pSucc[ c_nMaxHeight ];

            node_type *   pCur;
        };

        class head_node: public node_type
        {
            typename node_type::atomic_ptr   m_Tower[c_nMaxHeight];

        public:
            head_node( unsigned int nHeight )
            {
                for ( size_t i = 0; i < sizeof(m_Tower) / sizeof(m_Tower[0]); ++i )
                    m_Tower[i].store( nullptr, atomics::memory_order_relaxed );

                node_type::make_tower( nHeight, m_Tower );
            }

            node_type * head() const
            {
                return const_cast<node_type *>( static_cast<node_type const *>(this));
            }

            void clear()
            {
                for (unsigned int i = 0; i < sizeof(m_Tower) / sizeof(m_Tower[0]); ++i )
                    m_Tower[i].store( nullptr, atomics::memory_order_relaxed );
                node_type::m_pNext.store( nullptr, atomics::memory_order_relaxed );
            }
        };
        //@endcond

    protected:
        head_node                   m_Head;   ///< head tower (max height)

        random_level_generator      m_RandomLevelGen; ///< random level generator instance
        atomics::atomic<unsigned int>    m_nHeight;   ///< estimated high level
        item_counter                m_ItemCounter;    ///< item counter
        mutable stat                m_Stat;           ///< internal statistics

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

        static void dispose_node( node_type * pNode )
        {
            assert( pNode != nullptr );
            typename node_builder::node_disposer()( pNode );
            disposer()( node_traits::to_value_ptr( pNode ));
        }

        template <typename Q, typename Compare >
        bool find_position( Q const& val, position& pos, Compare cmp, bool bStopIfFound, bool bStrictSearch ) const
        {
            node_type * pPred;
            node_type * pSucc;
            node_type * pCur = nullptr;

            int nCmp = 1;

            unsigned int nHeight = c_nMaxHeight;
        retry:
            if ( !bStrictSearch )
                nHeight = m_nHeight.load( memory_model::memory_order_relaxed );
            pPred = m_Head.head();

            for ( int nLevel = (int) nHeight - 1; nLevel >= 0; --nLevel ) {
                while ( true ) {
                    pCur = pPred->next( nLevel ).load( memory_model::memory_order_relaxed );

                    if ( !pCur ) {
                        // end of the list at level nLevel - goto next level
                        break;
                    }

                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_relaxed );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ) != pCur
                        || pCur->next( nLevel ).load( memory_model::memory_order_acquire ) != pSucc )
                    {
                        goto retry;
                    }

                    nCmp = cmp( *node_traits::to_value_ptr( pCur ), val );
                    if ( nCmp < 0 )
                        pPred = pCur;
                    else if ( nCmp == 0 && bStopIfFound )
                        goto found;
                    else
                        break;
                }

                pos.pPrev[ nLevel ] = pPred;
                pos.pSucc[ nLevel ] = pCur;
            }

            if ( nCmp != 0 )
                return false;

        found:
            pos.pCur = pCur;
            return pCur && nCmp == 0;
        }

        template <typename Func>
        bool insert_at_position( value_type& val, node_type * pNode, position& pos, Func f )
        {
            unsigned int nHeight = pNode->height();

            for ( unsigned int nLevel = 1; nLevel < nHeight; ++nLevel )
                pNode->next( nLevel ).store( nullptr, memory_model::memory_order_relaxed );

            {
                node_type * p = pos.pSucc[0];
                pNode->next( 0 ).store( pos.pSucc[ 0 ], memory_model::memory_order_release );
                if ( !pos.pPrev[0]->next(0).compare_exchange_strong( p, pNode, memory_model::memory_order_release, memory_model::memory_order_relaxed )) {
                    return false;
                }
                f( val );
            }

            for ( unsigned int nLevel = 1; nLevel < nHeight; ++nLevel ) {
                node_type * p = nullptr;
                while ( true ) {
                    node_type * q = pos.pSucc[ nLevel ];

                    if ( pNode->next( nLevel ).compare_exchange_strong( p, q, memory_model::memory_order_release, memory_model::memory_order_relaxed )) {
                        p = q;
                        if ( pos.pPrev[nLevel]->next(nLevel).compare_exchange_strong( q, pNode, memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                            break;
                    }

                    // Renew insert position
                    find_position( val, pos, key_comparator(), false, true );
                }
            }
            return true;
        }

        template <typename Q, typename Compare, typename Func>
        node_type * find_with_( Q& val, Compare cmp, Func f ) const
        {
            position pos;
            if ( find_position( val, pos, cmp, true, false )) {
                assert( cmp( *node_traits::to_value_ptr( pos.pCur ), val ) == 0 );

                f( *node_traits::to_value_ptr( pos.pCur ), val );

                m_Stat.onFindFastSuccess();
                return pos.pCur;
            }
            else {
                m_Stat.onFindFastFailed();
                return nullptr;
            }
        }

        void increase_height( unsigned int nHeight )
        {
            unsigned int nCur = m_nHeight.load( memory_model::memory_order_relaxed );
            while ( nCur < nHeight && !m_nHeight.compare_exchange_weak( nCur, nHeight, memory_model::memory_order_acquire, atomics::memory_order_relaxed ));
        }
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

            // Barrier for head node
            atomics::atomic_thread_fence( memory_model::memory_order_release );
        }

        /// Clears and destructs the skip-list
        ~SkipListSet()
        {
            clear();
        }

    public:
    ///@name Forward iterators
    //@{
        /// Forward iterator
        /**
            The forward iterator for a split-list has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList
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

    protected:
        //@cond
        iterator nonconst_end() const
        {
            return iterator();
        }
        //@endcond

    public:
        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            node_type * pNode = node_traits::to_node_ptr( val );
            scoped_node_ptr scp( pNode );
            unsigned int nHeight = pNode->height();
            bool bTowerOk = nHeight > 1 && pNode->get_tower() != nullptr;
            bool bTowerMade = false;

            position pos;
            while ( true )
            {
                bool bFound = find_position( val, pos, key_comparator(), true, true );
                if ( bFound ) {
                    // scoped_node_ptr deletes the node tower if we create it
                    if ( !bTowerMade )
                        scp.release();

                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( !bTowerOk ) {
                    build_node( pNode );
                    nHeight = pNode->height();
                    bTowerMade =
                        bTowerOk = true;
                }

                if ( !insert_at_position( val, pNode, pos, []( value_type& ) {} )) {
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
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor can change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the set.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bInsert = true )
        {
            node_type * pNode = node_traits::to_node_ptr( val );
            scoped_node_ptr scp( pNode );
            unsigned int nHeight = pNode->height();
            bool bTowerOk = nHeight > 1 && pNode->get_tower() != nullptr;
            bool bTowerMade = false;

            position pos;
            while ( true )
            {
                bool bFound = find_position( val, pos, key_comparator(), true, true );
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
                    bTowerMade =
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

        /// Finds \p key
        /** \anchor cds_intrusive_SkipListSet_nogc_find_func
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
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            return find_with_( key, key_comparator(), f ) != nullptr;
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_with_( key, key_comparator(), f ) != nullptr;
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_nogc_find_func "find(Q&, Func)"
            but \p pred predicate is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), f ) != nullptr;
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), f ) != nullptr;
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns pointer to item found or \p nullptr.
        */
        template <typename Q>
        value_type * contains( Q const& key ) const
        {
            node_type * pNode = find_with_( key, key_comparator(), [](value_type& , Q const& ) {} );
            if ( pNode )
                return node_traits::to_value_ptr( pNode );
            return nullptr;
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        value_type * find( Q const& key ) const
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
        value_type * contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            node_type * pNode = find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), [](value_type& , Q const& ) {} );
            if ( pNode )
                return node_traits::to_value_ptr( pNode );
            return nullptr;
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        value_type * find_with( Q const& key, Less pred ) const
        {
            return contains( key, pred );
        }
        //@endcond

        /// Gets minimum key from the set
        /**
            If the set is empty the function returns \p nullptr
        */
        value_type * get_min() const
        {
            return node_traits::to_value_ptr( m_Head.head()->next( 0 ));
        }

        /// Gets maximum key from the set
        /**
            The function returns \p nullptr if the set is empty
        */
        value_type * get_max() const
        {
            node_type * pPred;

            unsigned int nHeight = m_nHeight.load( memory_model::memory_order_relaxed );
            pPred = m_Head.head();

            for ( int nLevel = (int) nHeight - 1; nLevel >= 0; --nLevel ) {
                while ( true ) {
                    node_type * pCur = pPred->next( nLevel ).load( memory_model::memory_order_relaxed );

                    if ( !pCur ) {
                        // end of the list at level nLevel - goto next level
                        break;
                    }
                    pPred = pCur;
                }
            }
            return pPred && pPred != m_Head.head() ? node_traits::to_value_ptr( pPred ) : nullptr;
        }

        /// Clears the set (non-atomic)
        /**
            The function is not atomic.
            Finding and/or inserting is prohibited while clearing.
            Otherwise an unpredictable result may be encountered.
            Thus, \p clear() may be used only for debugging purposes.
        */
        void clear()
        {
            node_type * pNode = m_Head.head()->next(0).load( memory_model::memory_order_relaxed );
            m_Head.clear();
            m_ItemCounter.reset();
            m_nHeight.store( c_nMinHeight, memory_model::memory_order_release );

            while ( pNode ) {
                node_type * pNext = pNode->next(0).load( memory_model::memory_order_relaxed );
                dispose_node( pNode );
                pNode = pNext;
            }
        }

        /// Returns item count in the set
        /**
            The value returned depends on item counter type provided by \p Traits template parameter.
            For \p atomicity::empty_item_counter the function always returns 0.
            The function is not suitable for checking the set emptiness, use \p empty().
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
    };

}} // namespace cds::intrusive


#endif // #ifndef CDSLIB_INTRUSIVE_SKIP_LIST_IMPL_H
