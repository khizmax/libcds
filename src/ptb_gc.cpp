//$$CDS-header$$

// Pass The Buck (PTB) Memory manager implementation

#include <cds/gc/ptb/ptb.h>
#include <cds/int_algo.h>

#include <cds/details/hash_functor_selector.h>
#include <algorithm>   // std::fill

namespace cds { namespace gc { namespace ptb {

    namespace details {

        class liberate_set {
            typedef retired_ptr_node *  item_type;
            typedef cds::details::Allocator<item_type, CDS_DEFAULT_ALLOCATOR>   allocator_type;

            size_t const m_nBucketCount;
            item_type *  m_Buckets;

            item_type&  bucket( retired_ptr_node& node )
            {
                return bucket( node.m_ptr.m_p );
            }
            item_type&  bucket( guard_data::guarded_ptr p )
            {
                return m_Buckets[ cds::details::hash<guard_data::guarded_ptr>()( p ) & (m_nBucketCount - 1)  ];
            }

        public:
            liberate_set( size_t nBucketCount )
                : m_nBucketCount( nBucketCount )
            {
                assert( nBucketCount > 0 );
                assert( (nBucketCount & (nBucketCount - 1)) == 0 );

                m_Buckets = allocator_type().NewArray( nBucketCount );
                std::fill( m_Buckets, m_Buckets + nBucketCount, null_ptr<item_type>());
            }

            ~liberate_set()
            {
                allocator_type().Delete( m_Buckets, m_nBucketCount );
            }

            void insert( retired_ptr_node& node )
            {
                node.m_pNext = null_ptr<item_type>();

                item_type& refBucket = bucket( node );
                if ( refBucket ) {
                    item_type p = refBucket;
                    do {
                        if ( p->m_ptr.m_p == node.m_ptr.m_p ) {
                            assert( node.m_pNextFree == null_ptr<details::retired_ptr_node *>() );

                            node.m_pNextFree = p->m_pNextFree;
                            p->m_pNextFree = &node;
                            return;
                        }
                        p = p->m_pNext;
                    } while ( p );

                    node.m_pNext = refBucket;
                }
                refBucket = &node;
            }

            item_type erase( guard_data::guarded_ptr ptr )
            {
                item_type& refBucket = bucket( ptr );
                item_type p = refBucket;
                item_type pPrev = null_ptr<item_type>();

                while ( p ) {
                    if ( p->m_ptr.m_p == ptr ) {
                        if ( pPrev )
                            pPrev->m_pNext = p->m_pNext;
                        else
                            refBucket = p->m_pNext;
                        p->m_pNext = null_ptr<item_type>();
                        return p;
                    }
                    pPrev = p;
                    p = p->m_pNext;
                }

                return null_ptr<item_type>();
            }

            typedef std::pair<item_type, item_type>     list_range;

            list_range free_all()
            {
                item_type pTail = null_ptr<item_type>();
                list_range ret = std::make_pair( pTail, pTail );

                item_type const * pEndBucket = m_Buckets + m_nBucketCount;
                for ( item_type * ppBucket = m_Buckets; ppBucket < pEndBucket; ++ppBucket ) {
                    item_type pBucket = *ppBucket;
                    if ( pBucket ) {
                        if ( !ret.first )
                            ret.first = pBucket;
                        else
                            pTail->m_pNextFree = pBucket;

                        pTail = pBucket;
                        for (;;) {
                            item_type pNext = pTail->m_pNext;
                            pTail->m_ptr.free();
                            pTail->m_pNext = null_ptr<item_type>();

                            while ( pTail->m_pNextFree ) {
                                pTail = pTail->m_pNextFree;
                                pTail->m_ptr.free();
                                pTail->m_pNext = null_ptr<item_type>();
                            }

                            if ( pNext )
                                pTail = pTail->m_pNextFree = pNext;
                            else
                                break;
                        }
                    }
                }

                if ( pTail )
                    pTail->m_pNextFree = null_ptr<item_type>();
                ret.second = pTail;
                return ret;
            }
        };
    }

    GarbageCollector * GarbageCollector::m_pManager = NULL;

    void CDS_STDCALL GarbageCollector::Construct(
        size_t nLiberateThreshold
        , size_t nInitialThreadGuardCount
    )
    {
        if ( !m_pManager ) {
            m_pManager = new GarbageCollector( nLiberateThreshold, nInitialThreadGuardCount );
        }
    }

    void CDS_STDCALL GarbageCollector::Destruct()
    {
        if ( m_pManager ) {
            delete m_pManager;
            m_pManager = NULL;
        }
    }

    GarbageCollector::GarbageCollector( size_t nLiberateThreshold, size_t nInitialThreadGuardCount )
        : m_nLiberateThreshold( nLiberateThreshold ? nLiberateThreshold : 1024 )
        , m_nInitialThreadGuardCount( nInitialThreadGuardCount ? nInitialThreadGuardCount : 8 )
        //, m_nInLiberate(0)
    {
    }

