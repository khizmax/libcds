/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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
        uint64_t allocatedBytes() const
        {
            return nBytesAllocated.load(atomics::memory_order_relaxed);
        }

        /// Returns current value of nBytesAllocated counter
        uint64_t deallocatedBytes() const
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
        uint64_t allocatedBytes() const
        {
            return 0;
        }

        /// Returns current value of nBytesAllocated counter
        uint64_t deallocatedBytes() const
        {
            return 0;
        }
    //@endcond
    };


}}} // namespace cds::memory::michael

#endif  /// CDSLIB_MEMORY_MICHAEL_ALLOCATOR_OSALLOC_STAT_H
