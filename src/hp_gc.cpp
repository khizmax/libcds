/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
    File: hzp_gc.cpp

    Hazard Pointers memory reclamation strategy implementation

    Editions:
        2008.02.10    Maxim.Khiszinsky    Created
*/

#include <cds/gc/details/hp.h>

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
            ,m_bStatEnabled( false )
            ,m_nHazardPointerCount( nHazardPtrCount == 0 ? c_nHazardPointerPerThread : nHazardPtrCount )
            ,m_nMaxThreadCount( nMaxThreadCount == 0 ? c_nMaxThreadCount : nMaxThreadCount )
            ,m_nMaxRetiredPtrCount( nMaxRetiredPtrCount > c_nMaxRetireNodeCount ? nMaxRetiredPtrCount : c_nMaxRetireNodeCount )
            ,m_nScanType( nScanType )
        {}

        GarbageCollector::~GarbageCollector()
        {
            CDS_DEBUG_ONLY( const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId; )
            CDS_DEBUG_ONLY( const cds::OS::ThreadId mainThreadId = cds::OS::get_current_thread_id() ;)

            hplist_node * pHead = m_pListHead.load( atomics::memory_order_relaxed );
            m_pListHead.store( nullptr, atomics::memory_order_relaxed );

            hplist_node * pNext = nullptr;
            for ( hplist_node * hprec = pHead; hprec; hprec = pNext ) {
                assert( hprec->m_idOwner.load( atomics::memory_order_relaxed ) == nullThreadId
                    || hprec->m_idOwner.load( atomics::memory_order_relaxed ) == mainThreadId
                    || !cds::OS::is_thread_alive( hprec->m_idOwner.load( atomics::memory_order_relaxed ))
                );
                details::retired_vector& vect = hprec->m_arrRetired;
                details::retired_vector::iterator itRetired = vect.begin();
                details::retired_vector::iterator itRetiredEnd = vect.end();
                while ( itRetired != itRetiredEnd ) {
                    itRetired->free();
                    ++itRetired;
                }
                vect.clear();
                pNext = hprec->m_pNextNode.load( atomics::memory_order_relaxed );
                hprec->m_bFree.store( true, atomics::memory_order_relaxed );
                DeleteHPRec( hprec );
            }
        }

        inline GarbageCollector::hplist_node * GarbageCollector::NewHPRec( OS::ThreadId owner )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_AllocNewHPRec )
            return new hplist_node( *this, owner );
        }

        inline void GarbageCollector::DeleteHPRec( hplist_node * pNode )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_DeleteHPRec )
            assert( pNode->m_arrRetired.size() == 0 );
            delete pNode;
        }

        details::hp_record * GarbageCollector::alloc_hp_record()
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_AllocHPRec )

            hplist_node * hprec;
            const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
            const cds::OS::ThreadId curThreadId  = cds::OS::get_current_thread_id();

            // First try to reuse a retired (non-active) HP record
            for ( hprec = m_pListHead.load( atomics::memory_order_relaxed ); hprec; hprec = hprec->m_pNextNode.load( atomics::memory_order_relaxed )) {
                cds::OS::ThreadId thId = nullThreadId;
                if ( !hprec->m_idOwner.compare_exchange_strong( thId, curThreadId, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                    continue;
                hprec->m_bFree.store( false, atomics::memory_order_relaxed );
                return hprec;
            }

            // No HP records available for reuse
            // Allocate and push a new HP record
            hprec = NewHPRec( curThreadId );

            hplist_node * pOldHead = m_pListHead.load( atomics::memory_order_relaxed );
            do {
                hprec->m_pNextNode.store( pOldHead, atomics::memory_order_relaxed );
            } while ( !m_pListHead.compare_exchange_weak( pOldHead, hprec, atomics::memory_order_acq_rel, atomics::memory_order_acquire ));

            return hprec;
        }

        void GarbageCollector::free_hp_record( details::hp_record * pRec )
        {
            assert( pRec != nullptr );
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_RetireHPRec )

            pRec->clear();
            Scan( pRec );
            HelpScan( pRec );
            hplist_node * pNode = static_cast<hplist_node *>( pRec );
            pNode->m_idOwner.store( cds::OS::c_NullThreadId, atomics::memory_order_release );
        }

        void GarbageCollector::detachAllThread()
        {
            hplist_node * pNext = nullptr;
            const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
            for ( hplist_node * hprec = m_pListHead.load(atomics::memory_order_relaxed); hprec; hprec = pNext ) {
                pNext = hprec->m_pNextNode.load( atomics::memory_order_relaxed );
                if ( hprec->m_idOwner.load(atomics::memory_order_relaxed) != nullThreadId ) {
                    free_hp_record( hprec );
                }
            }
        }

        void GarbageCollector::classic_scan( details::hp_record * pRec )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_ScanCallCount )

            std::vector< void * >   plist;
            plist.reserve( m_nMaxThreadCount * m_nHazardPointerCount );
            assert( plist.size() == 0 );

            // Stage 1: Scan HP list and insert non-null values in plist

            hplist_node * pNode = m_pListHead.load(atomics::memory_order_relaxed);

            while ( pNode ) {
                for ( size_t i = 0; i < m_nHazardPointerCount; ++i ) {
                    pRec->sync();
                    void * hptr = pNode->m_hzp[i].get();
                    if ( hptr )
                        plist.push_back( hptr );
                }
                pNode = pNode->m_pNextNode.load( atomics::memory_order_relaxed );
            }

            // Sort plist to simplify search in
            std::sort( plist.begin(), plist.end());

            // Stage 2: Search plist
            details::retired_vector& arrRetired = pRec->m_arrRetired;

            details::retired_vector::iterator itRetired     = arrRetired.begin();
            details::retired_vector::iterator itRetiredEnd  = arrRetired.end();
            // arrRetired is not a std::vector!
            // clear() is just set up item counter to 0, the items is not destroyed
            arrRetired.clear();

            {
                std::vector< void * >::iterator itBegin = plist.begin();
                std::vector< void * >::iterator itEnd = plist.end();
                size_t nDeferredCount = 0;
                while ( itRetired != itRetiredEnd ) {
                    if ( std::binary_search( itBegin, itEnd, itRetired->m_p )) {
                        arrRetired.push( *itRetired );
                        ++nDeferredCount;
                    }
                    else
                        itRetired->free();
                    ++itRetired;
                }
                CDS_HAZARDPTR_STATISTIC( m_Stat.m_DeferredNode += nDeferredCount )
                CDS_HAZARDPTR_STATISTIC( m_Stat.m_DeletedNode += (itRetiredEnd - arrRetired.begin()) - nDeferredCount )
            }
        }

        void GarbageCollector::inplace_scan( details::hp_record * pRec )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_ScanCallCount )

            // In-place scan algo uses LSB of retired ptr as a mark for internal purposes.
            // It is correct if all retired pointers are ar least 2-byte aligned (LSB is zero).
            // If it is wrong, we use classic scan algorithm

            // Check if all retired pointers has zero LSB
            // LSB is used for marking pointers that cannot be deleted yet
            details::retired_vector::iterator itRetired     = pRec->m_arrRetired.begin();
            details::retired_vector::iterator itRetiredEnd  = pRec->m_arrRetired.end();
            for ( auto it = itRetired; it != itRetiredEnd; ++it ) {
                if ( it->m_n & 1 ) {
                    // found a pointer with LSB bit set - use classic_scan
                    classic_scan( pRec );
                    return;
                }
            }

            // Sort retired pointer array
            std::sort( itRetired, itRetiredEnd, cds::gc::details::retired_ptr::less );

            // Check double free
            /*
            {
                auto it = itRetired;
                auto itPrev = it;
                while ( ++it != itRetiredEnd ) {
                    if ( it->m_p == itPrev->m_p )
                        throw std::runtime_error( "Double free" );
                    itPrev = it;
                }
            }
            */

            // Search guarded pointers in retired array
            hplist_node * pNode = m_pListHead.load( atomics::memory_order_relaxed );

            {
                details::retired_ptr dummyRetired;
                while ( pNode ) {
                    if ( !pNode->m_bFree.load( atomics::memory_order_relaxed )) {
                        for ( size_t i = 0; i < m_nHazardPointerCount; ++i ) {
                            pRec->sync();
                            void * hptr = pNode->m_hzp[i].get();
                            if ( hptr ) {
                                dummyRetired.m_p = hptr;
                                details::retired_vector::iterator it = std::lower_bound( itRetired, itRetiredEnd, dummyRetired, cds::gc::details::retired_ptr::less );
                                if ( it != itRetiredEnd && it->m_p == hptr ) {
                                    // Mark retired pointer as guarded
                                    it->m_n |= 1;
                                }
                            }
                        }
                    }
                    pNode = pNode->m_pNextNode.load( atomics::memory_order_relaxed );
                }
            }

            // Move all marked pointers to head of array
            {
                auto itInsert = itRetired;
                for ( auto it = itRetired; it != itRetiredEnd; ++it ) {
                    if ( it->m_n & 1 ) {
                        it->m_n &= ~1;
                        if ( itInsert != it )
                            *itInsert = *it;
                        ++itInsert;
                    }
                    else {
                        // Retired pointer may be freed
                        it->free();
                    }
                }
                const size_t nDeferred = itInsert - itRetired;
                pRec->m_arrRetired.size( nDeferred );
                CDS_HAZARDPTR_STATISTIC( m_Stat.m_DeferredNode += nDeferred )
                CDS_HAZARDPTR_STATISTIC( m_Stat.m_DeletedNode += (itRetiredEnd - itRetired) - nDeferred )
            }
        }

        void GarbageCollector::HelpScan( details::hp_record * pThis )
        {
            CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_HelpScanCallCount )

            assert( static_cast<hplist_node *>(pThis)->m_idOwner.load(atomics::memory_order_relaxed) == cds::OS::get_current_thread_id());

            const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
            const cds::OS::ThreadId curThreadId = cds::OS::get_current_thread_id();
            for ( hplist_node * hprec = m_pListHead.load(atomics::memory_order_relaxed); hprec; hprec = hprec->m_pNextNode.load( atomics::memory_order_relaxed )) {

                // If m_bFree == true then hprec->m_arrRetired is empty - we don't need to see it
                if ( hprec->m_bFree.load(atomics::memory_order_relaxed))
                    continue;

                // Owns hprec if it is empty.
                // Several threads may work concurrently so we use atomic technique only.
                {
                    cds::OS::ThreadId curOwner = hprec->m_idOwner.load(atomics::memory_order_relaxed);
                    if ( curOwner == nullThreadId || !cds::OS::is_thread_alive( curOwner )) {
                        if ( !hprec->m_idOwner.compare_exchange_strong( curOwner, curThreadId, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                            continue;
                    }
                    else
                        continue;
                    //else {
                    //    curOwner = nullThreadId;
                    //    if ( !hprec->m_idOwner.compare_exchange_strong( curOwner, curThreadId, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                    //        continue;
                    //}
                }

                // We own the thread successfully. Now, we can see whether hp_record has retired pointers.
                // If it has ones then we move to pThis that is private for current thread.
                details::retired_vector& src = hprec->m_arrRetired;
                details::retired_vector& dest = pThis->m_arrRetired;
                assert( !dest.isFull());
                details::retired_vector::iterator itRetired = src.begin();

                // TSan can issue a warning here:
                //  read src.m_nSize in src.end()
                //  write src.m_nSize in src.clear()
                // This is false positive since we own hprec
                CDS_TSAN_ANNOTATE_IGNORE_READS_BEGIN;
                details::retired_vector::iterator itRetiredEnd = src.end();
                CDS_TSAN_ANNOTATE_IGNORE_READS_END;

                while ( itRetired != itRetiredEnd ) {
                    dest.push( *itRetired );
                    if ( dest.isFull()) {
                        CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_CallScanFromHelpScan )
                        Scan( pThis );
                    }
                    ++itRetired;
                }

                // TSan: write src.m_nSize, see a comment above
                CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN;
                src.clear();
                CDS_TSAN_ANNOTATE_IGNORE_WRITES_END;

                hprec->m_bFree.store(true, atomics::memory_order_relaxed);
                hprec->m_idOwner.store( nullThreadId, atomics::memory_order_release );

                Scan( pThis );
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

            for ( hplist_node * hprec = m_pListHead.load(atomics::memory_order_relaxed); hprec; hprec = hprec->m_pNextNode.load( atomics::memory_order_relaxed )) {
                ++stat.nHPRecAllocated;
                stat.nTotalRetiredPtrCount += hprec->m_arrRetired.size();

                if ( hprec->m_bFree.load(atomics::memory_order_relaxed)) {
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
