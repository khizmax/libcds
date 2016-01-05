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

#ifndef CDSLIB_MEMORY_MICHAEL_OPTIONS_H
#define CDSLIB_MEMORY_MICHAEL_OPTIONS_H

/*
    Options for Michael allocator
    Source:
        [2004] Maged Michael "Scalable Lock-Free Dynamic Memory Allocation"

    Editions:
        2011.01.23 khizmax  Created
*/

#include <cds/opt/options.h>

namespace cds { namespace memory { namespace michael {

    /// Options related for Michael's allocator \ref Heap
    namespace opt {
        using namespace cds::opt;

        /// Option setter specifies system topology
        /**
            See cds::OS::Win32::topology for interface example.

            Default type: \p cds::OS::topology selects appropriate implementation for target system.
        */
        template <typename TOPOLOGY>
        struct sys_topology {
            //@cond
            template<class BASE> struct pack: public BASE
            {
                typedef TOPOLOGY sys_topology;
            };
            //@endcond
        };

        /// Option setter specifies system heap for large blocks
        /**
            If the block size requested is more that Michael's allocator upper limit
            then an allocator provided by \p system_heap option is called.
            By default, Michael's allocator can maintain blocks up to 64K bytes length;
            for blocks larger than 64K the allocator defined by this option is used.

            Available \p HEAP implementations:
                - malloc_heap
        */
        template <typename HEAP>
        struct system_heap
        {
            //@cond
            template<class BASE> struct pack: public BASE
            {
                typedef HEAP system_heap;
            };
            //@endcond
        };

        /// Option setter specifies internal aligned heap
        /**
            This heap is used by Michael's allocator for obtaining aligned memory.

            Available \p HEAP implementations:
                - aligned_malloc_heap
        */
        template <typename HEAP>
        struct aligned_heap {
            //@cond
            template<class BASE> struct pack: public BASE
            {
                typedef HEAP aligned_heap;
            };
            //@endcond
        };

        /// Option setter specifies page heap
        /**
            This heap is used by Michael's allocator for superblock allocation.
            The size of superblock is:
                - 64K - for small blocks
                - 1M - for other blocks

            Available \p HEAP implementations:
                - page_allocator
                - page_cached_allocator
        */
        template <typename HEAP>
        struct page_heap {
            //@cond
            template<class BASE> struct pack: public BASE
            {
                typedef HEAP page_heap;
            };
            //@endcond
        };

        /// Option setter specifies size-class selector
        /**
            The size-class selector determines the best size-class for requested block size,
            i.e. it specifies allocation granularity.
            In fact, it selects superblock descriptor within processor heap.

            Available \p Type implementation:
                - default_sizeclass_selector
        */
        template <typename Type>
        struct sizeclass_selector {
            //@cond
            template<class BASE> struct pack: public BASE
            {
                typedef Type sizeclass_selector;
            };
            //@endcond
        };

        /// Option setter specifies free-list of superblock descriptor
        /**
            Available \p Type implementations:
                - free_list_locked
        */
        template <typename Type>
        struct free_list {
            //@cond
            template<class BASE> struct pack: public BASE
            {
                typedef Type free_list;
            };
            //@endcond
        };

        /// Option setter specifies partial list of superblocks
        /**
            Available \p Type implementations:
                - partial_list_locked
        */
        template <typename Type>
        struct partial_list {
            //@cond
            template<class BASE> struct pack: public BASE
            {
                typedef Type partial_list;
            };
            //@endcond
        };

        /// Option setter for processor heap statistics
        /**
            The option specifies a type for gathering internal processor heap statistics.
            The processor heap statistics is gathered on per processor basis.
            Large memory block (more than 64K) allocated directly from OS does not fall into these statistics.
            For OS-allocated memory block see \ref os_allocated_stat option.

            Available \p Type implementations:
                - \ref procheap_atomic_stat
                - \ref procheap_empty_stat

            For interface of type \p Type see \ref procheap_atomic_stat.
        */
        template <typename Type>
        struct procheap_stat {
            //@cond
            template <class BASE> struct pack: public BASE
            {
                typedef Type procheap_stat;
            };
            //@endcond
        };

        /// Option setter for OS-allocated memory
        /**
            The option specifies a type for gathering internal statistics of
            large (OS-allocated) memory blocks that is too big to maintain by Michael's heap
            (with default \ref sizeclass_selector, the block that large than 64K is not
            maintained by Michael's heap and passed directly to system allocator).

            Note that OS-allocated memory statistics does not include memory allocation
            for heap's internal purposes. Only direct call of \p alloc or \p alloc_aligned
            for large memory block is counted.

            Available \p Type implementations:
                - \ref os_allocated_atomic
                - \ref os_allocated_empty
        */
        template <typename Type>
        struct os_allocated_stat {
            //@cond
            template <class BASE> struct pack: public BASE
            {
                typedef Type os_allocated_stat;
            };
            //@endcond
        };

        /// Option setter for bounds checking
        /**
            This option defines a strategy to check upper memory boundary of allocated blocks.
            \p Type defines a class for bound checking with following interface:

            \code
            class bound_checker
            {
            public:
                enum {
                    trailer_size = numeric_const
                };

                void make_trailer( void * pStartArea, void * pEndBlock, size_t nAllocSize );
                bool check_bounds( void * pStartArea, void * pEndBlock, size_t nBlockSize );
            }
            \endcode

            Before allocating a memory block of size N, the heap adds the \p trailer_size to N and really it
            allocates N + trailer_size bytes. Then, the heap calls \p make_trailer function of bound checker with arguments:
                - \p pStartArea - start of allocated block
                - \p pEndBlock - the first byte after really allocated block; \code pEndBlock - pStartArea >= N + trailer_size \endcode
                - \p nAllocSize -  requested size in bytes (i.e. N)
            So, \p make_trailer function can place some predefined value called bound mark of any type, for example, int64,
            on address pStartArea + nAllocSize, and store real allocated block size N to pEndBlock - sizeof(size_t).
            In this example, \p trailer_size constant is equal sizeof(int64) + sizeof(size_t).

            Before the memory block previously allocated is deallocating, the \p check_bounds function is called.
            The function has similar signature:
                - \p pStartArea - start of allocated block (like \p make_trailer fist argument)
                - \p pEndBlock - the first byte after allocated block (like \p make_trailer second argument)
                - \p nBlockSize - real allocated block size, not equal to \p nAllocSize argument of \p make_trailer

            The function can:
                - calculate real allocated block size: \code N = *reinterpret_cast<size_t>(pEndBlock - sizeof(size_t)) \endcode
                - check whether the bound mark is unchanged: \code *reinterpret_cast<int64>(pStartArea + N) == bound_mark \endcode
                - if it is not equal - make assertion

            The library provides the following predefined bound checkers, i.e they are possible values of \p Type
            template argument:
                \li cds::opt::none - no bound checking is performed (default)
                \li michael::debug_bound_checking - an assertion is thrown when memory bound violation is detected.
                    This option is acceptable only in debug mode. For release mode it is equal to cds::opt::none.
                \li michael::strong_bound_checking - an assertion is thrown in debug mode if memory bound violation is detected;
                    an exception is thrown in release mode.
        */
        template <typename Type>
        struct check_bounds {
            //@cond
            template <class BASE> struct pack: public BASE
            {
                typedef Type check_bounds;
            };
            //@endcond
        };
    }

}}} // namespace cds::memory::michael

#endif // #ifndef CDSLIB_MEMORY_MICHAEL_OPTIONS_H
