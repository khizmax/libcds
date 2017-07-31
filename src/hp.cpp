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

#include <algorithm>
#include <vector>

#include <cds/gc/hp.h>
#include <cds/os/thread.h>

namespace cds { namespace gc { namespace hp {

    namespace {
        void * default_alloc_memory( size_t size )
        {
            return new uintptr_t[( size + sizeof( uintptr_t ) - 1 ) / sizeof( uintptr_t) ];
        }

        void default_free_memory( void* p )
        {
            delete[] reinterpret_cast<uintptr_t*>( p );
        }

        void* ( *s_alloc_memory )( size_t size ) = default_alloc_memory;
        void ( *s_free_memory )( void* p ) = default_free_memory;

        template <typename T>
        class allocator
        {
        public:
            typedef T   value_type;

            allocator() {}
            allocator( allocator const& ) {}
            template <class U>
            explicit allocator( allocator<U> const& ) {}

            static T* allocate( size_t nCount )
            {
                return reinterpret_cast<T*>( s_alloc_memory( sizeof( value_type ) * nCount ));
            }

            static void deallocate( T* p, size_t /*nCount*/ )
            {
                s_free_memory( reinterpret_cast<void*>( p ));
            }
        };

        struct defaults {
            static const size_t c_nHazardPointerPerThread = 8;
            static const size_t c_nMaxThreadCount = 100;
        };

        size_t calc_retired_size( size_t nSize, size_t nHPCount, size_t nThreadCount )
        {
            size_t const min_size = nHPCount * nThreadCount;
            return nSize < min_size ? min_size * 2 : nSize;
        }

        stat s_postmortem_stat;
    } // namespace

    /*static*/ CDS_EXPORT_API smr* smr::instance_ = nullptr;
    thread_local thread_data* tls_ = nullptr;

    /*static*/ CDS_EXPORT_API thread_data* smr::tls()
    {
        assert( tls_ != nullptr );
        return tls_;
    }

    struct smr::thread_record: thread_data
    {
        atomics::atomic<thread_record*>     m_pNextNode; ///< next hazard ptr record in list
        atomics::atomic<cds::OS::ThreadId>  m_idOwner;   ///< Owner thread id; 0 - the record is free (not owned)
        atomics::atomic<bool>               m_bFree;     ///< true if record is free (not owned)

        thread_record( guard* guards, size_t guard_count, retired_ptr* retired_arr, size_t retired_capacity )
            : thread_data( guards, guard_count, retired_arr, retired_capacity )
            , m_pNextNode( nullptr )
            , m_idOwner( cds::OS::c_NullThreadId )
            , m_bFree( false )
        {}
    };

    /*static*/ CDS_EXPORT_API void smr::set_memory_allocator(
        void* ( *alloc_func )( size_t size ),
        void( *free_func )( void * p )
    )
    {
        // The memory allocation functions may be set BEFORE initializing HP SMR!!!
        assert( instance_ == nullptr );

        s_alloc_memory = alloc_func;
        s_free_memory = free_func;
    }


    /*static*/ CDS_EXPORT_API void smr::construct( size_t nHazardPtrCount, size_t nMaxThreadCount, size_t nMaxRetiredPtrCount, scan_type nScanType )
    {
        if ( !instance_ ) {
            instance_ = new( s_alloc_memory(sizeof(smr))) smr( nHazardPtrCount, nMaxThreadCount, nMaxRetiredPtrCount, nScanType );
        }
    }

    /*static*/ CDS_EXPORT_API void smr::destruct( bool bDetachAll )
    {
        if ( instance_ ) {
            if ( bDetachAll )
                instance_->detach_all_thread();

            instance_->~smr();
            s_free_memory( instance_ );
            instance_ = nullptr;
        }
    }

    CDS_EXPORT_API smr::smr( size_t nHazardPtrCount, size_t nMaxThreadCount, size_t nMaxRetiredPtrCount, scan_type nScanType )
        : hazard_ptr_count_( nHazardPtrCount == 0 ? defaults::c_nHazardPointerPerThread : nHazardPtrCount )
        , max_thread_count_( nMaxThreadCount == 0 ? defaults::c_nMaxThreadCount : nMaxThreadCount )
        , max_retired_ptr_count_( calc_retired_size( nMaxRetiredPtrCount, hazard_ptr_count_, max_thread_count_ ))
        , scan_type_( nScanType )
        , scan_func_( nScanType == classic ? &smr::classic_scan : &smr::inplace_scan )
    {
        thread_list_.store( nullptr, atomics::memory_order_release );
    }

