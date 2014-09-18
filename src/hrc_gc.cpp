//$$CDS-header$$

/*
    File: hrc_gc.cpp

    Implementation of cds::gc::hrc::HRCGarbageCollector

    Editions:
        2008.03.10    Maxim.Khiszinsky    Created
*/

#include <cds/gc/hrc/hrc.h>

#include "hzp_const.h"
#include <vector>
#include <algorithm>    // std::sort

#define    CDS_HRC_STATISTIC( _x )    if ( m_bStatEnabled ) { _x; }

namespace cds { namespace gc {
    namespace hrc {

        GarbageCollector * GarbageCollector::m_pGC = null_ptr<GarbageCollector *>();

        GarbageCollector::GarbageCollector(
            size_t nHazardPtrCount,
            size_t nMaxThreadCount,
            size_t nRetiredNodeArraySize
            )
            : m_pListHead( null_ptr<thread_list_node *>()),
            m_bStatEnabled( true ),
            m_nHazardPointerCount( nHazardPtrCount ),
            m_nMaxThreadCount( nMaxThreadCount ),
            m_nMaxRetiredPtrCount( nRetiredNodeArraySize )
        {}

        GarbageCollector::~GarbageCollector()
        {
            thread_list_node * pNode = m_pListHead.load( CDS_ATOMIC::memory_order_relaxed );
            while ( pNode ) {
                assert( pNode->m_idOwner.load(CDS_ATOMIC::memory_order_relaxed) == cds::OS::nullThreadId() );
                clearHRCThreadDesc( pNode );
                thread_list_node * pNext = pNode->m_pNext;
                deleteHRCThreadDesc( pNode );
                pNode = pNext;
            }
        }

        void CDS_STDCALL GarbageCollector::Construct(
            size_t nHazardPtrCount,        // hazard pointers count
            size_t nMaxThreadCount,        // max thread count
            size_t nMaxNodeLinkCount,    // max HRC-pointer count in the HRC-container's item
            size_t nMaxTransientLinks    // max HRC-container's item count that can point to deleting item of container
            )
        {
            if ( !m_pGC ) {
                if ( nHazardPtrCount == 0 )
                    nHazardPtrCount = c_nHazardPointerPerThread    + c_nCleanUpHazardPointerPerThread;
                if ( nMaxThreadCount == 0 )
                    nMaxThreadCount = c_nMaxThreadCount;
                if ( nMaxNodeLinkCount == 0 )
                    nMaxNodeLinkCount = c_nHRCMaxNodeLinkCount;
                if ( nMaxTransientLinks == 0 )
                    nMaxTransientLinks = c_nHRCMaxTransientLinks;

                size_t nRetiredNodeArraySize = nMaxThreadCount * ( nHazardPtrCount + nMaxNodeLinkCount + nMaxTransientLinks + 1 );

                m_pGC = new GarbageCollector( nHazardPtrCount, nMaxThreadCount, nRetiredNodeArraySize );
            }
        }

        void CDS_STDCALL GarbageCollector::Destruct()
        {
            if ( m_pGC ) {
                {
                    ThreadGC tgc;
                    tgc.init();
                    m_pGC->HelpScan( &tgc );
                    m_pGC->Scan( &tgc );
                    // tgc dtor calls fini()
                }

                delete m_pGC;
                m_pGC = null_ptr<GarbageCollector *>();
            }
        }

        inline GarbageCollector::thread_list_node * GarbageCollector::newHRCThreadDesc()
        {
            CDS_HRC_STATISTIC( ++m_Stat.m_AllocNewHRCThreadDesc );
            return new thread_list_node( *this );
        }

        inline void GarbageCollector::deleteHRCThreadDesc( thread_list_node * pNode )
        {
            assert( pNode->m_hzp.size() == pNode->m_hzp.capacity() );
            CDS_HRC_STATISTIC( ++m_Stat.m_DeleteHRCThreadDesc );
            delete pNode;
        }

        void GarbageCollector::clearHRCThreadDesc( thread_list_node * pNode )
        {
            assert( pNode->m_hzp.size() == pNode->m_hzp.capacity() );
            ContainerNode * pItem;
            for ( size_t n = 0; n < pNode->m_arrRetired.capacity(); ++n ) {
                if ( (pItem = pNode->m_arrRetired[n].m_pNode.load(CDS_ATOMIC::memory_order_relaxed)) != null_ptr<ContainerNode *>() ) {
                    pNode->m_arrRetired[n].m_funcFree( pItem );
                    //pItem->destroy();
                    pNode->m_arrRetired[n].m_pNode.store( null_ptr<ContainerNode *>(), CDS_ATOMIC::memory_order_relaxed );
                }
            }
            assert( pNode->m_hzp.size() == pNode->m_hzp.capacity() );
        }

