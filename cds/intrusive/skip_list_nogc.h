//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_SKIP_LIST_NOGC_H
#define __CDS_INTRUSIVE_SKIP_LIST_NOGC_H

#include <type_traits>
#include <memory>
#include <cds/gc/nogc.h>
#include <cds/intrusive/skip_list_base.h>
#include <cds/opt/compare.h>
#include <cds/ref.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace intrusive {

    //@cond
    namespace skip_list {
        template <typename Tag>
        class node< cds::gc::nogc, Tag >
        {
        public:
            typedef cds::gc::nogc   gc          ;   ///< Garbage collector
            typedef Tag             tag         ;   ///< tag

            typedef CDS_ATOMIC::atomic<node * > atomic_ptr;
            typedef atomic_ptr                  tower_item_type;

        protected:
            atomic_ptr      m_pNext     ;   ///< Next item in bottom-list (list at level 0)
            unsigned int    m_nHeight   ;   ///< Node height (size of m_arrNext array). For node at level 0 the height is 1.
            atomic_ptr *    m_arrNext   ;   ///< Array of next items for levels 1 .. m_nHeight - 1. For node at level 0 \p m_arrNext is \p NULL

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
                assert( nLevel < height() );
                assert( nLevel == 0 || (nLevel > 0 && m_arrNext != nullptr) );

                return nLevel ? m_arrNext[ nLevel - 1] : m_pNext;
            }

            /// Access to element of next pointer array (const version)
            atomic_ptr const& next( unsigned int nLevel ) const
            {
                assert( nLevel < height() );
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
                m_pNext.store( nullptr, CDS_ATOMIC::memory_order_release );
            }

            bool is_cleared() const
            {
                return m_pNext.load( CDS_ATOMIC::memory_order_relaxed ) == nullptr
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
            static bool const c_isConst = IsConst;

            typedef typename std::conditional< c_isConst, value_type const &, value_type &>::type   value_ref;

        protected:
            typedef typename node_type::atomic_ptr   atomic_ptr;
            node_type *             m_pNode;

        public: // for internal use only!!!
            iterator( node_type& refHead )
                : m_pNode( refHead[0].load( CDS_ATOMIC::memory_order_relaxed ) )
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
                    m_pNode = m_pNode->next(0).load( CDS_ATOMIC::memory_order_relaxed );
                return *this;
            }

            iterator& operator = (const iterator& src)
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

        This specialization is intended for so-called persistent usage when no item
        reclamation may be performed. The class does not support deleting of list item.

        See \ref cds_intrusive_SkipListSet_hp "SkipListSet" for description of skip-list.

        <b>Template arguments</b> :
        - \p T - type to be stored in the set. The type must be based on skip_list::node (for skip_list::base_hook)
            or it must have a member of type skip_list::node (for skip_list::member_hook).
        - \p Traits - type traits. See skip_list::type_traits for explanation.

        It is possible to declare option-based list with cds::intrusive::skip_list::make_traits metafunction istead of \p Traits template
        argument.
        Template argument list \p Options of cds::intrusive::skip_list::make_traits metafunction are:
        - opt::hook - hook used. Possible values are: skip_list::base_hook, skip_list::member_hook, skip_list::traits_hook.
            If the option is not specified, <tt>skip_list::base_hook<></tt> and gc::HP is used.
        - opt::compare - key comparison functor. No default functor is provided.
            If the option is not specified, the opt::less is used.
        - opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter that is no item counting.
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
        - skip_list::random_level_generator - random level generator. Can be skip_list::xorshift, skip_list::turbo_pascal or
            user-provided one. See skip_list::random_level_generator option description for explanation.
            Default is \p %skip_list::turbo_pascal.
        - opt::allocator - although the skip-list is an intrusive container,
            an allocator should be provided to maintain variable randomly-calculated height of the node
            since the node can contain up to 32 next pointers. The allocator option is used to allocate an array of next pointers
            for nodes which height is more than 1. Default is \ref CDS_DEFAULT_ALLOCATOR.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::Default is used.
        - opt::stat - internal statistics. Available types: skip_list::stat, skip_list::empty_stat (the default)
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer.
            The disposer is used only in object destructor and in \ref clear function.

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
        Note, the iterator object returned by \ref end, \p cend member functions points to \p NULL and should not be dereferenced.

        <b>How to use</b>

        You should incorporate skip_list::node into your struct \p T and provide
        appropriate skip_list::type_traits::hook in your \p Traits template parameters. Usually, for \p Traits you
        define a struct based on skip_list::type_traits.

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


        // Declare type_traits
        struct my_traits: public cds::intrusive::skip_list::type_traits
        {
            typedef cds::intrusive::skip_list::base_hook< cds::opt::gc< cds::gc::nogc > >   hook;
            typedef my_data_cmp compare;
        };

        // Declare skip-list set type
        typedef cds::intrusive::SkipListSet< cds::gc::nogc, my_data, my_traits >     traits_based_set;
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
       ,typename Traits = skip_list::type_traits
#else
       ,typename Traits
#endif
    >
    class SkipListSet< cds::gc::nogc, T, Traits >
    {
    public:
        typedef T       value_type      ;   ///< type of value stored in the skip-list
        typedef Traits  options         ;   ///< Traits template parameter

        typedef typename options::hook      hook        ;   ///< hook type
        typedef typename hook::node_type    node_type   ;   ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key comparison functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, options >::type key_comparator;
#   endif
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits

        typedef cds::gc::nogc  gc          ;   ///< No garbage collector is used
        typedef typename options::item_counter  item_counter ;   ///< Item counting policy used
        typedef typename options::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename options::random_level_generator    random_level_generator  ;   ///< random level generator
        typedef typename options::allocator     allocator_type  ;   ///< allocator for maintaining array of next pointers of the node
        typedef typename options::back_off      back_off    ;   ///< Back-off trategy
        typedef typename options::stat          stat        ;   ///< internal statistics type
        typedef typename options::disposer      disposer    ;   ///< disposer used

        /// Max node height. The actual node height should be in range <tt>[0 .. c_nMaxHeight)</tt>
        /**
            The max height is specified by \ref skip_list::random_level_generator "random level generator" constant \p m_nUpperBound
            but it should be no more than 32 (\ref skip_list::c_nHeightLimit).
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
        typedef typename node_type::atomic_ptr   atomic_node_ptr ;   ///< Atomic node pointer

    protected:
        //@cond
        typedef skip_list::details::intrusive_node_builder< node_type, atomic_node_ptr, allocator_type > intrusive_node_builder;

        typedef typename std::conditional<
            std::is_same< typename options::internal_node_builder, cds::opt::none >::value
            ,intrusive_node_builder
            ,typename options::internal_node_builder
        >::type node_builder;

        typedef std::unique_ptr< node_type, typename node_builder::node_disposer >    scoped_node_ptr;

        struct position {
            node_type *   pPrev[ c_nMaxHeight ];
            node_type *   pSucc[ c_nMaxHeight ];

            node_type *   pCur;
        };

#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        struct empty_insert_functor {
            void operator()( value_type& )
            {}
        };

        struct empty_find_functor {
            template <typename Q>
            void operator()( value_type& item, Q& val )
            {}
        };

        template <typename Func>
        struct insert_at_ensure_functor {
            Func m_func;
            insert_at_ensure_functor( Func f ) : m_func(f) {}

            void operator()( value_type& item )
            {
                cds::unref( m_func)( true, item, item );
            }
        };

#   endif // ifndef CDS_CXX11_LAMBDA_SUPPORT

        class head_node: public node_type
        {
            typename node_type::atomic_ptr   m_Tower[c_nMaxHeight];

        public:
            head_node( unsigned int nHeight )
            {
                for ( size_t i = 0; i < sizeof(m_Tower) / sizeof(m_Tower[0]); ++i )
                    m_Tower[i].store( nullptr, CDS_ATOMIC::memory_order_relaxed );

                node_type::make_tower( nHeight, m_Tower );
            }

            node_type * head() const
            {
                return const_cast<node_type *>( static_cast<node_type const *>(this));
            }

            void clear()
            {
                for (unsigned int i = 0; i < sizeof(m_Tower) / sizeof(m_Tower[0]); ++i )
                    m_Tower[i].store( nullptr, CDS_ATOMIC::memory_order_relaxed );
                node_type::m_pNext.store( nullptr, CDS_ATOMIC::memory_order_relaxed );
            }
        };
        //@endcond

    protected:
        head_node                   m_Head  ;   ///< head tower (max height)

        item_counter                m_ItemCounter       ;   ///< item counter
        random_level_generator      m_RandomLevelGen    ;   ///< random level generator instance
        CDS_ATOMIC::atomic<unsigned int>    m_nHeight   ;   ///< estimated high level
        mutable stat                m_Stat              ;   ///< internal statistics

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
                if ( !pos.pPrev[0]->next(0).compare_exchange_strong( p, pNode, memory_model::memory_order_release, memory_model::memory_order_relaxed ) ) {
                    return false;
                }
                cds::unref( f )( val );
            }

            for ( unsigned int nLevel = 1; nLevel < nHeight; ++nLevel ) {
                node_type * p = nullptr;
                while ( true ) {
                    node_type * q = pos.pSucc[ nLevel ];

                    if ( pNode->next( nLevel ).compare_exchange_strong( p, q, memory_model::memory_order_release, memory_model::memory_order_relaxed )) {
                        p = q;
                        if ( pos.pPrev[nLevel]->next(nLevel).compare_exchange_strong( q, pNode, memory_model::memory_order_release, memory_model::memory_order_relaxed ) )
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

                cds::unref(f)( *node_traits::to_value_ptr( pos.pCur ), val );

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
            while ( nCur < nHeight && !m_nHeight.compare_exchange_weak( nCur, nHeight, memory_model::memory_order_acquire, CDS_ATOMIC::memory_order_relaxed ) );
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
            CDS_ATOMIC::atomic_thread_fence( memory_model::memory_order_release );
        }

        /// Clears and destructs the skip-list
        ~SkipListSet()
        {
            clear();
        }

    public:
        /// Iterator type
        typedef skip_list::details::iterator< gc, node_traits, back_off, false >  iterator;

        /// Const iterator type
        typedef skip_list::details::iterator< gc, node_traits, back_off, true >   const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        iterator begin()
        {
            return iterator( *m_Head.head() );
        }

        /// Returns a forward const iterator addressing the first element in a set
        //@{
        const_iterator begin() const
        {
            return const_iterator( *m_Head.head() );
        }
        const_iterator cbegin()
        {
            return const_iterator( *m_Head.head() );
        }
        //@}

        /// Returns a forward iterator that addresses the location succeeding the last element in a set.
        iterator end()
        {
            return iterator();
        }

        /// Returns a forward const iterator that addresses the location succeeding the last element in a set.
        //@{
        const_iterator end() const
        {
            return const_iterator();
        }
        const_iterator cend()
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

#       ifndef CDS_CXX11_LAMBDA_SUPPORT
                if ( !insert_at_position( val, pNode, pos, empty_insert_functor() ))
#       else
                if ( !insert_at_position( val, pNode, pos, []( value_type& ) {} ))
#       endif
                {
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

        /// Ensures that the \p val exists in the set
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted into the set.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p ensure function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor can change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            You can pass \p func argument by value or by reference using <tt>boost::ref</tt> or cds::ref.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the set.
        */
        template <typename Func>
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            node_type * pNode = node_traits::to_node_ptr( val );
            scoped_node_ptr scp( pNode );
            unsigned int nHeight = pNode->height();
            bool bTowerOk = nHeight > 1 && pNode->get_tower() != nullptr;
            bool bTowerMade = false;

#       ifndef CDS_CXX11_LAMBDA_SUPPORT
            insert_at_ensure_functor<Func> wrapper( func );
#       endif

            position pos;
            while ( true )
            {
                bool bFound = find_position( val, pos, key_comparator(), true, true );
                if ( bFound ) {
                    // scoped_node_ptr deletes the node tower if we create it before
                    if ( !bTowerMade )
                        scp.release();

                    cds::unref(func)( false, *node_traits::to_value_ptr(pos.pCur), val );
                    m_Stat.onEnsureExist();
                    return std::make_pair( true, false );
                }

                if ( !bTowerOk ) {
                    build_node( pNode );
                    nHeight = pNode->height();
                    bTowerMade =
                        bTowerOk = true;
                }

#       ifdef CDS_CXX11_LAMBDA_SUPPORT
                if ( !insert_at_position( val, pNode, pos, [&func]( value_type& item ) { cds::unref(func)( true, item, item ); }))
#       else
                if ( !insert_at_position( val, pNode, pos, cds::ref(wrapper) ))
#       endif
                {
                    m_Stat.onInsertRetry();
                    continue;
                }

                increase_height( nHeight );
                ++m_ItemCounter;
                scp.release();
                m_Stat.onAddNode( nHeight );
                m_Stat.onEnsureNew();
                return std::make_pair( true, true );
            }
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_SkipListSet_nogc_find_func
            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You can pass \p f argument by value or by reference using <tt>boost::ref</tt> or cds::ref.

            The functor can change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& val, Func f ) const
        {
            return find_with_( val, key_comparator(), f ) != nullptr;
        }

        /// Finds the key \p val using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_nogc_find_func "find(Q&, Func)"
            but \p pred predicate is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& val, Less pred, Func f ) const
        {
            return find_with_( val, cds::opt::details::make_comparator_from_less<Less>(), f ) != nullptr;
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_SkipListSet_nogc_find_cfunc
            The function searches the item with key equal to \p val and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q const& val );
            };
            \endcode
            where \p item is the item found, \p val is the <tt>find</tt> function argument.

            You can pass \p f argument by value or by reference using <tt>boost::ref</tt> or cds::ref.

            The functor can change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& val, Func f ) const
        {
            return find_with_( val, key_comparator(), f ) != nullptr;
        }

        /// Finds the key \p val using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_nogc_find_cfunc "find(Q const&, Func)"
            but \p pred predicate is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& val, Less pred, Func f ) const
        {
            return find_with_( val, cds::opt::details::make_comparator_from_less<Less>(), f ) != nullptr;
        }

        /// Finds the key \p val
        /** \anchor cds_intrusive_SkipListSet_nogc_find_val
            The function searches the item with key equal to \p val
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        value_type * find( Q const& val ) const
        {
            node_type * pNode =
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
                find_with_( val, key_comparator(), [](value_type& , Q const& ) {} );
#       else
                find_with_( val, key_comparator(), empty_find_functor() );
#       endif
            if ( pNode )
                return node_traits::to_value_ptr( pNode );
            return nullptr;
        }

        /// Finds the key \p val using \p pred predicate for comparing
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_nogc_find_val "find(Q const&)"
            but \p pred predicate is used for key compare.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        value_type * find_with( Q const& val, Less pred ) const
        {
            node_type * pNode =
#       ifdef CDS_CXX11_LAMBDA_SUPPORT
                find_with_( val, cds::opt::details::make_comparator_from_less<Less>(), [](value_type& , Q const& ) {} );
#       else
                find_with_( val, cds::opt::details::make_comparator_from_less<Less>(), empty_find_functor() );
#       endif
            if ( pNode )
                return node_traits::to_value_ptr( pNode );
            return nullptr;
        }

        /// Gets minimum key from the set
        /**
            If the set is empty the function returns \p NULL
        */
        value_type * get_min() const
        {
            return node_traits::to_value_ptr( m_Head.head()->next( 0 ));
        }

        /// Gets maximum key from the set
        /**
            The function returns \p NULL if the set is empty
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
            If it is atomicity::empty_item_counter this function always returns 0.
            The function is not suitable for checking the set emptiness, use \ref empty
            member function for this purpose.
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
        static CDS_CONSTEXPR unsigned int max_height() CDS_NOEXCEPT
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


#endif // #ifndef __CDS_INTRUSIVE_SKIP_LIST_IMPL_H
