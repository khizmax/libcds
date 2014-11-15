//$$CDS-header$$

/*
    File: hzp_gc.cpp

    Hazard Pointers memory reclamation strategy implementation

    Editions:
        2008.02.10    Maxim.Khiszinsky    Created
*/

#include <cds/gc/hp/hp.h>

#include <algorithm>    // std::sort
#include "hp_const.h"

#define    CDS_HAZARDPTR_STATISTIC( _x )    if ( m_bStatEnabled ) { _x; }

namespace cds { namespace gc {
    namespace hp {

        /// Max array size of retired pointers
        static const size_t c_nMaxRetireNodeCount = c_nHazardPointerPerThread * c_nMaxThreadCount * 2;

        GarbageCollector *    GarbageCollector::m_pHZPManager = nullptr;

        void CDS_STDCALL GarbageCollector::Construct( size_t nHazardPtrCount, size_t nMaxThreadCount, size_t nMaxRetiredPtrCount, scan_type nScanType )
        {
            if ( !m_pHZPManager ) {
                m_pHZPManager = new GarbageCollector( nHazardPtrCount, nMaxThreadCount, nMaxRetiredPtrCount, nScanType );
            }
        }

        void CDS_STDCALL GarbageCollector::Destruct( bool bDetachAll )
        {
            if ( m_pHZPManager ) {
                if ( bDetachAll )
                    m_pHZPManager->detachAllThread();

                delete m_pHZPManager;
                m_pHZPManager = nullptr;
            }
        }

        GarbageCollector::GarbageCollector(
            size_t nHazardPtrCount,
            size_t nMaxThreadCount,
            size_t nMaxRetiredPtrCount,
            scan_type nScanType
        )
            : m_pListHead( nullptr )
            ,m_bStatEnabled( true )
            ,m_nHazardPointerCount( nHazardPtrCount == 0 ? c_nHazardPointerPerThread : nHazardPtrCount )
            ,m_nMaxThreadCount( nMaxThreadCount == 0 ? c_nMaxThreadCount : nMaxThreadCount )
            ,m_nMaxRetiredPtrCount( nMaxRetiredPtrCount > c_nMaxRetireNodeCount ? nMaxRetiredPtrCount : c_nMaxRetireNodeCount )
            ,m_nScanType( nScanType )
        {}

        GarbageCollector::~GarbageCollector()
        {
            CDS_DEBUG_ONLY( const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId; )
            CDS_DEBUG_ONLY( const cds::OS::ThreadId mainThreadId = cds::OS::getCurrentThreadId() ;)

            hplist_node * pHead = m_pListHead.load( atomics::memory_order_relaxed );
            m_pListHead.store( nullptr, atomics::memory_order_relaxed );

            hplist_node * pNext = nullptr;
            for ( hplist_node * hprec = pHead; hprec; hprec = pNext ) {
                assert( hprec->m_idOwner.load( atomics::memory_order_relaxed ) == nullThreadId
                    || hprec->m_idOwner.load( atomics::memory_order_relaxed ) == mainThreadId
                    || !cds::OS::isThreadAlive( hprec->m_idOwner.load( atomics::memory_order_relaxed ) )
                );
                details::retired_vector& vect = hprec->m_arrRetired;
                details::retired_vector::iterator itRetired = vect.begin();
                details::retired_vector::iterator itRetiredEnd = vect.end();
                while ( itRetired != itRetiredEnd ) {
                    DeletePtr( *itRetired );
                    ++itRetired;
                }
                vect.clear();
                pNext = hprec->m_pNextNode;
                hprec->m_bFree.store( true, atomics::memory_order_relaxed );
                DeleteHPRec( hprec );
            }
        }

        inline GarbageCollector::hplist_node * GarbageCollector::NewHPRec()
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_AllocNewHPRec );
            return new hplist_node( *this );
        }

