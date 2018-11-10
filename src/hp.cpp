// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <vector>

#include <cds/gc/hp.h>
#include <cds/os/thread.h>
#include <cds/gc/hp_membar.h>

#if CDS_OS_TYPE == CDS_OS_LINUX
#   include <unistd.h>
#   include <sys/syscall.h>

    // membarrier() was added in Linux 4.3
#   if !defined( __NR_membarrier )
#       define __NR_membarrier 324
#   endif

#   ifdef CDS_HAVE_LINUX_MEMBARRIER_H
#       include <linux/membarrier.h>
#   else
#       define MEMBARRIER_CMD_QUERY                         0
#       define MEMBARRIER_CMD_SHARED                        (1<<0)
#   endif
    // linux 4.14+
#   define CDS_MEMBARRIER_CMD_PRIVATE_EXPEDITED             (1<<3)
#   define CDS_MEMBARRIER_CMD_REGISTER_PRIVATE_EXPEDITED    (1<<4)
#endif

namespace cds { namespace gc { namespace hp {

    std::atomic<unsigned> shared_var_membar::shared_var_{ 0 };

#if CDS_OS_TYPE == CDS_OS_LINUX

    bool asymmetric_membar::membarrier_available_ = false;

    void asymmetric_membar::check_membarrier_available()
    {
        int res = syscall( __NR_membarrier, MEMBARRIER_CMD_QUERY, 0 );
        membarrier_available_ = !( res == -1 || ( res & CDS_MEMBARRIER_CMD_PRIVATE_EXPEDITED ) == 0 )
            && syscall( __NR_membarrier, CDS_MEMBARRIER_CMD_REGISTER_PRIVATE_EXPEDITED, 0 ) == 0;
    }

    void asymmetric_membar::call_membarrier()
    {
        assert( membarrier_available_ );
        syscall( __NR_membarrier, CDS_MEMBARRIER_CMD_PRIVATE_EXPEDITED, 0 );
    }

    bool asymmetric_global_membar::membarrier_available_ = false;

    void asymmetric_global_membar::check_membarrier_available()
    {
        int res = syscall( __NR_membarrier, MEMBARRIER_CMD_QUERY, 0 );
        membarrier_available_ = !( res == -1 || ( res & MEMBARRIER_CMD_SHARED ) == 0 );
    }

    void asymmetric_global_membar::call_membarrier()
    {
        assert( membarrier_available_ );
        syscall( __NR_membarrier, MEMBARRIER_CMD_SHARED, 0 );
    }

#endif

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
        // next hazard ptr record in list
        thread_record*                      next_ = nullptr; 
        // Owner thread id; 0 - the record is free (not owned)
        atomics::atomic<cds::OS::ThreadId>  thread_id_{ cds::OS::c_NullThreadId };
        // true if record is free (not owned)
        atomics::atomic<bool>               free_{ false };

        thread_record( guard* guards, size_t guard_count, retired_ptr* retired_arr, size_t retired_capacity )
            : thread_data( guards, guard_count, retired_arr, retired_capacity )
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
            assert( hprec->thread_id_.load( atomics::memory_order_relaxed ) == nullThreadId
                || hprec->thread_id_.load( atomics::memory_order_relaxed ) == mainThreadId );

            retired_array& arr = hprec->retired_;
            for ( retired_ptr* cur{ arr.first() }, *last{ arr.last() }; cur != last; ++cur ) {
                cur->free();
                CDS_HPSTAT( ++s_postmortem_stat.free_count );
            }

            arr.reset( 0 );
            pNext = hprec->next_;
            hprec->free_.store( true, atomics::memory_order_relaxed );
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
        for ( hprec = thread_list_.load( atomics::memory_order_acquire ); hprec; hprec = hprec->next_ ) {
            cds::OS::ThreadId thId = nullThreadId;
            if ( !hprec->thread_id_.compare_exchange_strong( thId, curThreadId, atomics::memory_order_relaxed, atomics::memory_order_relaxed ))
                continue;
            hprec->free_.store( false, atomics::memory_order_release );
            return hprec;
        }

