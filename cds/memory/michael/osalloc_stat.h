//$$CDS-header$$

#ifndef CDSLIB_MEMORY_MICHAEL_ALLOCATOR_OSALLOC_STAT_H
#define CDSLIB_MEMORY_MICHAEL_ALLOCATOR_OSALLOC_STAT_H

#include <cds/algo/atomic.h>

namespace cds { namespace memory { namespace michael {

    /// Statistics for large  (allocated directly from %OS) block
    struct os_allocated_atomic
    {
        ///@cond
        atomics::atomic<size_t>              nAllocCount         ;   ///< Event count of large block allocation from %OS
        atomics::atomic<size_t>              nFreeCount          ;   ///< Event count of large block deallocation to %OS
        atomics::atomic<unsigned long long>  nBytesAllocated     ;   ///< Total size of allocated large blocks, in bytes
        atomics::atomic<unsigned long long>  nBytesDeallocated   ;   ///< Total size of deallocated large blocks, in bytes

        os_allocated_atomic()
            : nAllocCount(0)
            , nFreeCount(0)
            , nBytesAllocated(0)
            , nBytesDeallocated(0)
        {}
        ///@endcond

        /// Adds \p nSize to nBytesAllocated counter
        void incBytesAllocated( size_t nSize )
        {
            nAllocCount.fetch_add( 1, atomics::memory_order_relaxed);
            nBytesAllocated.fetch_add( nSize, atomics::memory_order_relaxed );
        }

        /// Adds \p nSize to nBytesDeallocated counter
        void incBytesDeallocated( size_t nSize )
        {
            nFreeCount.fetch_add( 1, atomics::memory_order_relaxed );
            nBytesDeallocated.fetch_add( nSize, atomics::memory_order_relaxed );
        }

        /// Returns count of \p alloc and \p alloc_aligned function call (for large block allocated directly from %OS)
        size_t allocCount() const
        {
            return nAllocCount.load(atomics::memory_order_relaxed);
        }

        /// Returns count of \p free and \p free_aligned function call (for large block allocated directly from %OS)
        size_t freeCount() const
        {
            return nFreeCount.load(atomics::memory_order_relaxed);
        }

        /// Returns current value of nBytesAllocated counter
        atomic64u_t allocatedBytes() const
        {
            return nBytesAllocated.load(atomics::memory_order_relaxed);
        }

        /// Returns current value of nBytesAllocated counter
        atomic64u_t deallocatedBytes() const
        {
            return nBytesDeallocated.load(atomics::memory_order_relaxed);
        }
    };

    /// Dummy statistics for large (allocated directly from %OS) block
    /**
        This class does not gather any statistics.
        Class interface is the same as \ref os_allocated_atomic.
    */
    struct os_allocated_empty
    {
    //@cond
        /// Adds \p nSize to nBytesAllocated counter
        void incBytesAllocated( size_t nSize )
        { CDS_UNUSED(nSize); }

        /// Adds \p nSize to nBytesDeallocated counter
        void incBytesDeallocated( size_t nSize )
        { CDS_UNUSED(nSize); }

        /// Returns count of \p alloc and \p alloc_aligned function call (for large block allocated directly from OS)
        size_t allocCount() const
        {
            return 0;
        }

        /// Returns count of \p free and \p free_aligned function call (for large block allocated directly from OS)
        size_t freeCount() const
        {
            return 0;
        }

        /// Returns current value of nBytesAllocated counter
        atomic64u_t allocatedBytes() const
        {
            return 0;
        }

        /// Returns current value of nBytesAllocated counter
        atomic64u_t deallocatedBytes() const
        {
            return 0;
        }
    //@endcond
    };


}}} // namespace cds::memory::michael

#endif  /// CDSLIB_MEMORY_MICHAEL_ALLOCATOR_OSALLOC_STAT_H
