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

// Dynamic Hazard Pointer memory manager implementation

#include <algorithm>   // std::fill
#include <functional>  // std::hash

#include <cds/gc/details/dhp.h>
#include <cds/algo/int_algo.h>

namespace cds { namespace gc { namespace dhp {

    namespace details {

        class liberate_set {
            typedef retired_ptr_node *  item_type;
            typedef cds::details::Allocator<item_type, CDS_DEFAULT_ALLOCATOR>   allocator_type;

            size_t const m_nBucketCount;
            item_type *  m_Buckets;

            item_type&  bucket( retired_ptr_node& node ) const
            {
                return bucket( node.m_ptr.m_p );
            }
            item_type&  bucket( guard_data::guarded_ptr p ) const
            {
                return m_Buckets[ std::hash<guard_data::guarded_ptr>()( p ) & (m_nBucketCount - 1)  ];
            }

        public:
            liberate_set( size_t nBucketCount )
                : m_nBucketCount( nBucketCount )
            {
                assert( nBucketCount > 0 );
                assert( (nBucketCount & (nBucketCount - 1)) == 0 );

                m_Buckets = allocator_type().NewArray( nBucketCount );
                std::fill( m_Buckets, m_Buckets + nBucketCount, nullptr );
            }

            ~liberate_set()
            {
                allocator_type().Delete( m_Buckets, m_nBucketCount );
            }

            void insert( retired_ptr_node& node )
            {
                node.m_pNext.store( nullptr, atomics::memory_order_relaxed );

                item_type& refBucket = bucket( node );
                if ( refBucket ) {
                    item_type p = refBucket;
                    item_type prev = nullptr;
                    do {
                        if ( p->m_ptr.m_p >= node.m_ptr.m_p ) {
                            node.m_pNext.store( p, atomics::memory_order_relaxed );
                            if ( prev )
                                prev->m_pNext.store( &node, atomics::memory_order_relaxed );
                            else
                                refBucket = &node;
                            return;
                        }
                        prev = p;
                        p = p->m_pNext.load(atomics::memory_order_relaxed);
                    } while ( p );

                    assert( prev != nullptr );
                    prev->m_pNext.store( &node, atomics::memory_order_relaxed );
                }
                else
                    refBucket = &node;
            }

            struct erase_result
            {
                item_type head;
                item_type tail;
                size_t    size;

                erase_result()
                    : head( nullptr )
                    , tail( nullptr )
                    , size(0)
                {}
            };

            erase_result erase( guard_data::guarded_ptr ptr )
            {
                item_type& refBucket = bucket( ptr );
                item_type p = refBucket;
                item_type pPrev = nullptr;

                erase_result ret;
                while ( p && p->m_ptr.m_p <= ptr ) {
                    if ( p->m_ptr.m_p == ptr ) {
                        if ( pPrev )
                            pPrev->m_pNext.store( p->m_pNext.load(atomics::memory_order_relaxed ), atomics::memory_order_relaxed );
                        else
                            refBucket = p->m_pNext.load(atomics::memory_order_relaxed);

                        if ( ret.head )
                            ret.tail->m_pNext.store( p, atomics::memory_order_relaxed );
                        else
                            ret.head = p;
                        ret.tail = p;
                        ++ret.size;
                    }
                    else
                        pPrev = p;
                    p = p->m_pNext.load( atomics::memory_order_relaxed );
                }

                if ( ret.tail )
                    ret.tail->m_pNext.store( nullptr, atomics::memory_order_relaxed );
                return ret;
            }

            typedef std::pair<item_type, item_type>     list_range;

            list_range free_all()
            {
                item_type pTail = nullptr;
                list_range ret = std::make_pair( pTail, pTail );

                item_type const * pEndBucket = m_Buckets + m_nBucketCount;
                for ( item_type * ppBucket = m_Buckets; ppBucket < pEndBucket; ++ppBucket ) {
                    item_type pBucket = *ppBucket;
                    if ( pBucket ) {
                        if ( ret.first )
                            pTail->m_pNextFree.store( pBucket, atomics::memory_order_relaxed );
                        else
                            ret.first = pBucket;

                        pTail = pBucket;
                        for (;;) {
                            item_type pNext = pTail->m_pNext.load( atomics::memory_order_relaxed );
                            pTail->m_ptr.free();
                            pTail->m_pNext.store( nullptr, atomics::memory_order_relaxed );

                            /*
                            while ( pTail->m_pNextFree.load( atomics::memory_order_relaxed )) {
                                pTail = pTail->m_pNextFree.load( atomics::memory_order_relaxed );
                                pTail->m_ptr.free();
                                pTail->m_pNext.store( nullptr, atomics::memory_order_relaxed );
                            }
                            */

                            if ( pNext ) {
                                pTail->m_pNextFree.store( pNext, atomics::memory_order_relaxed );
                                pTail = pNext;
                            }
                            else
                                break;
                        }
                    }
                }

                if ( pTail )
                    pTail->m_pNextFree.store( nullptr, atomics::memory_order_relaxed );
                ret.second = pTail;
                return ret;
            }
        };
    }