        inline void GarbageCollector::DeleteHPRec( hplist_node * pNode )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_DeleteHPRec );
            assert( pNode->m_arrRetired.size() == 0 );
            delete pNode;
        }

        inline void GarbageCollector::DeletePtr( details::retired_ptr& p )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_DeletedNode );
            p.free();
        }

        details::hp_record * GarbageCollector::AllocateHPRec()
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_AllocHPRec );

            hplist_node * hprec;
            const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
            const cds::OS::ThreadId curThreadId  = cds::OS::getCurrentThreadId();

            // First try to reuse a retired (non-active) HP record
            for ( hprec = m_pListHead.load( atomics::memory_order_acquire ); hprec; hprec = hprec->m_pNextNode ) {
                cds::OS::ThreadId thId = nullThreadId;
                if ( !hprec->m_idOwner.compare_exchange_strong( thId, curThreadId, atomics::memory_order_seq_cst, atomics::memory_order_relaxed ) )
                    continue;
                hprec->m_bFree.store( false, atomics::memory_order_release );
                return hprec;
            }

            // No HP records available for reuse
            // Allocate and push a new HP record
            hprec = NewHPRec();
            hprec->m_idOwner.store( curThreadId, atomics::memory_order_relaxed );
            hprec->m_bFree.store( false, atomics::memory_order_relaxed );

            atomics::atomic_thread_fence( atomics::memory_order_release );

            hplist_node * pOldHead = m_pListHead.load( atomics::memory_order_acquire );
            do {
                hprec->m_pNextNode = pOldHead;
            } while ( !m_pListHead.compare_exchange_weak( pOldHead, hprec, atomics::memory_order_release, atomics::memory_order_relaxed ));

            return hprec;
        }

        void GarbageCollector::RetireHPRec( details::hp_record * pRec )
        {
            assert( pRec != nullptr );
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_RetireHPRec );

            pRec->clear();
            Scan( pRec );
            hplist_node * pNode = static_cast<hplist_node *>( pRec );
            pNode->m_idOwner.store( cds::OS::c_NullThreadId, atomics::memory_order_release );
        }

        void GarbageCollector::detachAllThread()
        {
            hplist_node * pNext = nullptr;
            const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
            for ( hplist_node * hprec = m_pListHead.load(atomics::memory_order_acquire); hprec; hprec = pNext ) {
                pNext = hprec->m_pNextNode;
                if ( hprec->m_idOwner.load(atomics::memory_order_relaxed) != nullThreadId ) {
                    RetireHPRec( hprec );
                }
            }
        }

        void GarbageCollector::classic_scan( details::hp_record * pRec )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_ScanCallCount );

            std::vector< void * >   plist;
            plist.reserve( m_nMaxThreadCount * m_nHazardPointerCount );
            assert( plist.size() == 0 );

            // Stage 1: Scan HP list and insert non-null values in plist

            hplist_node * pNode = m_pListHead.load(atomics::memory_order_acquire);

            while ( pNode ) {
                for ( size_t i = 0; i < m_nHazardPointerCount; ++i ) {
                    void * hptr = pNode->m_hzp[i];
                    if ( hptr )
                        plist.push_back( hptr );
                }
                pNode = pNode->m_pNextNode;
            }

            // Sort plist to simplify search in
            std::sort( plist.begin(), plist.end() );

            // Stage 2: Search plist
            details::retired_vector& arrRetired = pRec->m_arrRetired;

            details::retired_vector::iterator itRetired     = arrRetired.begin();
            details::retired_vector::iterator itRetiredEnd  = arrRetired.end();
            // arrRetired is not a std::vector!
            // clear is just set up item counter to 0, the items is not destroying
            arrRetired.clear();

            std::vector< void * >::iterator itBegin = plist.begin();
            std::vector< void * >::iterator itEnd = plist.end();
            while ( itRetired != itRetiredEnd ) {
                if ( std::binary_search( itBegin, itEnd, itRetired->m_p) ) {
                    CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_DeferredNode );
                    arrRetired.push( *itRetired );
                }
                else
                    DeletePtr( *itRetired );
                ++itRetired;
            }
        }

        void GarbageCollector::inplace_scan( details::hp_record * pRec )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_ScanCallCount );

            // In-place scan algo uses LSB of retired ptr as a mark for internal purposes.
            // It is correct if all retired pointers are ar least 2-byte aligned (LSB is zero).
            // If it is wrong, we use classic scan algorithm

            // Check if all retired pointers has zero LSB
            // LSB is used for marking pointers that cannot be deleted yet
            details::retired_vector::iterator itRetired     = pRec->m_arrRetired.begin();
            details::retired_vector::iterator itRetiredEnd  = pRec->m_arrRetired.end();
            for ( details::retired_vector::iterator it = itRetired; it != itRetiredEnd; ++it ) {
                if ( reinterpret_cast<ptr_atomic_t>(it->m_p) & 1 ) {
                    // found a pointer with LSB bit set - use classic_scan
                    classic_scan( pRec );
                    return;
                }
            }

            // Sort retired pointer array
            std::sort( itRetired, itRetiredEnd, cds::gc::details::retired_ptr::less );

            // Search guarded pointers in retired array

            hplist_node * pNode = m_pListHead.load(atomics::memory_order_acquire);

            while ( pNode ) {
                for ( size_t i = 0; i < m_nHazardPointerCount; ++i ) {
                    void * hptr = pNode->m_hzp[i];
                    if ( hptr ) {
                        details::retired_ptr    dummyRetired;
                        dummyRetired.m_p = hptr;
                        details::retired_vector::iterator it = std::lower_bound( itRetired, itRetiredEnd, dummyRetired, cds::gc::details::retired_ptr::less );
                        if ( it != itRetiredEnd && it->m_p == hptr )  {
                            // Mark retired pointer as guarded
                            it->m_p = reinterpret_cast<void *>(reinterpret_cast<ptr_atomic_t>(it->m_p ) | 1);
                        }
                    }
                }
                pNode = pNode->m_pNextNode;
            }

            // Move all marked pointers to head of array
            details::retired_vector::iterator itInsert = itRetired;
            for ( details::retired_vector::iterator it = itRetired; it != itRetiredEnd; ++it ) {
                if ( reinterpret_cast<ptr_atomic_t>(it->m_p) & 1 ) {
                    it->m_p = reinterpret_cast<void *>(reinterpret_cast<ptr_atomic_t>(it->m_p ) & ~1);
                    *itInsert = *it;
                    ++itInsert;
                    CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_DeferredNode );
                }
                else {
                    // Retired pointer may be freed
                    DeletePtr( *it );
                }
            }
            pRec->m_arrRetired.size( itInsert - itRetired );
        }

        void GarbageCollector::HelpScan( details::hp_record * pThis )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_HelpScanCallCount );

            assert( static_cast<hplist_node *>(pThis)->m_idOwner.load(atomics::memory_order_relaxed) == cds::OS::getCurrentThreadId() );

            const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
            const cds::OS::ThreadId curThreadId = cds::OS::getCurrentThreadId();
            for ( hplist_node * hprec = m_pListHead.load(atomics::memory_order_acquire); hprec; hprec = hprec->m_pNextNode ) {

                // If m_bFree == true then hprec->m_arrRetired is empty - we don't need to see it
                if ( hprec->m_bFree.load(atomics::memory_order_acquire) )
                    continue;

                // Owns hprec if it is empty.
                // Several threads may work concurrently so we use atomic technique only.
                {
                    cds::OS::ThreadId curOwner = hprec->m_idOwner.load(atomics::memory_order_acquire);
                    if ( curOwner == nullThreadId || !cds::OS::isThreadAlive( curOwner )) {
                        if ( !hprec->m_idOwner.compare_exchange_strong( curOwner, curThreadId, atomics::memory_order_release, atomics::memory_order_relaxed ))
                            continue;
                    }
                    else {
                        curOwner = nullThreadId;
                        if ( !hprec->m_idOwner.compare_exchange_strong( curOwner, curThreadId, atomics::memory_order_release, atomics::memory_order_relaxed ))
                            continue;
                    }
                }

                // We own the thread successfully. Now, we can see whether hp_record has retired pointers.
                // If it has ones then we move to pThis that is private for current thread.
                details::retired_vector& src = hprec->m_arrRetired;
                details::retired_vector& dest = pThis->m_arrRetired;
                assert( !dest.isFull());
                details::retired_vector::iterator itRetired = src.begin();
                details::retired_vector::iterator itRetiredEnd = src.end();
                while ( itRetired != itRetiredEnd ) {
                    dest.push( *itRetired );
                    if ( dest.isFull()) {
                        CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_CallScanFromHelpScan );
                        Scan( pThis );
                    }
                    ++itRetired;
                }
                src.clear();

                hprec->m_bFree.store(true, atomics::memory_order_release);
                hprec->m_idOwner.store( nullThreadId, atomics::memory_order_release );
            }
        }

        GarbageCollector::InternalState& GarbageCollector::getInternalState( GarbageCollector::InternalState& stat) const
        {
            stat.nHPCount                = m_nHazardPointerCount;
            stat.nMaxThreadCount         = m_nMaxThreadCount;
            stat.nMaxRetiredPtrCount     = m_nMaxRetiredPtrCount;
            stat.nHPRecSize              = sizeof( hplist_node )
                                            + sizeof(details::retired_ptr) * m_nMaxRetiredPtrCount;

            stat.nHPRecAllocated         =
                stat.nHPRecUsed              =
                stat.nTotalRetiredPtrCount   =
                stat.nRetiredPtrInFreeHPRecs = 0;

            for ( hplist_node * hprec = m_pListHead.load(atomics::memory_order_acquire); hprec; hprec = hprec->m_pNextNode ) {
                ++stat.nHPRecAllocated;
                stat.nTotalRetiredPtrCount += hprec->m_arrRetired.size();

                if ( hprec->m_bFree.load(atomics::memory_order_relaxed) ) {
                    // Free HP record
                    stat.nRetiredPtrInFreeHPRecs += hprec->m_arrRetired.size();
                }
                else {
                    // Used HP record
                    ++stat.nHPRecUsed;
                }
            }

            // Events
            stat.evcAllocHPRec   = m_Stat.m_AllocHPRec;
            stat.evcRetireHPRec  = m_Stat.m_RetireHPRec;
            stat.evcAllocNewHPRec= m_Stat.m_AllocNewHPRec;
            stat.evcDeleteHPRec  = m_Stat.m_DeleteHPRec;

            stat.evcScanCall     = m_Stat.m_ScanCallCount;
            stat.evcHelpScanCall = m_Stat.m_HelpScanCallCount;
            stat.evcScanFromHelpScan= m_Stat.m_CallScanFromHelpScan;

            stat.evcDeletedNode  = m_Stat.m_DeletedNode;
            stat.evcDeferredNode = m_Stat.m_DeferredNode;

            return stat;
        }


    } //namespace hp
}} // namespace cds::gc