    GarbageCollector::~GarbageCollector()
    {
        liberate();

#if 0
        details::retired_ptr_node * pHead = null_ptr<details::retired_ptr_node *>();
        details::retired_ptr_node * pTail = null_ptr<details::retired_ptr_node *>();

        for ( details::guard_data * pGuard = m_GuardPool.begin(); pGuard; pGuard = pGuard->pGlobalNext.load(CDS_ATOMIC::memory_order_relaxed)) {
            details::guard_data::handoff_ptr h = pGuard->pHandOff;
            pGuard->pHandOff  = null_ptr<details::guard_data::handoff_ptr>();
            while ( h ) {
                details::guard_data::handoff_ptr pNext = h->m_pNextFree;
                if ( h->m_ptr.m_p )
                    h->m_ptr.free();
                if ( !pHead )
                    pTail = pHead = h;
                else
                    pTail = pTail->m_pNextFree = h;
                h = pNext;
            }
        }
        if ( pHead )
            m_RetiredAllocator.free_range( pHead, pTail );
#endif
    }

    void GarbageCollector::liberate()
    {
        details::retired_ptr_buffer::privatize_result retiredList = m_RetiredBuffer.privatize();
        if ( retiredList.first ) {

            size_t nLiberateThreshold = m_nLiberateThreshold.load(CDS_ATOMIC::memory_order_relaxed);
            details::liberate_set set( beans::ceil2( retiredList.second > nLiberateThreshold ? retiredList.second : nLiberateThreshold ) );

            // Get list of retired pointers
            details::retired_ptr_node * pHead = retiredList.first;
            while ( pHead ) {
                details::retired_ptr_node * pNext = pHead->m_pNext;
                pHead->m_pNextFree = null_ptr<details::retired_ptr_node *>();
                set.insert( *pHead );
                pHead = pNext;
            }

            // Liberate cycle
            for ( details::guard_data * pGuard = m_GuardPool.begin(); pGuard; pGuard = pGuard->pGlobalNext.load(CDS_ATOMIC::memory_order_acquire) )
            {
                // get guarded pointer
                details::guard_data::guarded_ptr  valGuarded = pGuard->pPost.load(CDS_ATOMIC::memory_order_acquire);

                if ( valGuarded ) {
                    details::retired_ptr_node * pRetired = set.erase( valGuarded );
                    if ( pRetired ) {
                        // Retired pointer is being guarded
                        // pRetired is the head of retired pointers list for which the m_ptr.m_p field is equal
                        // List is linked on m_pNextFree field

                        do {
                            details::retired_ptr_node * pNext = pRetired->m_pNextFree;
                            m_RetiredBuffer.push( *pRetired );
                            pRetired = pNext;
                        } while ( pRetired );
                    }
                }
            }

            // Free all retired pointers
            details::liberate_set::list_range range = set.free_all();

            m_RetiredAllocator.inc_epoch();

            if ( range.first ) {
                assert( range.second != null_ptr<details::retired_ptr_node *>() );
                m_RetiredAllocator.free_range( range.first, range.second );
            }
            else {
                // liberate cycle did not free any retired pointer - double liberate threshold
                m_nLiberateThreshold.compare_exchange_strong( nLiberateThreshold, nLiberateThreshold * 2, CDS_ATOMIC::memory_order_release, CDS_ATOMIC::memory_order_relaxed );
            }
        }
    }

#if 0
    void GarbageCollector::liberate( details::liberate_set& set )
    {
        details::guard_data::handoff_ptr const nullHandOff = null_ptr<details::guard_data::handoff_ptr>();

        for ( details::guard_data * pGuard = m_GuardPool.begin(); pGuard; pGuard = pGuard->pGlobalNext.load(CDS_ATOMIC::memory_order_acquire) )
        {
            // get guarded pointer
            details::guard_data::guarded_ptr  valGuarded = pGuard->pPost.load(CDS_ATOMIC::memory_order_acquire);
            details::guard_data::handoff_ptr h;

            if ( valGuarded ) {
                details::retired_ptr_node * pRetired = set.erase( valGuarded );
                if ( pRetired ) {
                    // Retired pointer is being guarded

                    // pRetired is the head of retired pointers list for which the m_ptr.m_p field is equal
                    // List is linked on m_pNextFree field

                    // Now, try to set retired node pRetired as a hand-off node for the guard
                    cds::lock::Auto<details::guard_data::handoff_spin> al( pGuard->spinHandOff );
                    if ( valGuarded == pGuard->pPost.load(CDS_ATOMIC::memory_order_acquire) ) {
                        if ( pGuard->pHandOff && pGuard->pHandOff->m_ptr.m_p == pRetired->m_ptr.m_p ) {
                            h = nullHandOff ; //null_ptr<details::guard_data::handoff_ptr>();
                            details::retired_ptr_node * pTail = pGuard->pHandOff;
                            while ( pTail->m_pNextFree )
                                pTail = pTail->m_pNextFree;
                            pTail->m_pNextFree = pRetired;
                        }
                        else {
                            // swap h and pGuard->pHandOff
                            h = pGuard->pHandOff;
                            pGuard->pHandOff = pRetired;
                        }
                    }
                    else
                        h = pRetired;
                }
                else {
                    cds::lock::Auto<details::guard_data::handoff_spin> al( pGuard->spinHandOff );
                    h = pGuard->pHandOff;
                    if ( h ) {
                        if ( h->m_ptr.m_p != valGuarded )
                            pGuard->pHandOff = nullHandOff;
                        else
                            h = nullHandOff;
                    }
                }
            }
            else {
                cds::lock::Auto<details::guard_data::handoff_spin> al( pGuard->spinHandOff );
                h = pGuard->pHandOff;
                pGuard->pHandOff = nullHandOff;
            }

            // h is the head of a list linked on m_pNextFree field
            if ( h ) {
                set.insert( *h );
            }
        }
    }
#endif
}}} // namespace cds::gc::ptb