    GarbageCollector * GarbageCollector::m_pManager = nullptr;

    void CDS_STDCALL GarbageCollector::Construct(
        size_t nLiberateThreshold
        , size_t nInitialThreadGuardCount
        , size_t nEpochCount
    )
    {
        if ( !m_pManager ) {
            m_pManager = new GarbageCollector( nLiberateThreshold, nInitialThreadGuardCount, nEpochCount );
        }
    }

    void CDS_STDCALL GarbageCollector::Destruct()
    {
        delete m_pManager;
        m_pManager = nullptr;
    }

    GarbageCollector::GarbageCollector( size_t nLiberateThreshold, size_t nInitialThreadGuardCount, size_t nEpochCount )
        : m_nLiberateThreshold( nLiberateThreshold ? nLiberateThreshold : 1024 )
        , m_nInitialThreadGuardCount( nInitialThreadGuardCount ? nInitialThreadGuardCount : 8 )
        , m_RetiredAllocator( static_cast<unsigned int>( nEpochCount ? nEpochCount : 16 ))
        , m_bStatEnabled( false )
    {}

    GarbageCollector::~GarbageCollector()
    {
        scan();
    }

    void GarbageCollector::scan()
    {
        details::retired_ptr_buffer::privatize_result retiredList = m_RetiredBuffer.privatize();
        if ( retiredList.first ) {

            size_t nLiberateThreshold = m_nLiberateThreshold.load(atomics::memory_order_relaxed);
            details::liberate_set set( beans::ceil2( retiredList.second > nLiberateThreshold ? retiredList.second : nLiberateThreshold ));

            // Get list of retired pointers
            size_t nRetiredCount = 0;
            details::retired_ptr_node * pHead = retiredList.first;
            while ( pHead ) {
                details::retired_ptr_node * pNext = pHead->m_pNext.load( atomics::memory_order_relaxed );
                pHead->m_pNextFree.store( nullptr, atomics::memory_order_relaxed );
                set.insert( *pHead );
                pHead = pNext;
                ++nRetiredCount;
            }

            // Liberate cycle

            details::retired_ptr_node dummy;
            dummy.m_pNext.store( nullptr, atomics::memory_order_relaxed );
            details::retired_ptr_node * pBusyLast = &dummy;
            size_t nBusyCount = 0;

            for ( details::guard_data * pGuard = m_GuardPool.begin(); pGuard; pGuard = pGuard->pGlobalNext.load(atomics::memory_order_acquire))
            {
                // get guarded pointer
                details::guard_data::guarded_ptr valGuarded = pGuard->pPost.load(atomics::memory_order_acquire);

                if ( valGuarded ) {
                    auto retired = set.erase( valGuarded );
                    if ( retired.head ) {
                        // Retired pointer is being guarded
                        // [retired.head, retired.tail] is the list linked by m_pNext field

                        pBusyLast->m_pNext.store( retired.head, atomics::memory_order_relaxed );
                        pBusyLast = retired.tail;
                        nBusyCount += retired.size;
                    }
                }
            }

            // Place [dummy.m_pNext, pBusyLast] back to m_RetiredBuffer
            if ( nBusyCount )
                m_RetiredBuffer.push_list( dummy.m_pNext.load(atomics::memory_order_relaxed), pBusyLast, nBusyCount );

            // Free all retired pointers
            details::liberate_set::list_range range = set.free_all();

            m_RetiredAllocator.inc_epoch();

            if ( range.first ) {
                assert( range.second != nullptr );
                m_RetiredAllocator.free_range( range.first, range.second );
            }
            else if ( nRetiredCount >= nLiberateThreshold ) {
                // scan() cycle did not free any retired pointer - double scan() threshold
                m_nLiberateThreshold.compare_exchange_strong( nLiberateThreshold, nLiberateThreshold * 2, atomics::memory_order_release, atomics::memory_order_relaxed );
            }
        }
    }
}}} // namespace cds::gc::dhp
