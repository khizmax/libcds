#ifndef CDSLIB_INTRUSIVE_IMPL_LAZY_SKIP_LIST_H
#define CDSLIB_INTRUSIVE_IMPL_LAZY_SKIP_LIST_H

#include <type_traits>
#include <memory>
#include <functional>   // ref
#include <cds/intrusive/details/lazy_skip_list_base.h>
#include <cds/opt/compare.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace intrusive {

    namespace lazy_skip_list { namespace details {

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
    }}  // namespace lazy_skip_list::details

    template <
        class GC
       ,typename T
#ifdef CDS_DOXYGEN_INVOKED
       ,typename Traits = lazy_skip_list::traits
#else
       ,typename Traits
#endif
    >
    class LazySkipListSet
    {
    public:
        typedef GC      gc;
        typedef T       value_type;
        typedef Traits  traits;

        typedef typename traits::hook    hook;
        typedef typename hook::node_type node_type;

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator;
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif

        typedef typename traits::disposer  disposer;
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;

        typedef typename traits::item_counter  item_counter;
        typedef typename traits::memory_model  memory_model;
        typedef typename traits::random_level_generator random_level_generator;
        typedef typename traits::allocator     allocator_type;
        typedef typename traits::back_off      back_off;
        typedef typename traits::stat          stat;

    public:
        typedef typename gc::template guarded_ptr< value_type > guarded_ptr;

        static unsigned int const c_nMaxHeight = std::conditional<
            (random_level_generator::c_nUpperBound <= lazy_skip_list::c_nHeightLimit),
            std::integral_constant< unsigned int, random_level_generator::c_nUpperBound >,
            std::integral_constant< unsigned int, lazy_skip_list::c_nHeightLimit >
        >::type::value;

        static unsigned int const c_nMinHeight = 5;

        // c_nMaxHeight * 2 - pPred/pSucc guards
        // + 1 - for erase, unlink
        // + 1 - for clear
        // + 1 - for help_remove()
        static size_t const c_nHazardPtrCount = c_nMaxHeight * 2 + 3;

    protected:
        typedef typename node_type::atomic_marked_ptr   atomic_node_ptr;
        typedef typename node_type::marked_ptr          marked_node_ptr;

    protected:
        typedef lazy_skip_list::details::intrusive_node_builder< node_type, atomic_node_ptr, allocator_type > intrusive_node_builder;

        typedef typename std::conditional<
            std::is_same< typename traits::internal_node_builder, cds::opt::none >::value
            ,intrusive_node_builder
            ,typename traits::internal_node_builder
        >::type node_builder;

        typedef std::unique_ptr< node_type, typename node_builder::node_disposer > scoped_node_ptr;

        struct position {
            node_type *   pPrev[ c_nMaxHeight ];
            node_type *   pSucc[ c_nMaxHeight ];

            typename gc::template GuardArray< c_nMaxHeight * 2 > guards;
            node_type *   pCur;   // guarded by one of guards
        };

    public:
        LazySkipListSet()
            : m_Head( c_nMaxHeight )
            , m_nHeight( c_nMinHeight )
        {
            static_assert( (std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type" );

            gc::check_available_guards( c_nHazardPtrCount );

            // Barrier for head node
            atomics::atomic_thread_fence( memory_model::memory_order_release );
        }

        ~LazySkipListSet()
        {
            destroy();
        }

    public:

        typedef lazy_skip_list::details::iterator< gc, node_traits, back_off, false >  iterator;

        typedef lazy_skip_list::details::iterator< gc, node_traits, back_off, true >   const_iterator;

        iterator begin()
        {
            return iterator( *m_Head.head());
        }

        const_iterator begin() const
        {
            return const_iterator( *m_Head.head());
        }
        const_iterator cbegin() const
        {
            return const_iterator( *m_Head.head());
        }

        iterator end()
        {
            return iterator();
        }

        const_iterator end() const
        {
            return const_iterator();
        }
        const_iterator cend() const
        {
            return const_iterator();
        }

    public:
        
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
        }

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
        
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }

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

        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return extract_( key, key_comparator());
        }

        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return extract_( key, cds::opt::details::make_comparator_from_less<Less>());
        }

        guarded_ptr extract_min()
        {
            return extract_min_();
        }

        guarded_ptr extract_max()
        {
            return extract_max_();
        }

        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_( key, key_comparator(), [](value_type const&) {} );
        }

        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_( key, cds::opt::details::make_comparator_from_less<Less>(), [](value_type const&) {} );
        }

        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_( key, key_comparator(), f );
        }

        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_with_( key, key_comparator(), f );
        }
        
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_with_( key, key_comparator(), f );
        }

        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        template <typename Q>
        bool contains( Q const& key )
        {
            return find_with_( key, key_comparator(), [](value_type& , Q const& ) {} );
        }
        
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }

        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_with_( key, cds::opt::details::make_comparator_from_less<Less>(), [](value_type& , Q const& ) {} );
        }
        
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }

        template <typename Q>
        guarded_ptr get( Q const& key )
        {
            return get_with_( key, key_comparator());
        }

        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return get_with_( key, cds::opt::details::make_comparator_from_less<Less>());
        }

        size_t size() const
        {
            return m_ItemCounter;
        }

        bool empty() const
        {
            return m_Head.head()->next( 0 ).load( memory_model::memory_order_relaxed ) == nullptr;
        }

        void clear()
        {
            while ( extract_min_());
        }

        static constexpr unsigned int max_height() noexcept
        {
            return c_nMaxHeight;
        }

        stat const& statistics() const
        {
            return m_Stat;
        }

    protected:
        
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


    private:
        
        lazy_skip_list::details::head_node< node_type > m_Head;

        random_level_generator      m_RandomLevelGen;
        atomics::atomic<unsigned int> m_nHeight;
        item_counter                m_ItemCounter;
        mutable stat                m_Stat;
    };

}} // namespace cds::intrusive


#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_LAZY_SKIP_LIST_H