    CDS_EXPORT_API smr::~smr()
    {
        CDS_DEBUG_ONLY( const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId; )
        CDS_DEBUG_ONLY( const cds::OS::ThreadId mainThreadId = cds::OS::get_current_thread_id();)

        CDS_HPSTAT( statistics( s_postmortem_stat ));

        thread_record* pHead = thread_list_.load( atomics::memory_order_relaxed );
        thread_list_.store( nullptr, atomics::memory_order_release );

        thread_record* pNext = nullptr;
        for ( thread_record* hprec = pHead; hprec; hprec = pNext )
        {
            assert( hprec->m_idOwner.load( atomics::memory_order_relaxed ) == nullThreadId
                || hprec->m_idOwner.load( atomics::memory_order_relaxed ) == mainThreadId );

            retired_array& arr = hprec->retired_;
            for ( retired_ptr* cur{ arr.first() }, *last{ arr.last() }; cur != last; ++cur ) {
                cur->free();
                CDS_HPSTAT( ++s_postmortem_stat.free_count );
            }

            arr.reset( 0 );
            pNext = hprec->m_pNextNode.load( atomics::memory_order_relaxed );
            hprec->m_bFree.store( true, atomics::memory_order_relaxed );
            destroy_thread_data( hprec );
        }
    }


    CDS_EXPORT_API smr::thread_record* smr::create_thread_data()
    {
        size_t const guard_array_size = thread_hp_storage::calc_array_size( get_hazard_ptr_count());
        size_t const retired_array_size = retired_array::calc_array_size( get_max_retired_ptr_count());
        size_t const nSize = sizeof( thread_record ) + guard_array_size + retired_array_size;

        /*
            The memory is allocated by contnuous block
            Memory layout:
            +--------------------------+
            |                          |
            | thread_record            |
            |         hazards_         +---+
        +---|         retired_         |   |
        |   |                          |   |
        |   |--------------------------|   |
        |   | hazard_ptr[]             |<--+
        |   |                          |
        |   |                          |
        |   |--------------------------|
        +-->| retired_ptr[]            |
            |                          |
            |                          |
            +--------------------------+
        */

        uint8_t* mem = reinterpret_cast<uint8_t*>( s_alloc_memory( nSize ));

        return new( mem ) thread_record(
            reinterpret_cast<guard*>( mem + sizeof( thread_record )),
            get_hazard_ptr_count(),
            reinterpret_cast<retired_ptr*>( mem + sizeof( thread_record ) + guard_array_size ),
            get_max_retired_ptr_count()
        );
    }

    /*static*/ CDS_EXPORT_API void smr::destroy_thread_data( thread_record* pRec )
    {
        // all retired pointers must be freed
        assert( pRec->retired_.size() == 0 );

        pRec->~thread_record();
        s_free_memory( pRec );
    }


    CDS_EXPORT_API smr::thread_record* smr::alloc_thread_data()
    {
        thread_record * hprec;
        const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
        const cds::OS::ThreadId curThreadId = cds::OS::get_current_thread_id();

        // First try to reuse a free (non-active) HP record
        for ( hprec = thread_list_.load( atomics::memory_order_acquire ); hprec; hprec = hprec->m_pNextNode.load( atomics::memory_order_acquire )) {
            cds::OS::ThreadId thId = nullThreadId;
            if ( !hprec->m_idOwner.compare_exchange_strong( thId, curThreadId, atomics::memory_order_relaxed, atomics::memory_order_relaxed ))
                continue;
            hprec->m_bFree.store( false, atomics::memory_order_release );
            return hprec;
        }

        // No HP records available for reuse
        // Allocate and push a new HP record
        hprec = create_thread_data();
        hprec->m_idOwner.store( curThreadId, atomics::memory_order_relaxed );

        thread_record* pOldHead = thread_list_.load( atomics::memory_order_relaxed );
        do {
            hprec->m_pNextNode.store( pOldHead, atomics::memory_order_release );
        } while ( !thread_list_.compare_exchange_weak( pOldHead, hprec, atomics::memory_order_release, atomics::memory_order_acquire ));

        return hprec;
    }

    CDS_EXPORT_API void smr::free_thread_data( smr::thread_record* pRec )
    {
        assert( pRec != nullptr );

        pRec->hazards_.clear();
        scan( pRec );
        help_scan( pRec );
        pRec->m_idOwner.store( cds::OS::c_NullThreadId, atomics::memory_order_release );
    }