        GarbageCollector::thread_list_node *  GarbageCollector::getHRCThreadDescForCurrentThread() const
        {
            thread_list_node * hprec;
            const cds::OS::ThreadId curThreadId  = cds::OS::getCurrentThreadId();

            for ( hprec = m_pListHead.load( CDS_ATOMIC::memory_order_acquire ); hprec; hprec = hprec->m_pNext ) {
                if ( hprec->m_idOwner.load( CDS_ATOMIC::memory_order_acquire ) == curThreadId ) {
                    assert( !hprec->m_bFree );
                    return hprec;
                }
            }
            return null_ptr<GarbageCollector::thread_list_node *>();
        }

        details::thread_descriptor * GarbageCollector::allocateHRCThreadDesc( ThreadGC * pThreadGC )
        {
            CDS_HRC_STATISTIC( ++m_Stat.m_AllocHRCThreadDesc );

            thread_list_node * hprec;
            const cds::OS::ThreadId nullThreadId = cds::OS::nullThreadId();
            const cds::OS::ThreadId curThreadId  = cds::OS::getCurrentThreadId();

            // First try to reuse a retired (non-active) HP record
            for ( hprec = m_pListHead.load( CDS_ATOMIC::memory_order_acquire ); hprec; hprec = hprec->m_pNext ) {
                cds::OS::ThreadId expectedThreadId = nullThreadId;
                if ( !hprec->m_idOwner.compare_exchange_strong( expectedThreadId, curThreadId, CDS_ATOMIC::memory_order_acq_rel, CDS_ATOMIC::memory_order_relaxed ) )
                    continue;
                hprec->m_pOwner = pThreadGC;
                hprec->m_bFree = false;
                assert( hprec->m_hzp.size() == hprec->m_hzp.capacity() );
                return hprec;
            }

            // No HP records available for reuse
            // Allocate and push a new HP record
            hprec = newHRCThreadDesc();
            assert( hprec->m_hzp.size() == hprec->m_hzp.capacity() );
            hprec->m_idOwner.store( curThreadId, CDS_ATOMIC::memory_order_relaxed );
            hprec->m_pOwner = pThreadGC;
            hprec->m_bFree = false;
            thread_list_node * pOldHead;

            pOldHead = m_pListHead.load( CDS_ATOMIC::memory_order_relaxed );
            do {
                hprec->m_pNext = pOldHead;
            } while ( !m_pListHead.compare_exchange_weak( pOldHead, hprec, CDS_ATOMIC::memory_order_release, CDS_ATOMIC::memory_order_relaxed ));

            assert( hprec->m_hzp.size() == hprec->m_hzp.capacity() );
            return hprec;
        }

        void GarbageCollector::retireHRCThreadDesc( details::thread_descriptor * pRec )
        {
            CDS_HRC_STATISTIC( ++m_Stat.m_RetireHRCThreadDesc );

            pRec->clear();
            thread_list_node * pNode = static_cast<thread_list_node *>( pRec );
            assert( pNode->m_hzp.size() == pNode->m_hzp.capacity() );
            /*
                It is possible that
                    pNode->m_idOwner.value() != cds::OS::getCurrentThreadId()
                if the destruction of thread object is called by the destructor
                after thread termination
            */
            assert( pNode->m_idOwner.load(CDS_ATOMIC::memory_order_relaxed) != cds::OS::nullThreadId() );
            pNode->m_pOwner = null_ptr<ThreadGC *>();
            pNode->m_idOwner.store( cds::OS::nullThreadId(), CDS_ATOMIC::memory_order_release );
            assert( pNode->m_hzp.size() == pNode->m_hzp.capacity() );
        }