        // No HP records available for reuse
        // Allocate and push a new HP record
        hprec = create_thread_data();
        hprec->thread_id_.store( curThreadId, atomics::memory_order_relaxed );

        thread_record* pOldHead = thread_list_.load( atomics::memory_order_relaxed );
        do {
            hprec->next_ = pOldHead;
        } while ( !thread_list_.compare_exchange_weak( pOldHead, hprec, atomics::memory_order_release, atomics::memory_order_acquire ));

        return hprec;
    }

    CDS_EXPORT_API void smr::free_thread_data( smr::thread_record* pRec, bool callHelpScan )
    {
        assert( pRec != nullptr );

        pRec->hazards_.clear();
        scan( pRec );
        if ( callHelpScan )
            help_scan( pRec );
        pRec->thread_id_.store( cds::OS::c_NullThreadId, atomics::memory_order_release );
    }

    CDS_EXPORT_API void smr::detach_all_thread()
    {
        thread_record * pNext = nullptr;
        const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;

        for ( thread_record * hprec = thread_list_.load( atomics::memory_order_relaxed ); hprec; hprec = pNext ) {
            pNext = hprec->next_;
            if ( hprec->thread_id_.load( atomics::memory_order_relaxed ) != nullThreadId ) {
                free_thread_data( hprec, false );
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
            instance().free_thread_data( static_cast<thread_record*>( rec ), true );
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
                if ( pNode->thread_id_.load( atomics::memory_order_relaxed ) != cds::OS::c_NullThreadId ) {
                    thread_hp_storage& hpstg = pNode->hazards_;
                    
                    for ( auto hp = hpstg.begin(), end = hpstg.end(); hp != end; ++hp ) {
                        void * hptr = hp->get( atomics::memory_order_relaxed );
                        if ( hptr ) {
                            dummy_retired.m_p = hptr;
                            retired_ptr* it = std::lower_bound(first_retired, last_retired, dummy_retired, retired_ptr::less);
                            if ( it != last_retired && it->m_p == hptr ) {
                                // Mark retired pointer as guarded
                                it->m_n |= 1;
                            }
                        }
                    }
                }
                pNode = pNode->next_;
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
            if ( pNode->thread_id_.load( std::memory_order_relaxed ) != cds::OS::c_NullThreadId ) {
                for ( size_t i = 0; i < get_hazard_ptr_count(); ++i ) {
                    void * hptr = pNode->hazards_[i].get();
                    if ( hptr )
                        plist.push_back( hptr );
                }
            }
            pNode = pNode->next_;
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
        assert( static_cast<thread_record*>( pThis )->thread_id_.load( atomics::memory_order_relaxed ) == cds::OS::get_current_thread_id());

        CDS_HPSTAT( ++pThis->help_scan_count_ );

        const cds::OS::ThreadId nullThreadId = cds::OS::c_NullThreadId;
        const cds::OS::ThreadId curThreadId = cds::OS::get_current_thread_id();
        for ( thread_record* hprec = thread_list_.load( atomics::memory_order_acquire ); hprec; hprec = hprec->next_ )
        {
            if ( hprec == static_cast<thread_record*>( pThis ))
                continue;

            // If free_ == true then hprec->retired_ is empty - we don't need to see it
            if ( hprec->free_.load( atomics::memory_order_acquire ))
                continue;

            // Owns hprec if it is empty.
            // Several threads may work concurrently so we use atomic technique only.
            {
                cds::OS::ThreadId curOwner = hprec->thread_id_.load( atomics::memory_order_relaxed );
                if ( curOwner == nullThreadId ) {
                    if ( !hprec->thread_id_.compare_exchange_strong( curOwner, curThreadId, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
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
            hprec->free_.store( true, atomics::memory_order_release );
            hprec->thread_id_.store( nullThreadId, atomics::memory_order_release );

            scan( pThis );
        }
    }

    CDS_EXPORT_API void smr::statistics( stat& st )
    {
        st.clear();
#   ifdef CDS_ENABLE_HPSTAT
        for ( thread_record* hprec = thread_list_.load( atomics::memory_order_acquire ); hprec; hprec = hprec->next_ )
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