    CDS_EXPORT_API void smr::detach_all_thread()
    {
        thread_record * pNext = nullptr;
        const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;

        for ( thread_record * hprec = thread_list_.load( atomics::memory_order_relaxed ); hprec; hprec = pNext ) {
            pNext = hprec->m_pNextNode.load( atomics::memory_order_relaxed );
            if ( hprec->m_idOwner.load( atomics::memory_order_relaxed ) != nullThreadId ) {
                free_thread_data( hprec );
            }
        }
    }

    /*static*/ CDS_EXPORT_API void smr::attach_thread()
    {
        if ( !tls_ )
            tls_ = instance().alloc_thread_data();
    }

    /*static*/ CDS_EXPORT_API void smr::detach_thread()
    {
        thread_data* rec = tls_;
        if ( rec ) {
            tls_ = nullptr;
            instance().free_thread_data( static_cast<thread_record*>( rec ));
        }
    }


    CDS_EXPORT_API void smr::inplace_scan( thread_data* pThreadRec )
    {
        thread_record* pRec = static_cast<thread_record*>( pThreadRec );

        //CDS_HAZARDPTR_STATISTIC( ++m_Stat.m_ScanCallCount )

        // In-place scan algo uses LSB of retired ptr as a mark for internal purposes.
        // It is correct if all retired pointers are ar least 2-byte aligned (LSB is zero).
        // If it is wrong, we use classic scan algorithm

        // Check if all retired pointers has zero LSB
        // LSB is used for marking pointers that cannot be deleted yet
        retired_ptr* first_retired = pRec->retired_.first();
        retired_ptr* last_retired = pRec->retired_.last();
        if ( first_retired == last_retired )
            return;

        for ( auto it = first_retired; it != last_retired; ++it ) {
            if ( it->m_n & 1 ) {
                // found a pointer with LSB bit set - use classic_scan
                classic_scan( pRec );
                return;
            }
        }

        CDS_HPSTAT( ++pThreadRec->scan_count_ );

        // Sort retired pointer array
        std::sort( first_retired, last_retired, retired_ptr::less );

        // Check double free
#   ifdef _DEBUG
        {
            auto it = first_retired;
            auto itPrev = it;
            while ( ++it != last_retired ) {
                assert( itPrev->m_p < it->m_p );
                itPrev = it;
            }
        }
#   endif

        // Search guarded pointers in retired array
        thread_record* pNode = thread_list_.load( atomics::memory_order_acquire );

        {
            retired_ptr dummy_retired;
            while ( pNode ) {
                if ( pNode->m_idOwner.load( atomics::memory_order_relaxed ) != cds::OS::c_NullThreadId ) {
                    thread_hp_storage& hpstg = pNode->hazards_;
                    for ( size_t i = 0; i < hazard_ptr_count_; ++i ) {
                        pRec->sync();
                        void * hptr = hpstg[i].get();
                        if ( hptr ) {
                            dummy_retired.m_p = hptr;
                            retired_ptr* it = std::lower_bound( first_retired, last_retired, dummy_retired, retired_ptr::less );
                            if ( it != last_retired && it->m_p == hptr ) {
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
            retired_ptr* insert_pos = first_retired;
            for ( retired_ptr* it = first_retired; it != last_retired; ++it ) {
                if ( it->m_n & 1 ) {
                    it->m_n &= ~uintptr_t(1);
                    if ( insert_pos != it )
                        *insert_pos = *it;
                    ++insert_pos;
                }
                else {
                    // Retired pointer may be freed
                    it->free();
                    CDS_HPSTAT( ++pRec->free_count_ );
                }
            }
            const size_t nDeferred = insert_pos - first_retired;
            pRec->retired_.reset( nDeferred );
        }
    }

    // cppcheck-suppress functionConst
    CDS_EXPORT_API void smr::classic_scan( thread_data* pThreadRec )
    {
        thread_record* pRec = static_cast<thread_record*>( pThreadRec );

        CDS_HPSTAT( ++pThreadRec->scan_count_ );

        std::vector< void*, allocator<void*>>   plist;
        plist.reserve( get_max_thread_count() * get_hazard_ptr_count());
        assert( plist.size() == 0 );

        // Stage 1: Scan HP list and insert non-null values in plist

        thread_record* pNode = thread_list_.load( atomics::memory_order_acquire );

        while ( pNode ) {
            if ( pNode->m_idOwner.load( std::memory_order_relaxed ) != cds::OS::c_NullThreadId ) {
                for ( size_t i = 0; i < get_hazard_ptr_count(); ++i ) {
                    pRec->sync();
                    void * hptr = pNode->hazards_[i].get();
                    if ( hptr )
                        plist.push_back( hptr );
                }
            }
            pNode = pNode->m_pNextNode.load( atomics::memory_order_relaxed );
        }

        // Sort plist to simplify search in
        std::sort( plist.begin(), plist.end());

        // Stage 2: Search plist
        retired_array& retired = pRec->retired_;

        retired_ptr* first_retired = retired.first();
        retired_ptr* last_retired = retired.last();

        {
            auto itBegin = plist.begin();
            auto itEnd = plist.end();
            retired_ptr* insert_pos = first_retired;
            for ( retired_ptr* it = first_retired; it != last_retired; ++it ) {
                if ( std::binary_search( itBegin, itEnd, first_retired->m_p )) {
                    if ( insert_pos != it )
                        *insert_pos = *it;
                    ++insert_pos;
                }
                else {
                    it->free();
                    CDS_HPSTAT( ++pRec->free_count_ );
                }
            }

            retired.reset( insert_pos - first_retired );
        }
    }

    CDS_EXPORT_API void smr::help_scan( thread_data* pThis )
    {
        assert( static_cast<thread_record*>( pThis )->m_idOwner.load( atomics::memory_order_relaxed ) == cds::OS::get_current_thread_id());

        CDS_HPSTAT( ++pThis->help_scan_count_ );

        const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
        const cds::OS::ThreadId curThreadId = cds::OS::get_current_thread_id();
        for ( thread_record* hprec = thread_list_.load( atomics::memory_order_acquire ); hprec; hprec = hprec->m_pNextNode.load( atomics::memory_order_relaxed ))
        {
            if ( hprec == static_cast<thread_record*>( pThis ))
                continue;

            // If m_bFree == true then hprec->retired_ is empty - we don't need to see it
            if ( hprec->m_bFree.load( atomics::memory_order_acquire ))
                continue;

            // Owns hprec if it is empty.
            // Several threads may work concurrently so we use atomic technique only.
            {
                cds::OS::ThreadId curOwner = hprec->m_idOwner.load( atomics::memory_order_relaxed );
                if ( curOwner == nullThreadId ) {
                    if ( !hprec->m_idOwner.compare_exchange_strong( curOwner, curThreadId, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                        continue;
                }
                else
                    continue;
            }

            // We own the thread record successfully. Now, we can see whether it has retired pointers.
            // If it has ones then we move them to pThis that is private for current thread.
            retired_array& src = hprec->retired_;
            retired_array& dest = pThis->retired_;
            assert( !dest.full());

            retired_ptr* src_first = src.first();
            retired_ptr* src_last = src.last();

            for ( ; src_first != src_last; ++src_first ) {
                if ( !dest.push( std::move( *src_first )))
                    scan( pThis );
            }

            src.interthread_clear();
            hprec->m_bFree.store( true, atomics::memory_order_release );
            hprec->m_idOwner.store( nullThreadId, atomics::memory_order_release );

            scan( pThis );
        }
    }

    CDS_EXPORT_API void smr::statistics( stat& st )
    {
        st.clear();
#   ifdef CDS_ENABLE_HPSTAT
        for ( thread_record* hprec = thread_list_.load( atomics::memory_order_acquire ); hprec; hprec = hprec->m_pNextNode.load( atomics::memory_order_relaxed ))
        {
            CDS_TSAN_ANNOTATE_IGNORE_READS_BEGIN;
            ++st.thread_rec_count;
            st.guard_allocated += hprec->hazards_.alloc_guard_count_;
            st.guard_freed     += hprec->hazards_.free_guard_count_;
            st.retired_count   += hprec->retired_.retire_call_count_;
            st.free_count      += hprec->free_count_;
            st.scan_count      += hprec->scan_count_;
            st.help_scan_count += hprec->help_scan_count_;
            CDS_TSAN_ANNOTATE_IGNORE_READS_END;
        }
#   endif
    }

}}} // namespace cds::gc::hp

CDS_EXPORT_API /*static*/ cds::gc::HP::stat const& cds::gc::HP::postmortem_statistics()
{
    return cds::gc::hp::s_postmortem_stat;
}