        void GarbageCollector::Scan( ThreadGC * pThreadGC )
        {
            CDS_HRC_STATISTIC( ++m_Stat.m_ScanCalls );

            typedef std::vector< ContainerNode * > hazard_ptr_list;

            details::thread_descriptor * pRec = pThreadGC->m_pDesc;
            assert( static_cast< thread_list_node *>( pRec )->m_idOwner.load(CDS_ATOMIC::memory_order_relaxed) == cds::OS::getCurrentThreadId() );

            // Step 1: mark all pRec->m_arrRetired items as "traced"
            {
                details::retired_vector::const_iterator itEnd = pRec->m_arrRetired.end();

                for ( details::retired_vector::const_iterator it = pRec->m_arrRetired.begin() ; it != itEnd; ++it ) {
                    ContainerNode * pNode = it->m_pNode.load( CDS_ATOMIC::memory_order_acquire );
                    if ( pNode ) {
                        if ( pNode->m_RC.value() == 0 ) {
                            pNode->m_bTrace.store( true, CDS_ATOMIC::memory_order_release );
                            if ( pNode->m_RC.value() != 0 )
                                pNode->m_bTrace.store( false, CDS_ATOMIC::memory_order_release );
                        }
                    }
                }
            }

            // Array of hazard pointers for all threads
            hazard_ptr_list   plist;
            plist.reserve( m_nMaxThreadCount * m_nHazardPointerCount );
            assert( plist.size() == 0 );

            // Stage 2: Scan HP list and insert non-null values to plist
            {
                thread_list_node * pNode = m_pListHead.load( CDS_ATOMIC::memory_order_acquire );

                while ( pNode ) {
                    for ( size_t i = 0; i < m_nHazardPointerCount; ++i ) {
                        ContainerNode * hptr = pNode->m_hzp[i];
                        if ( hptr )
                            plist.push_back( hptr );
                    }
                    pNode = pNode->m_pNext;
                }
            }

            // Sort plist to simplify search in
            std::sort( plist.begin(), plist.end() );

            // Stage 3: Deletes all nodes for refCount == 0 and that do not declare as Hazard in other thread
            {
                details::retired_vector& arr =  pRec->m_arrRetired;

                hazard_ptr_list::iterator itHPBegin = plist.begin();
                hazard_ptr_list::iterator itHPEnd = plist.end();

                details::retired_vector::iterator itEnd = arr.end();
                details::retired_vector::iterator it = arr.begin();

                for ( size_t nRetired = 0; it != itEnd; ++nRetired, ++it ) {
                    details::retired_node& node = *it;
                    ContainerNode * pNode = node.m_pNode.load(CDS_ATOMIC::memory_order_acquire);
                    if ( !pNode )
                        continue;

                    if ( pNode->m_RC.value() == 0 && pNode->m_bTrace.load(CDS_ATOMIC::memory_order_acquire) && !std::binary_search( itHPBegin, itHPEnd, pNode ) ) {
                        // pNode may be destructed safely

                        node.m_bDone.store( true, CDS_ATOMIC::memory_order_release );
                        if ( node.m_nClaim.load( CDS_ATOMIC::memory_order_acquire ) == 0 ) {
                            pNode->terminate( pThreadGC, false );
                            pNode->clean( CDS_ATOMIC::memory_order_relaxed );
                            node.m_funcFree( pNode );

                            arr.pop( nRetired );
                            CDS_HRC_STATISTIC( ++m_Stat.m_DeletedNode );
                            continue;
                        }

                        pNode->terminate( pThreadGC, true );
                        //node.m_bDone.store( true, CDS_ATOMIC::memory_order_release );
                        CDS_HRC_STATISTIC( ++m_Stat.m_ScanClaimGuarded );
                    }
                    else {
                        CDS_HRC_STATISTIC( ++m_Stat.m_ScanGuarded );
                    }
                }
            }
        }

        void GarbageCollector::HelpScan( ThreadGC * pThis )
        {
            if ( pThis->m_pDesc->m_arrRetired.isFull() )
                return;

            CDS_HRC_STATISTIC( ++m_Stat.m_HelpScanCalls );

            const cds::OS::ThreadId nullThreadId = cds::OS::nullThreadId();
            const cds::OS::ThreadId curThreadId  = cds::OS::getCurrentThreadId();

            for ( thread_list_node * pRec = m_pListHead.load(CDS_ATOMIC::memory_order_acquire); pRec; pRec = pRec->m_pNext )
            {
                // If threadDesc is free then own its
                cds::OS::ThreadId expectedThreadId = nullThreadId;
                if ( !pRec->m_idOwner.compare_exchange_strong(expectedThreadId, curThreadId, CDS_ATOMIC::memory_order_acquire, CDS_ATOMIC::memory_order_relaxed) )
                {
                    continue;
                }

                // We own threadDesc.
                assert( pRec->m_pOwner == null_ptr<ThreadGC *>() );

                if ( !pRec->m_bFree ) {
                    // All undeleted pointers is moved to pThis (it is private for the current thread)

                    details::retired_vector& src = pRec->m_arrRetired;
                    details::retired_vector& dest = pThis->m_pDesc->m_arrRetired;
                    assert( !dest.isFull());

                    details::retired_vector::iterator itEnd = src.end();
                    details::retired_vector::iterator it = src.begin();

                    for ( size_t nRetired = 0; it != itEnd; ++nRetired, ++it ) {
                        if ( it->m_pNode.load(CDS_ATOMIC::memory_order_relaxed) == null_ptr<ContainerNode *>() )
                            continue;

                        dest.push( it->m_pNode.load(CDS_ATOMIC::memory_order_relaxed), it->m_funcFree );
                        src.pop( nRetired );

                        while ( dest.isFull() ) {
                            pThis->cleanUpLocal();
                            if ( dest.isFull() )
                                Scan( pThis );
                            if ( dest.isFull() )
                                CleanUpAll( pThis );
                            else
                                break;
                        }
                    }
                    pRec->m_bFree = true;
                }
                pRec->m_idOwner.store( nullThreadId, CDS_ATOMIC::memory_order_release );
            }
        }

        void GarbageCollector::CleanUpAll( ThreadGC * pThis )
        {
            CDS_HRC_STATISTIC( ++m_Stat.m_CleanUpAllCalls );

            //const cds::OS::ThreadId nullThreadId = cds::OS::nullThreadId();
            thread_list_node * pThread = m_pListHead.load(CDS_ATOMIC::memory_order_acquire);
            while ( pThread ) {
                for ( size_t i = 0; i < pThread->m_arrRetired.capacity(); ++i ) {
                    details::retired_node& rRetiredNode = pThread->m_arrRetired[i];
                    ContainerNode * pNode = rRetiredNode.m_pNode.load(CDS_ATOMIC::memory_order_acquire);
                    if ( pNode && !rRetiredNode.m_bDone.load(CDS_ATOMIC::memory_order_acquire) ) {
                        rRetiredNode.m_nClaim.fetch_add( 1, CDS_ATOMIC::memory_order_release );
                        if ( !rRetiredNode.m_bDone.load(CDS_ATOMIC::memory_order_acquire)
                            && pNode == rRetiredNode.m_pNode.load(CDS_ATOMIC::memory_order_acquire) )
                        {
                            pNode->cleanUp( pThis );
                        }
                        rRetiredNode.m_nClaim.fetch_sub( 1, CDS_ATOMIC::memory_order_release );
                    }
                }
                pThread = pThread->m_pNext;
            }
        }

        GarbageCollector::internal_state& GarbageCollector::getInternalState( GarbageCollector::internal_state& stat) const
        {
            // Const
            stat.nHPCount               = m_nHazardPointerCount;
            stat.nMaxThreadCount        = m_nMaxThreadCount;
            stat.nMaxRetiredPtrCount    = m_nMaxRetiredPtrCount;
            stat.nHRCRecSize            = sizeof( thread_list_node )
                                            + sizeof( details::retired_node) * m_nMaxRetiredPtrCount;
            stat.nHRCRecAllocated            =
                stat.nHRCRecUsed             =
                stat.nTotalRetiredPtrCount   =
                stat.nRetiredPtrInFreeHRCRecs = 0;

            // Walk through HRC records
            for ( thread_list_node *hprec = m_pListHead.load(CDS_ATOMIC::memory_order_acquire); hprec; hprec = hprec->m_pNext ) {
                ++stat.nHRCRecAllocated;
                size_t nRetiredNodeCount = hprec->m_arrRetired.retiredNodeCount();
                if ( hprec->m_bFree ) {
                    stat.nRetiredPtrInFreeHRCRecs += nRetiredNodeCount;
                }
                else {
                    ++stat.nHRCRecUsed;
                }
                stat.nTotalRetiredPtrCount += nRetiredNodeCount;
            }

            // Events
            stat.evcAllocHRCRec            = m_Stat.m_AllocHRCThreadDesc;
            stat.evcRetireHRCRec        = m_Stat.m_RetireHRCThreadDesc;
            stat.evcAllocNewHRCRec        = m_Stat.m_AllocNewHRCThreadDesc;
            stat.evcDeleteHRCRec        = m_Stat.m_DeleteHRCThreadDesc;
            stat.evcScanCall            = m_Stat.m_ScanCalls;
            stat.evcHelpScanCalls       = m_Stat.m_HelpScanCalls;
            stat.evcCleanUpAllCalls     = m_Stat.m_CleanUpAllCalls;
            stat.evcDeletedNode         = m_Stat.m_DeletedNode;
            stat.evcScanGuarded         = m_Stat.m_ScanGuarded;
            stat.evcScanClaimGuarded    = m_Stat.m_ScanClaimGuarded;

#       ifdef CDS_DEBUG
            stat.evcNodeConstruct       = m_Stat.m_NodeConstructed;
            stat.evcNodeDestruct        = m_Stat.m_NodeDestructed;
#       endif

            return stat;
        }

        void ContainerNode::cleanUp( ThreadGC * /*pGC*/ )
        {
            CDS_PURE_VIRTUAL_FUNCTION_CALLED_("cds::gc::hrc::ContainerNode::cleanUp");
        }
        void ContainerNode::terminate( ThreadGC * /*pGC*/, bool /*bConcurrent*/ )
        {
            CDS_PURE_VIRTUAL_FUNCTION_CALLED_("cds::gc::hrc::ContainerNode::terminate");
        }

    }    // namespace hrc
}} // namespace cds::gc
