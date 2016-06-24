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

#ifndef CDSLIB_MEMORY_MICHAEL_ALLOCATOR_TMPL_H
#define CDSLIB_MEMORY_MICHAEL_ALLOCATOR_TMPL_H

/*
    Michael allocator implementation
    Source:
        [2004] Maged Michael "Scalable Lock-Free Dynamic Memory Allocation"

    Editions:
    2011.09.07 khizmax  Optimization: small page (about 64K) is allocated by Heap::alloc call.
                        This optimization allows to allocate system memory more regularly,
                        in blocks of 1M that leads to less memory fragmentation.
    2011.01.02 khizmax  Created
*/

#include <stdlib.h>
#include <mutex>        // unique_lock
#include <cds/init.h>
#include <cds/memory/michael/options.h>
#include <cds/memory/michael/bound_check.h>
#include <cds/memory/michael/procheap_stat.h>
#include <cds/memory/michael/osalloc_stat.h>

#include <cds/os/topology.h>
#include <cds/os/alloc_aligned.h>
#include <cds/sync/spinlock.h>
#include <cds/details/type_padding.h>
#include <cds/details/marked_ptr.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>
#include <cds/user_setup/cache_line.h>
#include <cds/details/lib.h>

#include <boost/intrusive/list.hpp>

namespace cds {
    /// Memory-related algorithms: allocators etc.
namespace memory {
    /// Michael's allocator (class Heap)
    /**
        \par Source
            \li [2004] M.Michael "Scalable Lock-free Dynamic Memory Allocation"

        This namespace declares the main class Heap and a lot of helper classes.
    */
namespace michael {

    /// Size class
    struct size_class {
        unsigned int    nBlockSize  ;   ///< block size in bytes
        unsigned int    nSBSize     ;   ///< superblock size (64K or 1M)
        unsigned int    nCapacity   ;   ///< superblock capacity (nSBSize / nBlockSize)
        unsigned int    nSBSizeIdx  ;   ///< internal superblock size index (page index)
    };

    /// %Heap based on system \p malloc and \p free functions
    struct malloc_heap
    {
        /// Allocates memory block of \p nSize bytes (\p malloc wrapper)
        static void * alloc( size_t nSize )
        {
            void * p = ::malloc( nSize );
            return p;
        }
        /// Returning memory block to the system (\p free wrapper)
        static void free( void * p )
        {
            ::free( p );
        }
    };

    /// %Heap based on system provided aligned \p malloc and \p free functions
    struct aligned_malloc_heap
    {
        /// Allocates aligned memory block of \p nSize bytes with alignment \p nAlignment
        static void * alloc( size_t nSize, size_t nAlignment  )
        {
            return cds::OS::aligned_malloc( nSize, nAlignment );
        }
        /// Frees aligned memory block \p p that has been previosly allocated by \ref alloc call
        static void free( void * p )
        {
            cds::OS::aligned_free( p );
        }
    };

    /// Page heap based on \p Heap
    /**
        Page heap can allocate memory by page-sized block only.
        \p Heap may be any heap that provides interface like \ref malloc_heap.

        This class is one of available implementation of opt::page_heap option.
    */
    template <class Heap = malloc_heap>
    class page_allocator: public Heap
    {
        //@cond
        typedef Heap base_class;
        size_t  m_nPageSize;
        //@endcond

    public:
        /// Initializes heap
        page_allocator(
            size_t nPageSize    ///< page size in bytes
        )
            : m_nPageSize( nPageSize )
        {}

        /// Allocate new page
        void * alloc()
        {
            return base_class::alloc( m_nPageSize );
        }

        /// Free page \p pPage
        void free( void * pPage )
        {
            base_class::free( pPage );
        }
    };

    /// Page cacheable heap
    /**
        To improve performance this allocator maintains small list of free pages.
        Page heap can allocate memory by page-sized block only.

        Template parameters:
            \li \p FreeListCapacity - capacity of free-list, default value is 64 page
            \li \p Heap may be any heap that provides interface like \ref malloc_heap.

        This class is one of available implementation of opt::page_heap option.
    */
    template <size_t FreeListCapacity = 64, class Heap = malloc_heap>
    class page_cached_allocator: public page_allocator<Heap>
    {
        //@cond
        typedef page_allocator<Heap> base_class;

#ifdef _DEBUG
        struct make_null_ptr {
            void operator ()(void *& p)
            {
                p = nullptr;
            }
        };
#endif
        struct free_list_traits : public cds::container::vyukov_queue::traits
        {
            typedef opt::v::initialized_static_buffer<void *, FreeListCapacity> buffer;
#ifdef _DEBUG
            typedef make_null_ptr value_cleaner;
#endif
        };
        typedef container::VyukovMPMCCycleQueue< void *, free_list_traits > free_list;

        free_list   m_FreeList;
        //@endcond

    public:
        /// Initializes heap
        page_cached_allocator(
            size_t nPageSize    ///< page size in bytes
        )
            : base_class( nPageSize )
            , m_FreeList( FreeListCapacity )
        {}

        //@cond
        ~page_cached_allocator()
        {
            void * pPage;
            while ( m_FreeList.pop(pPage) )
                base_class::free( pPage );
        }
        //@endcond

        /// Allocate new page
        void * alloc()
        {
            void * pPage;
            if ( !m_FreeList.pop( pPage ) )
                pPage = base_class::alloc();
            return pPage;
        }

        /// Free page \p pPage
        void free( void * pPage )
        {
            if ( !m_FreeList.push( pPage ))
                base_class::free( pPage );
        }
    };

    /// Implementation of opt::sizeclass_selector option
    /**
        Default size-class selector can manage memory blocks up to 64K.
    */
    class CDS_EXPORT_API default_sizeclass_selector
    {
        //@cond
        /// Count of different size-classes
        static const size_t c_nSizeClassCount = 63;

        /// Max block size
        static const size_t c_nMaxBlockSize = 64 * 1024;

        /// Page size of type 0 (64K)
        static const unsigned int c_nPage64K = 64 * 1024 - 32;

        /// Page size of type 1 (1M)
        static const unsigned int c_nPage1M = 1024 * 1024;

        static CDS_DATA_ALIGNMENT(128) unsigned int const m_szClassBounds[c_nSizeClassCount];
        static size_class const m_szClass[c_nSizeClassCount];
        static unsigned char const m_szClassMap[];
        //@endcond
    public:
        /// Type of size-class index
        typedef unsigned int sizeclass_index;

#ifdef _DEBUG
        default_sizeclass_selector();
#endif

        /// "No size class" index
        static const sizeclass_index c_nNoSizeClass = (unsigned int) (0 - 1);

        /// Returns size-class count
        static sizeclass_index size()
        {
            return c_nSizeClassCount;
        }

        /// Returns page size in bytes for given page type \p nPageType
        static size_t page_size(size_t nPageType )
        {
            switch (nPageType) {
            case 0:
                return c_nPage64K;
            case 1:
                return c_nPage1M;
            default:
                assert(false)   ;   // anything forgotten?..
            }
            return c_nPage1M;
        }

        /// Returns count of page size-class
        /**
            This class supports pages of two types: 64K page for small objects and 1M page for other objects.
        */
        static size_t pageTypeCount()
        {
            return 2;
        }

        /// Returns size-class index for \p nSize
        /**
            For large blocks that cannot be allocated by Michael's allocator
            the function must return -1.
        */
        static sizeclass_index find( size_t nSize )
        {
            if ( nSize > c_nMaxBlockSize ) {
                // Too large block - allocate from system
                return c_nNoSizeClass;
            }
            sizeclass_index szClass = m_szClassMap[ (nSize + 15) / 16 ];
            assert( nSize <= m_szClassBounds[ szClass ] );
            assert( szClass == 0 || m_szClassBounds[ szClass - 1] < nSize );

            return szClass;
        }

        /// Gets details::size_class struct for size-class index \p nIndex
        static const size_class * at( sizeclass_index nIndex )
        {
            assert( nIndex < size() );
            return m_szClass + nIndex;
        }
    };

    //@cond
    namespace details {
        struct free_list_tag;
        typedef boost::intrusive::list_base_hook< boost::intrusive::tag< free_list_tag > >  free_list_locked_hook;

        struct partial_list_tag;
        typedef boost::intrusive::list_base_hook< boost::intrusive::tag< partial_list_tag > >  partial_list_locked_hook;

        struct intrusive_superblock_desc: public free_list_locked_hook, partial_list_locked_hook
        {};
    }
    //@endcond

    /// List of free superblock descriptor
    /**
        This class is a implementation of \ref opt::free_list option
    */
    template <class Lock, class T = details::intrusive_superblock_desc>
    class free_list_locked: public boost::intrusive::list<T, boost::intrusive::base_hook<details::free_list_locked_hook> >
    {
        //@cond
        typedef boost::intrusive::list<T, boost::intrusive::base_hook<details::free_list_locked_hook> > base_class;
    public:
        typedef details::free_list_locked_hook item_hook;
        typedef Lock lock_type;
    protected:
        typedef std::unique_lock<lock_type>   auto_lock;

        mutable lock_type   m_access;
        //@endcond

    public:
        /// Rebinds to other item type \p T2
        template <class T2>
        struct rebind {
            typedef free_list_locked<Lock, T2>    other   ;   ///< rebind result
        };

    public:
        /// Push superblock descriptor to free-list
        void push( T * pDesc )
        {
            assert( base_class::node_algorithms::inited( static_cast<item_hook *>(pDesc) ) );
            auto_lock al(m_access);
            base_class::push_back( *pDesc );
        }

        /// Pop superblock descriptor from free-list
        T *   pop()
        {
            auto_lock al(m_access);
            if ( base_class::empty() )
                return nullptr;
            T& rDesc = base_class::front();
            base_class::pop_front();
            assert( base_class::node_algorithms::inited( static_cast<item_hook *>(&rDesc) ) );
            return &rDesc;
        }

        /// Returns current count of superblocks in free-list
        size_t  size() const
        {
            auto_lock al(m_access);
            return base_class::size();
        }
    };

    /// List of partial filled superblock descriptor
    /**
        This class is a implementation of \ref opt::partial_list option
    */
    template <class Lock, class T = details::intrusive_superblock_desc>
    class partial_list_locked: public boost::intrusive::list<T, boost::intrusive::base_hook<details::partial_list_locked_hook> >
    {
        //@cond
        typedef boost::intrusive::list<T, boost::intrusive::base_hook<details::partial_list_locked_hook> > base_class;
    public:
        typedef details::partial_list_locked_hook item_hook;
        typedef Lock    lock_type;
    protected:
        typedef std::unique_lock<lock_type>   auto_lock;

        mutable lock_type   m_access;
        //@endcond

    public:
        /// Rebinds to other item type \p T2
        template <class T2>
        struct rebind {
            typedef partial_list_locked<Lock, T2>    other   ;   ///< rebind result
        };

    public:
        /// Push a superblock \p pDesc to the list
        void    push( T * pDesc )
        {
            auto_lock al( m_access );
            assert( base_class::node_algorithms::inited( static_cast<item_hook *>(pDesc) ) );
            base_class::push_back( *pDesc );
        }

        /// Pop superblock from the list
        T * pop()
        {
            auto_lock al( m_access );
            if ( base_class::empty() )
                return nullptr;
            T& rDesc = base_class::front();
            base_class::pop_front();
            assert( base_class::node_algorithms::inited( static_cast<item_hook *>(&rDesc) ) );
            return &rDesc;
        }

        /// Removes \p pDesc descriptor from the free-list
        bool unlink( T * pDesc )
        {
            assert(pDesc != nullptr);
            auto_lock al( m_access );
            // !inited(pDesc) is equal to "pDesc is being linked to partial list"
            if ( !base_class::node_algorithms::inited( static_cast<item_hook *>(pDesc) ) ) {
                base_class::erase( base_class::iterator_to( *pDesc ) );
                return true;
            }
            return false;
        }

        /// Count of element in the list
        size_t size() const
        {
            auto_lock al( m_access );
            return base_class::size();
        }
    };

    /// Summary processor heap statistics
    /**
        Summary heap statistics for use with Heap::summaryStat function.
    */
    struct summary_stat
    {
        size_t      nAllocFromActive    ;  ///< Event count of allocation from active superblock
        size_t      nAllocFromPartial   ;  ///< Event count of allocation from partial superblock
        size_t      nAllocFromNew       ;  ///< Event count of allocation from new superblock
        size_t      nFreeCount          ;  ///< Count of \p free function call
        size_t      nPageAllocCount     ;  ///< Count of page (superblock) allocated
        size_t      nPageDeallocCount   ;  ///< Count of page (superblock) deallocated
        size_t      nDescAllocCount     ;  ///< Count of superblock descriptors
        size_t      nDescFull           ;  ///< Count of full superblock
        uint64_t    nBytesAllocated     ;  ///< Count of allocated bytes (for heap managed memory blocks)
        uint64_t    nBytesDeallocated   ;  ///< Count of deallocated bytes (for heap managed memory blocks)

        size_t      nSysAllocCount      ;  ///< Count of \p alloc and \p alloc_aligned function call (for large memory blocks that allocated directly from OS)
        size_t      nSysFreeCount       ;  ///< Count of \p free and \p free_aligned function call (for large memory blocks that allocated directly from OS)
        uint64_t    nSysBytesAllocated  ;  ///< Count of allocated bytes (for large memory blocks that allocated directly from OS)
        int64_t     nSysBytesDeallocated;  ///< Count of deallocated bytes (for large memory blocks that allocated directly from OS)

        // Internal contention indicators
        /// CAS failure counter for updating active field of active block of \p alloc_from_active Heap internal function
        /**
            Contention indicator. The less value is better
        */
        size_t      nActiveDescCASFailureCount;
        /// CAS failure counter for updating active field of active block of \p alloc_from_active Heap internal function
        /**
            Contention indicator. The less value is better
        */
        size_t      nActiveAnchorCASFailureCount;
        /// CAS failure counter for updating anchor field of partial block of \p alloc_from_partial Heap internal function
        /**
            Contention indicator. The less value is better
        */
        size_t      nPartialDescCASFailureCount;
        /// CAS failure counter for updating anchor field of partial block of \p alloc_from_partial Heap internal function
        /**
            Contention indicator. The less value is better
        */
        size_t      nPartialAnchorCASFailureCount;


    public:
        /// Constructs empty statistics. All counters are zero.
        summary_stat()
        {
            clear();
        }

        /// Difference statistics
        /**
            This operator computes difference between \p *this and \p stat and places the difference to \p this.
            Returns \p *this;
        */
        summary_stat& operator -=( const summary_stat& stat )
        {
            nAllocFromActive    -= stat.nAllocFromActive;
            nAllocFromPartial   -= stat.nAllocFromPartial;
            nAllocFromNew       -= stat.nAllocFromNew;
            nFreeCount          -= stat.nFreeCount;
            nPageAllocCount     -= stat.nPageAllocCount;
            nPageDeallocCount   -= stat.nPageDeallocCount;
            nDescAllocCount     -= stat.nDescAllocCount;
            nDescFull           -= stat.nDescFull;
            nBytesAllocated     -= stat.nBytesAllocated;
            nBytesDeallocated   -= stat.nBytesDeallocated;

            nSysAllocCount      -= stat.nSysAllocCount;
            nSysFreeCount       -= stat.nSysFreeCount;
            nSysBytesAllocated  -= stat.nSysBytesAllocated;
            nSysBytesDeallocated -= stat.nSysBytesDeallocated;

            nActiveDescCASFailureCount      -= stat.nActiveDescCASFailureCount;
            nActiveAnchorCASFailureCount    -= stat.nActiveAnchorCASFailureCount;
            nPartialDescCASFailureCount     -= stat.nPartialDescCASFailureCount;
            nPartialAnchorCASFailureCount   -= stat.nPartialAnchorCASFailureCount;

            return *this;
        }

        /// Clears statistics
        /**
            All counters are set to zero.
        */
        void clear()
        {
            memset( this, 0, sizeof(*this));
        }

        //@cond
        template <typename Stat>
        summary_stat& add_procheap_stat( const Stat& stat )
        {
            nAllocFromActive    += stat.allocFromActive();
            nAllocFromPartial   += stat.allocFromPartial();
            nAllocFromNew       += stat.allocFromNew();
            nFreeCount          += stat.freeCount();
            nPageAllocCount     += stat.blockAllocated();
            nPageDeallocCount   += stat.blockDeallocated();
            nDescAllocCount     += stat.descAllocCount();
            nDescFull           += stat.descFull();
            nBytesAllocated     += stat.allocatedBytes();
            nBytesDeallocated   += stat.deallocatedBytes();

            nActiveDescCASFailureCount      += stat.activeDescCASFailureCount();
            nActiveAnchorCASFailureCount    += stat.activeAnchorCASFailureCount();
            nPartialDescCASFailureCount     += stat.partialDescCASFailureCount();
            nPartialAnchorCASFailureCount   += stat.partialAnchorCASFailureCount();

            return *this;
        }

        template <typename Stat>
        summary_stat& add_heap_stat( const Stat& stat )
        {
            nSysAllocCount      += stat.allocCount();
            nSysFreeCount       += stat.freeCount();

            nSysBytesAllocated  += stat.allocatedBytes();
            nSysBytesDeallocated+= stat.deallocatedBytes();

            return *this;
        }
        //@endcond
    };

    /// Michael's allocator
    /**
        This class provides base functionality for Michael's allocator. It does not provide
        the interface described by \p std::allocator, therefore, we name it as a heap, not as an allocator.
        The heap interface is closer to semantics of \p malloc / \p free system functions.
        The heap supports allocation of aligned and unaligned data.

        The algorithm is based on simplified version of
            \li [2004] M.Michael "Scalable Lock-free Dynamic Memory Allocation"

        that, in turn, is concurrent version of well-known Hoard allocator developed by Emery Berger, see
            \li [2002] Emery Berger "Memory Management for High-Performance Application", PhD thesis

        This is powerful, scalable, fully customizable heap with fast-path without any locks
        that has been developed specifically for multi-threading.
        With opt:sys_topology you can set as many allocation arena ("processor heap") as you need.
        You can manually bound any your thread to any arena ("processor"). With opt::sizeclass_selector option you can manage
        allocation granularity. With opt::page_heap you can utilize any OS-provided features for page allocation
        like \p mmap, \p VirtualAlloc etc. The heap can gather internal statistics that helps you to tune your application.
        The opt::check_bounds feature can help you to find a memory buffer overflow.

        Brief algorithm description from Michael's work:

        Large blocks (greater than 64K) are allocated directly from the OS and freed directly to the OS. For smaller block sizes,
        the heap is composed of large superblocks (64 KB or 1MB size). Each superblock is divided into multiple equal-sized blocks.
        Superblocks are distributed among size classes based on their block sizes. Each size class contains multiple processor
        heaps proportional to the number of processors in the system. A processor heap contains at most one active superblock.
        An active superblock contains one or more blocks available for reservation that are guaranteed to be available to threads
        that reach them through the header of the processor heap. Each superblock is associated with a descriptor. Each allocated
        block contains a prefix (8 bytes) that points to the descriptor of its superblock. On the first call to malloc, the static
        structures for the size classes and processor heaps (about 16 KB for a 16 processor machine) are allocated and initialized
        in a lock-free manner.

        Malloc starts by identifying the appropriate processor heap, based on the requested block size and the identity of
        the calling thread. Typically, the heap already has an active superblock with blocks available for reservation. The thread
        atomically reads a pointer to the descriptor of the active superblock and reserves a block. Next, the thread atomically
        pops a block from that superblock and updates its descriptor. A typical free pushes the freed block into the list of
        available blocks of its original superblock by atomically updating its descriptor.

        <b>Constraint</b>: one superblock may contain up to 2048 block. This restriction imposes a restriction on the maximum
        superblock size.

        Available \p Options:
        - \ref opt::sys_topology - class that describes system topology needed for allocator.
            Default is \p cds::OS::topology (see cds::OS::Win32::topology for interface description)
        - \ref opt::system_heap - option setter for an allocator for large blocks that is used for direct allocation from OS.
            Default is \ref malloc_heap.
        - \ref opt::aligned_heap - option setter for a heap used for internal aligned memory management.
            Default is \ref aligned_malloc_heap
        - \ref opt::page_heap - option setter for a heap used for page (superblock) allocation of 64K/1M size.
            Default is \ref page_cached_allocator
        - \ref opt::sizeclass_selector - option setter for a class used to select appropriate size-class
            for incoming allocation request.
            Default is \ref default_sizeclass_selector
        - \ref opt::free_list - option setter for a class to manage a list of free superblock descriptors
            Default is \ref free_list_locked
        - \ref opt::partial_list - option setter for a class to manage a list of partial filled superblocks
            Default is \ref partial_list_locked
        - \ref opt::procheap_stat - option setter for a class to gather internal statistics for memory allocation
            that is maintained by the heap.
            Default is \ref procheap_empty_stat
        - \ref opt::os_allocated_stat - option setter for a class to gather internal statistics for large block
            allocation. Term "large block" is specified by the size-class selector (see \ref opt::sizeclass_selector)
            and it is 64K for \ref default_sizeclass_selector. Any block that is large that 64K is allocated from
            OS directly. \p os_allocated_stat option is set a class to gather statistics for large blocks.
            Default is \ref os_allocated_empty
        - \ref opt::check_bounds - a bound checker.
            Default is no bound checker (cds::opt::none)

        \par Usage:
        The heap is the basic building block for your allocator or <tt> operator new</tt> implementation.

        \code
        #include <cds/memory/michael/allocator.h>

        // Heap with explicitly defined options:
        cds::memory::michael::Heap<
            opt::aligned_heap< aligned_malloc_heap >,
            opt::page_heap< page_cached_allocator<16, malloc_heap> >
        >   myHeap;

        // Heap with default options:
        cds::memory::michael::Heap<>    myDefHeap;
        \endcode

        \par How to make std-like allocator

        There are serious differencies of heap and <tt>std::allocator</tt> interface:
            - Heap is stateful, and \p std::allocator is stateless.
            - Heap has much more template parameters than \p std::allocator
            - Heap has low-level interface for memory allocating only unlike the allocator
                interface that can construct/destroy objects of any type T.

        To convert heap interface into \p std::allocator -like interface you should:
            - Declare object of class cds::memory::michael::Heap specifying the necessary
                template parameters; this is usually static object
            - Create a class with \p std::allocator interface that uses the function of heap.
        \code
        #include <cds/memory/michael/allocator.h>

        template <class T>
        class MichaelAllocator
        {
            typedef std::allocator<T>               std_allocator;
            typedef cds::memory::michael::Heap<>    michael_heap;

            // Michael heap static object
            static michael_heap     s_Heap;
        public:
            // Declare typedefs from std::allocator
            typedef typename std_allocator::const_pointer   const_pointer;
            typedef typename std_allocator::pointer         pointer;
            typedef typename std_allocator::const_reference const_reference;
            typedef typename std_allocator::reference       reference;
            typedef typename std_allocator::difference_type difference_type;
            typedef typename std_allocator::size_type       size_type;
            typedef typename std_allocator::value_type      value_type;

            // Allocation function
            pointer allocate( size_type _Count, const void* _Hint )
            {
                return reinterpret_cast<pointer>( s_Heap.alloc( sizeof(T) * _Count ));
            }

            // Deallocation function
            void deallocate( pointer _Ptr, size_type _Count )
            {
                s_Heap.free( _Ptr );
            }

            // Other std::allocator specific functions: address, construct, destroy, etc.
            ...

            // Rebinding allocator to other type
            template <class _Other>
            struct rebind {
                typedef MichaelAllocator<_Other> other;
            };
        };

        // In .cpp file:
        MichaelAllocator::michael_heap MichaelAllocator::s_Heap;

        \endcode
    */
    template <typename... Options>
    class Heap {
    protected:

        //@cond
        static const unsigned int c_nAlignment = cds::c_nCacheLineSize;
        static const unsigned int c_nDefaultBlockAlignment = 8;

        struct default_options {
            typedef cds::OS::topology           sys_topology;
            typedef malloc_heap                 system_heap;
            typedef page_cached_allocator<>     page_heap;
            typedef aligned_malloc_heap         aligned_heap;
            typedef default_sizeclass_selector  sizeclass_selector;
            typedef free_list_locked<cds::sync::spin>    free_list;
            typedef partial_list_locked<cds::sync::spin> partial_list;
            typedef procheap_empty_stat         procheap_stat;
            typedef os_allocated_empty          os_allocated_stat;
            typedef cds::opt::none              check_bounds;
        };
        //@endcond

    protected:
        //@cond
        typedef typename opt::make_options<default_options, Options...>::type   options;
        //@endcond

        //@cond
        typedef unsigned char   byte;
        //@endcond
    public:
        typedef typename options::sys_topology          sys_topology        ;   ///< effective system topology
        typedef typename options::system_heap           system_heap         ;   ///< effective system heap
        typedef typename options::aligned_heap          aligned_heap        ;   ///< effective aligned heap
        typedef typename options::sizeclass_selector    sizeclass_selector  ;   ///< effective sizeclass selector
        typedef typename options::page_heap             page_heap           ;   ///< effective page heap
        typedef typename options::procheap_stat         procheap_stat       ;   ///< effective processor heap statistics
        typedef typename options::os_allocated_stat     os_allocated_stat   ;   ///< effective OS-allocated memory statistics
        typedef details::bound_checker_selector< typename options::check_bounds >    bound_checker   ;  ///< effective bound checker

        // forward declarations
        //@cond
        struct superblock_desc;
        struct processor_heap_base;
        struct processor_desc;
        //@endcond

        /// Superblock states
        /**
            A superblock can be in one of four states: \p ACTIVE, \p FULL,
            \p PARTIAL, or \p EMPTY. A superblock is \p ACTIVE if it is the active
            superblock in a heap, or if a thread intends to try to install it
            as such. A superblock is \p FULL if all its blocks are either allocated
            or reserved. A superblock is \p PARTIAL if it is not \p ACTIVE
            and contains unreserved available blocks. A superblock is
            \p EMPTY if all its blocks are free and it is not \p ACTIVE.
        */
        enum superblock_state {
            SBSTATE_ACTIVE  = 0,    ///< superblock is active
            SBSTATE_FULL    = 1,    ///< superblock is full
            SBSTATE_PARTIAL = 2,    ///< superblock is partially allocated
            SBSTATE_EMPTY   = 3     ///< superblock is empty and may be freed
        };

        static const size_t c_nMaxBlockInSuperBlock = 1024 * 2  ;   ///< Max count of blocks in superblock (2 ** 11)

        /// Anchor of the superblock descriptor. Updated by CAS
        struct anchor_tag {
            unsigned long long  avail:11    ;   ///< index of first available block in the superblock
            unsigned long long  count:11    ;   ///< number of unreserved blocks in the superblock
            unsigned long long  state: 2    ;   ///< state of the superblock (see \ref superblock_state enum)
            unsigned long long    tag:40    ;   ///< ABA prevention tag
        };

        /// Superblock descriptor
        struct superblock_desc
            : public options::free_list::item_hook
            , public options::partial_list::item_hook
        {
            atomics::atomic<anchor_tag>          anchor      ;   ///< anchor, see \ref anchor_tag
            byte *              pSB         ;   ///< ptr to superblock
            processor_heap_base * pProcHeap ;   ///< pointer to owner processor heap
            unsigned int        nBlockSize  ;   ///< block size in bytes
            unsigned int        nCapacity   ;   ///< superblock size/block size

            //@cond
            superblock_desc()
                : pSB(nullptr)
                , pProcHeap( nullptr )
            {}
            //@endcond
        };

        //@cond
        typedef typename options::free_list::template rebind<superblock_desc>::other    free_list;
        typedef typename options::partial_list::template rebind<superblock_desc>::other partial_list;
        //@endcond

#if CDS_BUILD_BITS == 32
        /// Allocated block header
        /**
            Each allocated block has 8-byte header.
            The header contains pointer to owner superblock descriptor and the redirection flag.
            If the block has been allocated by \ref alloc, then the redirection flag is 0 and the block's structure is:
            \code
                +---------------+
                | blockHeader   |   [8 byte] pointer to owner superblock (flag=0)
                +---------------+
                |               | <- memory allocated
                |   memory      |
                |               |
                +---------------+
            \endcode
            If the block has been allocated by \ref alloc_aligned, then it is possible that pointer returned must be aligned.
            In this case the redirection flag is 1 and the block's structure is:
            \code
                +---------------+
            +-> | blockHeader   |   [8 byte] pointer to owner superblock (flag=0)
            |   +---------------+
            |   |   padding     |
            |   |   (unused)    |
            |   |               |
            |   +---------------+
            +-- | blockHeader   |   [8 byte] pointer to block head (flag=1)
                +---------------+
                |               | <- memory allocated
                |   memory      |
                |               |
                +---------------+
            \endcode
        */
        class block_header
        {
        //@cond
            enum {
                bitAligned = 1,
                bitOSAllocated = 2
            };

            union {
                superblock_desc *   pDesc       ;   // pointer to superblock descriptor
                uint32_t         nSize       ;   // block size (allocated form OS)
            };
            uint32_t         nFlags;

        public:
            void  set( superblock_desc * pdesc, uint32_t isAligned )
            {
                pDesc = pdesc;
                nFlags = isAligned ? bitAligned : 0;
            }

            superblock_desc * desc()
            {
                assert( (nFlags & bitOSAllocated) == 0 );
                return (nFlags & bitAligned) ? reinterpret_cast<block_header *>( pDesc )->desc() : pDesc;
            }

            block_header * begin()
            {
                return (nFlags & bitAligned) ? reinterpret_cast<block_header *>(pDesc) : this;
            }

            bool isAligned() const
            {
                return (nFlags & bitAligned) != 0;
            }

            bool isOSAllocated() const
            {
                return (nFlags & bitOSAllocated) != 0;
            }

            void setOSAllocated( size_t sz )
            {
                nSize = sz;
                nFlags = bitOSAllocated;
            }

            size_t getOSAllocSize() const
            {
                assert( isOSAllocated() );
                return nSize;
            }

        //@endcond
        };
#elif CDS_BUILD_BITS == 64
        //@cond
        class block_header
        {
            enum {
                bitAligned = 1,
                bitOSAllocated = 2
            };
            typedef cds::details::marked_ptr<superblock_desc, bitAligned|bitOSAllocated> marked_desc_ptr;
            // If bitOSAllocated is set the pDesc contains size of memory block
            // allocated from OS
            marked_desc_ptr     pDesc;
        public:
            void  set( superblock_desc * pdesc, uint32_t isAligned )
            {
                pDesc = marked_desc_ptr( pdesc, isAligned );
            }

            superblock_desc * desc()
            {
                assert( !isOSAllocated() );
                return (pDesc.bits() & bitAligned) ? reinterpret_cast<block_header *>( pDesc.ptr() )->desc() : pDesc.ptr();
            }

            block_header * begin()
            {
                return (pDesc.bits() & bitAligned) ? reinterpret_cast<block_header *>( pDesc.ptr() ) : this;
            }

            bool isAligned() const
            {
                return (pDesc.bits() & bitAligned) != 0;
            }

            bool isOSAllocated() const
            {
                return (pDesc.bits() & bitOSAllocated) != 0;
            }

            void setOSAllocated( size_t nSize )
            {

                pDesc = marked_desc_ptr( reinterpret_cast<superblock_desc *>(nSize << 2), bitOSAllocated );
            }

            size_t getOSAllocSize() const
            {
                assert( isOSAllocated() );
                return reinterpret_cast<uintptr_t>( pDesc.ptr() ) >> 2;
            }

        };
        //@endcond
#else
#       error "Unexpected value of CDS_BUILD_BITS"
#endif  // CDS_BUILD_BITS

        //@cond
        struct free_block_header: block_header {
            unsigned int    nNextFree;
        };
        //@endcond

#if CDS_BUILD_BITS == 32
        /// Processor heap's \p active field
        /**
            The \p active field in the processor heap structure is primarily a pointer to the descriptor
            of the active superblock owned by the processor heap. If the value of \p active is not \p nullptr, it is
            guaranteed that the active superblock has at least one block available for reservation.
            Since the addresses of superblock descriptors can be guaranteed to be aligned to some power
            of 2 (e.g., 64), as an optimization, we can carve a credits subfield to hold the number
            of blocks available for reservation in the active superblock less one. That is, if the value
            of credits is n, then the active superblock contains n+1 blocks available for reservation
            through the \p active field. Note that the number of blocks in a superblock is not limited
            to the maximum reservations that can be held in the credits subfield. In a typical malloc operation
            (i.e., when \p active != \p nullptr and \p credits > 0), the thread reads \p active and then
            atomically decrements credits while validating that the active superblock is still valid.
        */
        class active_tag {
        //@cond
            superblock_desc *       pDesc;
            uint32_t             nCredits;

        public:
            static const unsigned int c_nMaxCredits = 0 - 1;

        public:
            CDS_CONSTEXPR active_tag() CDS_NOEXCEPT
                : pDesc( nullptr )
                , nCredits(0)
            {}

            active_tag( active_tag const& ) CDS_NOEXCEPT = default;
            ~active_tag() CDS_NOEXCEPT = default;
            active_tag& operator=(active_tag const& ) CDS_NOEXCEPT = default;
#       if !defined(CDS_DISABLE_DEFAULT_MOVE_CTOR)
            active_tag( active_tag&& ) CDS_NOEXCEPT = default;
            active_tag& operator=(active_tag&&) CDS_NOEXCEPT = default;
#       endif

            /// Returns pointer to superblock descriptor
            superblock_desc * ptr() const
            {
                return pDesc;
            }

            /// Sets superblock descriptor
            void ptr( superblock_desc * p )
            {
                pDesc = p;
            }

            unsigned int credits() const
            {
                return nCredits;
            }

            void credits( unsigned int n )
            {
                nCredits = n;
            }

            void clear()
            {
                pDesc = nullptr;
                nCredits = 0;
            }

            void set( superblock_desc * pSB, unsigned int n )
            {
                pDesc = pSB;
                nCredits = n;
            }
        //@endcond
        };
#elif CDS_BUILD_BITS == 64
        //@cond
        class active_tag
        {
        public:
            static const unsigned int c_nMaxCredits = c_nAlignment - 1 ; // 0x003F;
        protected:
            typedef cds::details::marked_ptr<superblock_desc, c_nMaxCredits> marked_desc_ptr;
            marked_desc_ptr pDesc;

        public:
            active_tag() CDS_NOEXCEPT
                : pDesc( nullptr )
            {}
            // Clang 3.1: error: first argument to atomic operation must be a pointer to a trivially-copyable type
            //active_tag() CDS_NOEXCEPT = default;
            active_tag( active_tag const& ) CDS_NOEXCEPT = default;
            ~active_tag() CDS_NOEXCEPT = default;
            active_tag& operator=(active_tag const&) CDS_NOEXCEPT = default;
#       if !defined(CDS_DISABLE_DEFAULT_MOVE_CTOR)
            active_tag( active_tag&& ) CDS_NOEXCEPT = default;
            active_tag& operator=(active_tag&&) CDS_NOEXCEPT = default;
#       endif
            superblock_desc *    ptr() const
            {
                return pDesc.ptr();
            }

            void ptr( superblock_desc * p )
            {
                assert( (reinterpret_cast<uintptr_t>(p) & c_nMaxCredits) == 0 );
                pDesc = marked_desc_ptr( p, pDesc.bits());
            }

            unsigned int credits()
            {
                return (unsigned int) pDesc.bits();
            }

            void credits( unsigned int n )
            {
                assert( n <= c_nMaxCredits );
                pDesc = marked_desc_ptr( pDesc.ptr(), n );
            }

            void clear()
            {
                pDesc = marked_desc_ptr();
            }

            void set( superblock_desc * pSB, unsigned int n )
            {
                assert( (reinterpret_cast<uintptr_t>(pSB) & c_nMaxCredits) == 0 );
                pDesc = marked_desc_ptr( pSB, n );
            }

        };
        //@endcond
#else
#       error "Unexpected value of CDS_BUILD_BITS"
#endif  // CDS_BUILD_BITS


        /// Processor heap
        struct processor_heap_base
        {
            CDS_DATA_ALIGNMENT(8) atomics::atomic<active_tag> active;   ///< pointer to the descriptor of active superblock owned by processor heap
            processor_desc *    pProcDesc   ;   ///< pointer to parent processor descriptor
            const size_class *  pSizeClass  ;   ///< pointer to size class
            atomics::atomic<superblock_desc *>   pPartial    ;   ///< pointer to partial filled superblock (may be \p nullptr)
            partial_list        partialList ;   ///< list of partial filled superblocks owned by the processor heap
            unsigned int        nPageIdx    ;   ///< page size-class index, \ref c_nPageSelfAllocation - "small page"

            /// Small page marker
            /**
                If page is small and can be allocated by the Heap, the \p nPageIdx value is \p c_nPageSelfAllocation.
                This optimization allows to allocate system memory more regularly, in blocks of 1M that leads
                to less memory fragmentation.
            */
            static const unsigned int c_nPageSelfAllocation = (unsigned int) -1;

            procheap_stat       stat        ;   ///< heap statistics
            //processor_heap_statistics   stat;

            //@cond
            processor_heap_base() CDS_NOEXCEPT
                : pProcDesc( nullptr )
                , pSizeClass( nullptr )
                , pPartial( nullptr )
            {
                assert( (reinterpret_cast<uintptr_t>(this) & (c_nAlignment - 1)) == 0 );
            }
            //@endcond

            /// Get partial superblock owned by the processor heap
            superblock_desc * get_partial()
            {
                superblock_desc * pDesc = pPartial.load(atomics::memory_order_acquire);
                do {
                    if ( !pDesc ) {
                        pDesc =  partialList.pop();
                        break;
                    }
                } while ( !pPartial.compare_exchange_weak( pDesc, nullptr, atomics::memory_order_release, atomics::memory_order_relaxed ) );

                //assert( pDesc == nullptr || free_desc_list<superblock_desc>::node_algorithms::inited( static_cast<sb_free_list_hook *>(pDesc) ));
                //assert( pDesc == nullptr || partial_desc_list<superblock_desc>::node_algorithms::inited( static_cast<sb_partial_list_hook *>(pDesc) ) );
                return pDesc;
            }

            /// Add partial superblock \p pDesc to the list
            void add_partial( superblock_desc * pDesc )
            {
                assert( pPartial != pDesc );
                //assert( partial_desc_list<superblock_desc>::node_algorithms::inited( static_cast<sb_partial_list_hook *>(pDesc) ) );

                superblock_desc * pCur = nullptr;
                if ( !pPartial.compare_exchange_strong(pCur, pDesc, atomics::memory_order_acq_rel, atomics::memory_order_relaxed) )
                    partialList.push( pDesc );
            }


            /// Remove superblock \p pDesc from the list of partial superblock
            bool unlink_partial( superblock_desc * pDesc )
            {
                return partialList.unlink( pDesc );
            }
        };

        /// Aligned superblock descriptor
        typedef typename cds::details::type_padding<processor_heap_base, c_nAlignment>::type processor_heap;

        /// Processor descriptor
        struct processor_desc
        {
            processor_heap *    arrProcHeap     ; ///< array of processor heap
            free_list           listSBDescFree  ; ///< List of free superblock descriptors
            page_heap *         pageHeaps       ; ///< array of page heap (one for each page size)

            //@cond
            processor_desc()
                : arrProcHeap( nullptr )
                , pageHeaps( nullptr )
            {}
            //@endcond
        };


    protected:
        sys_topology        m_Topology           ;  ///< System topology
        system_heap         m_LargeHeap          ;  ///< Heap for large block
        aligned_heap        m_AlignedHeap        ;  ///< Internal aligned heap
        sizeclass_selector  m_SizeClassSelector  ;  ///< Size-class selector
        atomics::atomic<processor_desc *> *   m_arrProcDesc  ;  ///< array of pointers to the processor descriptors
        unsigned int        m_nProcessorCount    ;  ///< Processor count
        bound_checker       m_BoundChecker       ;  ///< Bound checker

        os_allocated_stat   m_OSAllocStat        ;  ///< OS-allocated memory statistics

    protected:
        //@cond

        /// Allocates large block from system memory
        block_header * alloc_from_OS( size_t nSize )
        {
            block_header * p = reinterpret_cast<block_header *>( m_LargeHeap.alloc( nSize ) );
            m_OSAllocStat.incBytesAllocated( nSize );
            p->setOSAllocated( nSize );
            return p;
        }

        /// Allocates from the active superblock if it possible
        block_header * alloc_from_active( processor_heap * pProcHeap )
        {
            active_tag  oldActive;
            int nCollision = -1;

            // Reserve block
            while ( true ) {
                ++nCollision;
                oldActive = pProcHeap->active.load(atomics::memory_order_acquire);
                if ( !oldActive.ptr() )
                    return nullptr;
                unsigned int nCredits = oldActive.credits();
                active_tag  newActive   ; // default = 0
                if ( nCredits != 0 ) {
                    newActive = oldActive;
                    newActive.credits( nCredits - 1 );
                }
                if ( pProcHeap->active.compare_exchange_strong( oldActive, newActive, atomics::memory_order_release, atomics::memory_order_relaxed ))
                    break;
            }

            if ( nCollision )
                pProcHeap->stat.incActiveDescCASFailureCount( nCollision );

            // pop block
            superblock_desc * pDesc = oldActive.ptr();

            anchor_tag  oldAnchor;
            anchor_tag  newAnchor;
            byte * pAddr;
            unsigned int nMoreCredits = 0;

            nCollision = -1;
            do {
                ++nCollision;
                newAnchor = oldAnchor = pDesc->anchor.load(atomics::memory_order_acquire);

                assert( oldAnchor.avail < pDesc->nCapacity );
                pAddr = pDesc->pSB + oldAnchor.avail * (unsigned long long) pDesc->nBlockSize;

                // TSan reports data race if the block contained atomic ops before
                CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN;
                newAnchor.avail = reinterpret_cast<free_block_header *>( pAddr )->nNextFree;
                CDS_TSAN_ANNOTATE_IGNORE_WRITES_END;
                newAnchor.tag += 1;

                if ( oldActive.credits() == 0 ) {
                    // state must be ACTIVE
                    if ( oldAnchor.count == 0 )
                        newAnchor.state = SBSTATE_FULL;
                    else {
                        nMoreCredits = oldAnchor.count < active_tag::c_nMaxCredits ? ((unsigned int) oldAnchor.count) : active_tag::c_nMaxCredits;
                        newAnchor.count -= nMoreCredits;
                    }
                }
            } while ( !pDesc->anchor.compare_exchange_strong( oldAnchor, newAnchor, atomics::memory_order_release, atomics::memory_order_relaxed ));

            if ( nCollision )
                pProcHeap->stat.incActiveAnchorCASFailureCount( nCollision );

            assert( newAnchor.state != SBSTATE_EMPTY );

            if ( newAnchor.state == SBSTATE_FULL )
                pProcHeap->stat.incDescFull();
            if ( oldActive.credits() == 0 && oldAnchor.count > 0 )
                update_active( pProcHeap, pDesc, nMoreCredits );

            pProcHeap->stat.incAllocFromActive();

            // block_header fields is not needed to setup
            // It was set in alloc_from_new_superblock
            assert( reinterpret_cast<block_header *>( pAddr )->desc() == pDesc );
            assert( !reinterpret_cast<block_header *>( pAddr )->isOSAllocated() );
            assert( !reinterpret_cast<block_header *>( pAddr )->isAligned() );

            return reinterpret_cast<block_header *>( pAddr );
        }

        /// Allocates from a partial filled superblock if it possible
        block_header * alloc_from_partial( processor_heap * pProcHeap )
        {
        retry:
            superblock_desc * pDesc = pProcHeap->get_partial();
            if ( !pDesc )
                return nullptr;

            // reserve blocks
            anchor_tag  oldAnchor;
            anchor_tag  newAnchor;
            //byte * pAddr;
            unsigned int nMoreCredits = 0;

            int nCollision = -1;
            do {
                ++nCollision;

                newAnchor = oldAnchor = pDesc->anchor.load(atomics::memory_order_acquire);
                if ( oldAnchor.state == SBSTATE_EMPTY ) {
                    free_superblock( pDesc );
                    goto retry;
                }

                nMoreCredits = ((unsigned int)(oldAnchor.count - 1)) < active_tag::c_nMaxCredits ? (unsigned int)(oldAnchor.count - 1) : active_tag::c_nMaxCredits;
                newAnchor.count -= nMoreCredits + 1;
                newAnchor.state = (nMoreCredits > 0) ? SBSTATE_ACTIVE : SBSTATE_FULL;
                newAnchor.tag += 1;
            } while ( !pDesc->anchor.compare_exchange_strong(oldAnchor, newAnchor, atomics::memory_order_release, atomics::memory_order_relaxed) );

            if ( nCollision )
                pProcHeap->stat.incPartialDescCASFailureCount( nCollision );

            if ( newAnchor.state == SBSTATE_FULL )
                pProcHeap->stat.incDescFull();

            // Now, the thread is guaranteed to have reserved one or more blocks
            // pop reserved block
            byte * pAddr;
            nCollision = -1;
            do {
                ++nCollision;

                newAnchor = oldAnchor = pDesc->anchor.load(atomics::memory_order_acquire);

                assert( oldAnchor.avail < pDesc->nCapacity );
                pAddr = pDesc->pSB + oldAnchor.avail * pDesc->nBlockSize;
                newAnchor.avail = reinterpret_cast<free_block_header *>( pAddr )->nNextFree;
                ++newAnchor.tag;
            } while ( !pDesc->anchor.compare_exchange_strong(oldAnchor, newAnchor, atomics::memory_order_release, atomics::memory_order_relaxed) );

            if ( nCollision )
                pProcHeap->stat.incPartialAnchorCASFailureCount( nCollision );

            assert( newAnchor.state != SBSTATE_EMPTY );

            pProcHeap->stat.incAllocFromPartial();

            if ( nMoreCredits > 0 )
                update_active( pProcHeap, pDesc, nMoreCredits );

            // block_header fields is not needed to setup
            // It was set in alloc_from_new_superblock
            assert( reinterpret_cast<block_header *>( pAddr )->desc() == pDesc );
            assert( !reinterpret_cast<block_header *>( pAddr )->isAligned() );
            assert( !reinterpret_cast<block_header *>( pAddr )->isOSAllocated() );

            return reinterpret_cast<block_header *>( pAddr );
        }

        /// Allocates from the new superblock
        block_header * alloc_from_new_superblock( processor_heap * pProcHeap )
        {
            superblock_desc * pDesc = new_superblock_desc( pProcHeap );
            assert( pDesc != nullptr );
            pDesc->pSB = new_superblock_buffer( pProcHeap );

            anchor_tag anchor = pDesc->anchor.load(atomics::memory_order_relaxed);
            anchor.tag += 1;

            // Make single-linked list of free blocks in superblock
            byte * pEnd = pDesc->pSB + pDesc->nCapacity * pDesc->nBlockSize;
            unsigned int nNext = 0;
            const unsigned int nBlockSize = pDesc->nBlockSize;
            for ( byte * p = pDesc->pSB; p < pEnd; p += nBlockSize ) {
                reinterpret_cast<block_header *>( p )->set( pDesc, 0 );
                reinterpret_cast<free_block_header *>( p )->nNextFree = ++nNext;
            }
            reinterpret_cast<free_block_header *>( pEnd - nBlockSize )->nNextFree = 0;

            active_tag newActive;
            newActive.set( pDesc, ( (pDesc->nCapacity - 1 < active_tag::c_nMaxCredits) ? pDesc->nCapacity - 1 : active_tag::c_nMaxCredits ) - 1 );

            anchor.count = pDesc->nCapacity - 1 - (newActive.credits() + 1);
            anchor.state = SBSTATE_ACTIVE;
            pDesc->anchor.store(anchor, atomics::memory_order_relaxed);

            active_tag curActive;
            if ( pProcHeap->active.compare_exchange_strong( curActive, newActive, atomics::memory_order_release, atomics::memory_order_relaxed )) {
                pProcHeap->stat.incAllocFromNew();
                //reinterpret_cast<block_header *>( pDesc->pSB )->set( pDesc, 0 );
                return reinterpret_cast<block_header *>( pDesc->pSB );
            }

            free_superblock( pDesc );
            return nullptr;
        }

        /// Find appropriate processor heap based on size-class selected
        processor_heap * find_heap( typename sizeclass_selector::sizeclass_index nSizeClassIndex )
        {
            assert( nSizeClassIndex < m_SizeClassSelector.size() );

            unsigned int nProcessorId = m_Topology.current_processor();
            assert( nProcessorId < m_nProcessorCount );

            if ( nProcessorId >= m_nProcessorCount )
                nProcessorId = 0;

            processor_desc * pDesc = m_arrProcDesc[ nProcessorId ].load( atomics::memory_order_relaxed );
            while ( !pDesc ) {

                processor_desc * pNewDesc = new_processor_desc( nProcessorId );
                if ( m_arrProcDesc[nProcessorId].compare_exchange_strong( pDesc, pNewDesc, atomics::memory_order_release, atomics::memory_order_relaxed ) ) {
                    pDesc = pNewDesc;
                    break;
                }
                free_processor_desc( pNewDesc );
            }

            return &( pDesc->arrProcHeap[ nSizeClassIndex ] );
        }

        /// Updates active field of processor heap \p pProcHeap
        void update_active( processor_heap * pProcHeap, superblock_desc * pDesc, unsigned int nCredits )
        {
            assert( pProcHeap == pDesc->pProcHeap );

            active_tag  nullActive;
            active_tag  newActive;
            newActive.set( pDesc, nCredits - 1 );

            if ( pProcHeap->active.compare_exchange_strong( nullActive, newActive, atomics::memory_order_seq_cst, atomics::memory_order_relaxed ) )
                return;

            // Someone installed another active superblock.
            // Return credits to superblock and make it partial

            anchor_tag  oldAnchor;
            anchor_tag  newAnchor;

            do {
                newAnchor = oldAnchor = pDesc->anchor.load(atomics::memory_order_acquire);
                newAnchor.count += nCredits;
                newAnchor.state = SBSTATE_PARTIAL;
            } while ( !pDesc->anchor.compare_exchange_weak( oldAnchor, newAnchor, atomics::memory_order_release, atomics::memory_order_relaxed ));

            pDesc->pProcHeap->add_partial( pDesc );
        }

        /// Allocates new processor descriptor
        processor_desc * new_processor_desc( unsigned int nProcessorId )
        {
            CDS_UNUSED( nProcessorId );
            processor_desc * pDesc;
            const size_t nPageHeapCount = m_SizeClassSelector.pageTypeCount();

            /*
                Processor descriptor layout

                proc_desc -  64-byte alignment
                page_heap[0] 64-byte alignment
                page_heap[1] 64-byte alignment
                ...
                page_heap[P] 64-byte alignment

                proc_heap[0] 64-byte alignment
                proc_heap[1] 64-byte alignment
                ...
                proc_heap[N] 64-byte alignment
            */

            const size_t szDesc =
                ( sizeof(processor_desc)
                    + sizeof(pDesc->pageHeaps[0]) * nPageHeapCount
                    + c_nAlignment - 1
                ) / c_nAlignment
;

            const size_t szTotal = szDesc * c_nAlignment + sizeof(processor_heap) * m_SizeClassSelector.size();

            static_assert( (sizeof(processor_heap) % c_nAlignment) == 0, "sizeof(processor_heap) error" );

            // TSan false positive: a new descriptor will be linked further with release fence
            CDS_TSAN_ANNOTATE_IGNORE_WRITES_BEGIN;

            pDesc = new( m_AlignedHeap.alloc( szTotal, c_nAlignment ) ) processor_desc;

            pDesc->pageHeaps = reinterpret_cast<page_heap *>( pDesc + 1 );
            for ( size_t i = 0; i < nPageHeapCount; ++i )
                new (pDesc->pageHeaps + i) page_heap( m_SizeClassSelector.page_size(i));

            // initialize processor heaps
            pDesc->arrProcHeap =
                reinterpret_cast<processor_heap *>(
                    reinterpret_cast<uintptr_t>(reinterpret_cast<byte *>(pDesc + 1) + sizeof(pDesc->pageHeaps[0]) * nPageHeapCount + c_nAlignment - 1)
                    & ~(uintptr_t(c_nAlignment) - 1)
                );

            processor_heap * pProcHeap = pDesc->arrProcHeap;
            processor_heap * pProcHeapEnd = pDesc->arrProcHeap + m_SizeClassSelector.size();
            for ( unsigned int i = 0; pProcHeap != pProcHeapEnd; ++pProcHeap, ++i ) {
                new (pProcHeap) processor_heap();
                pProcHeap->pProcDesc = pDesc;
                pProcHeap->pSizeClass = m_SizeClassSelector.at(i);
                if ( m_SizeClassSelector.find( pProcHeap->pSizeClass->nSBSize ) != sizeclass_selector::c_nNoSizeClass )
                    pProcHeap->nPageIdx = processor_heap::c_nPageSelfAllocation;
                else
                    pProcHeap->nPageIdx = pProcHeap->pSizeClass->nSBSizeIdx;
            }
            CDS_TSAN_ANNOTATE_IGNORE_WRITES_END;
            return pDesc;
        }


        void free_processor_heap( processor_heap * pProcHeap )
        {
            assert( pProcHeap->nPageIdx != processor_heap::c_nPageSelfAllocation );

            page_heap& pageHeap = pProcHeap->pProcDesc->pageHeaps[pProcHeap->nPageIdx];
            superblock_desc * pDesc;

            for ( pDesc = pProcHeap->partialList.pop(); pDesc; pDesc = pProcHeap->partialList.pop()) {
                pageHeap.free( pDesc->pSB );
                m_AlignedHeap.free( pDesc );
            }

            superblock_desc * pPartial = pProcHeap->pPartial.load(atomics::memory_order_relaxed);
            if ( pPartial ) {
                pageHeap.free( pPartial->pSB );
                m_AlignedHeap.free( pPartial );
            }

            pDesc = pProcHeap->active.load(atomics::memory_order_relaxed).ptr();
            if ( pDesc ) {
                pageHeap.free( pDesc->pSB );
                m_AlignedHeap.free( pDesc );
            }
        }

        /// Frees processor descriptor
        void free_processor_desc( processor_desc * pDesc )
        {
            const size_t nPageHeapCount = m_SizeClassSelector.pageTypeCount();

            {
                processor_heap * const pProcHeapEnd = pDesc->arrProcHeap + m_SizeClassSelector.size();

                // free large blocks only
                for ( processor_heap * pProcHeap = pDesc->arrProcHeap; pProcHeap < pProcHeapEnd; ++pProcHeap ) {
                    if ( pProcHeap->nPageIdx != processor_heap::c_nPageSelfAllocation )
                        free_processor_heap( pProcHeap );

                    pProcHeap->~processor_heap();
                }
            }

            for ( superblock_desc * pSBDesc = pDesc->listSBDescFree.pop(); pSBDesc; pSBDesc = pDesc->listSBDescFree.pop())
                m_AlignedHeap.free( pSBDesc );

            for (size_t i = 0; i < nPageHeapCount; ++i )
                (pDesc->pageHeaps + i)->page_heap::~page_heap();

            pDesc->pageHeaps = nullptr;

            pDesc->processor_desc::~processor_desc();
            m_AlignedHeap.free( pDesc );
        }

        /// Allocates new superblock descriptor
        superblock_desc * new_superblock_desc( processor_heap * pProcHeap )
        {
            anchor_tag anchor;
            superblock_desc * pDesc = pProcHeap->pProcDesc->listSBDescFree.pop();
            if ( pDesc == nullptr ) {
                pDesc = new( m_AlignedHeap.alloc(sizeof(superblock_desc), c_nAlignment ) ) superblock_desc;
                assert( (uintptr_t(pDesc) & (c_nAlignment - 1)) == 0 );

                anchor = pDesc->anchor.load( atomics::memory_order_relaxed );
                anchor.tag = 0;
                pDesc->anchor.store( anchor, atomics::memory_order_relaxed );

                pProcHeap->stat.incDescAllocCount();
            }
            pDesc->nBlockSize = pProcHeap->pSizeClass->nBlockSize;
            pDesc->nCapacity = pProcHeap->pSizeClass->nCapacity;
            assert( pDesc->nCapacity <= c_nMaxBlockInSuperBlock );
            pDesc->pProcHeap = pProcHeap;

            anchor = pDesc->anchor.load( atomics::memory_order_relaxed );
            anchor.avail = 1;
            pDesc->anchor.store( anchor, atomics::memory_order_relaxed );

            return pDesc;
        }

        /// Allocates superblock page
        byte * new_superblock_buffer( processor_heap * pProcHeap )
        {
            pProcHeap->stat.incBlockAllocated();
            if ( pProcHeap->nPageIdx == processor_heap::c_nPageSelfAllocation ) {
                return (byte *) alloc( pProcHeap->pSizeClass->nSBSize );
            }
            else {
                return (byte *) pProcHeap->pProcDesc->pageHeaps[pProcHeap->nPageIdx].alloc();
            }
        }

        /// Frees superblock descriptor and its page
        void free_superblock( superblock_desc * pDesc )
        {
            pDesc->pProcHeap->stat.incBlockDeallocated();
            processor_desc * pProcDesc = pDesc->pProcHeap->pProcDesc;
            if ( pDesc->pSB ) {
                if ( pDesc->pProcHeap->nPageIdx == processor_heap::c_nPageSelfAllocation )
                    free( pDesc->pSB );
                else
                    pProcDesc->pageHeaps[pDesc->pProcHeap->nPageIdx].free( pDesc->pSB );
            }
            pProcDesc->listSBDescFree.push( pDesc );
        }

        /// Allocate memory block
        block_header * int_alloc(
            size_t nSize    ///< Size of memory block to allocate in bytes
            )
        {
            typename sizeclass_selector::sizeclass_index nSizeClassIndex = m_SizeClassSelector.find( nSize );
            if ( nSizeClassIndex == sizeclass_selector::c_nNoSizeClass ) {
                return alloc_from_OS( nSize );
            }
            assert( nSizeClassIndex < m_SizeClassSelector.size() );

            block_header * pBlock;
            processor_heap * pProcHeap;
            while ( true ) {
                pProcHeap = find_heap( nSizeClassIndex );
                if ( !pProcHeap )
                    return alloc_from_OS( nSize );

                if ( (pBlock = alloc_from_active( pProcHeap )) != nullptr )
                    break;
                if ( (pBlock = alloc_from_partial( pProcHeap )) != nullptr )
                    break;
                if ( (pBlock = alloc_from_new_superblock( pProcHeap )) != nullptr )
                    break;
            }

            pProcHeap->stat.incAllocatedBytes( pProcHeap->pSizeClass->nBlockSize );

            assert( pBlock != nullptr );
            return pBlock;
        }

        //@endcond
    public:
        /// Heap constructor
        Heap()
        {
            // Explicit libcds initialization is needed since a static object may be constructed
            cds::Initialize();

            m_nProcessorCount = m_Topology.processor_count();
            m_arrProcDesc = new( m_AlignedHeap.alloc(sizeof(processor_desc *) * m_nProcessorCount, c_nAlignment ))
                atomics::atomic<processor_desc *>[ m_nProcessorCount ];
            memset( m_arrProcDesc, 0, sizeof(processor_desc *) * m_nProcessorCount )    ;   // ?? memset for atomic<>
        }

        /// Heap destructor
        /**
            The destructor frees all memory allocated by the heap.
        */
        ~Heap()
        {
            for ( unsigned int i = 0; i < m_nProcessorCount; ++i ) {
                processor_desc * pDesc = m_arrProcDesc[i].load(atomics::memory_order_relaxed);
                if ( pDesc )
                    free_processor_desc( pDesc );
            }

            m_AlignedHeap.free( m_arrProcDesc );

            // Explicit termination of libcds
            cds::Terminate();
        }

        /// Allocate memory block
        void * alloc(
            size_t nSize    ///< Size of memory block to allocate in bytes
        )
        {
            block_header * pBlock = int_alloc( nSize + sizeof(block_header) + bound_checker::trailer_size );

            // Bound checking is only for our blocks
            if ( !pBlock->isOSAllocated() ) {
                // the block is allocated from our heap - bound checker is applicable
                m_BoundChecker.make_trailer(
                    reinterpret_cast<byte *>(pBlock + 1),
                    reinterpret_cast<byte *>(pBlock) + pBlock->desc()->nBlockSize,
                    nSize
                );
            }

            CDS_TSAN_ANNOTATE_NEW_MEMORY( pBlock + 1, nSize );
            return pBlock + 1;
        }

        /// Free previously allocated memory block
        void free(
            void * pMemory  ///< Pointer to memory block to free
        )
        {
            if ( !pMemory )
                return;

            block_header * pRedirect = (reinterpret_cast<block_header *>( pMemory ) - 1);
            block_header * pBlock = pRedirect->begin();

            if ( pBlock->isOSAllocated() ) {
                // Block has been allocated from OS
                m_OSAllocStat.incBytesDeallocated( pBlock->getOSAllocSize() );
                m_LargeHeap.free( pBlock );
                return;
            }

            assert( !pBlock->isAligned() );
            superblock_desc * pDesc = pBlock->desc();

            m_BoundChecker.check_bounds(
                pRedirect + 1,
                reinterpret_cast<byte *>( pBlock ) + pDesc->nBlockSize,
                pDesc->nBlockSize
            );


            anchor_tag oldAnchor;
            anchor_tag newAnchor;
            processor_heap_base * pProcHeap = pDesc->pProcHeap;

            pProcHeap->stat.incDeallocatedBytes( pDesc->nBlockSize );

            oldAnchor = pDesc->anchor.load(atomics::memory_order_acquire);
            do {
                newAnchor = oldAnchor;
                reinterpret_cast<free_block_header *>( pBlock )->nNextFree = oldAnchor.avail;
                newAnchor.avail = (reinterpret_cast<byte *>( pBlock ) - pDesc->pSB) / pDesc->nBlockSize;
                newAnchor.tag += 1;

                assert( oldAnchor.state != SBSTATE_EMPTY );

                if ( oldAnchor.state == SBSTATE_FULL )
                    newAnchor.state = SBSTATE_PARTIAL;

                if ( oldAnchor.count == pDesc->nCapacity - 1 ) {
                    //pProcHeap = pDesc->pProcHeap;
                    //CDS_COMPILER_RW_BARRIER         ;   // instruction fence is needed?..
                    newAnchor.state = SBSTATE_EMPTY;
                }
                else
                    newAnchor.count += 1;
            } while ( !pDesc->anchor.compare_exchange_strong( oldAnchor, newAnchor, atomics::memory_order_release, atomics::memory_order_relaxed ) );

            pProcHeap->stat.incFreeCount();

            if ( newAnchor.state == SBSTATE_EMPTY ) {
                if ( pProcHeap->unlink_partial( pDesc ))
                    free_superblock( pDesc );
            }
            else if (oldAnchor.state == SBSTATE_FULL ) {
                assert( pProcHeap != nullptr );
                pProcHeap->stat.decDescFull();
                pProcHeap->add_partial( pDesc );
            }
        }

        /// Reallocate memory block
        /**
            If \p nNewSize is zero, then the block pointed to by \p pMemory is freed;
            the return value is \p nullptr, and \p pMemory is left pointing at a freed block.

            If there is not enough available memory to expand the block to the given size,
            the original block is left unchanged, and \p nullptr is returned.

            Aligned memory block cannot be realloc'ed: if \p pMemory has been allocated by \ref alloc_aligned,
            then the return value is \p nullptr and the original block is left unchanged.
        */
        void * realloc(
            void *  pMemory,    ///< Pointer to previously allocated memory block
            size_t  nNewSize    ///< New size of memory block, in bytes
        )
        {
            if ( nNewSize == 0 ) {
                free( pMemory );
                return nullptr;
            }

            const size_t nOrigSize = nNewSize;
            nNewSize += sizeof(block_header) + bound_checker::trailer_size;

            block_header * pBlock = reinterpret_cast<block_header *>( pMemory ) - 1;

            // Reallocation of aligned block is not possible
            if ( pBlock->isAligned() ) {
                assert( false );
                return nullptr;
            }

            if ( pBlock->isOSAllocated() ) {
                // The block has been allocated from OS
                size_t nCurSize = pBlock->getOSAllocSize();

                if ( nCurSize >= nNewSize )
                    return pMemory;

                // Grow block size
                void * pNewBuf = alloc( nOrigSize );
                if ( pNewBuf ) {
                    memcpy( pNewBuf, pMemory, nCurSize - sizeof(block_header) );
                    free( pMemory );
                }
                return pNewBuf;
            }

            superblock_desc * pDesc = pBlock->desc();
            if ( pDesc->nBlockSize <= nNewSize ) {
                // In-place reallocation
                m_BoundChecker.make_trailer(
                    reinterpret_cast<byte *>(pBlock + 1),
                    reinterpret_cast<byte *>(pBlock) + pBlock->desc()->nBlockSize,
                    nOrigSize
                    );

                return pMemory;
            }

            void * pNew = alloc( nNewSize );
            if ( pNew ) {
                memcpy( pNew, pMemory, pDesc->nBlockSize - sizeof(block_header) );
                free( pMemory );
                return pNew;
            }

            return nullptr;
        }

        /// Allocate aligned memory block
        void * alloc_aligned(
            size_t nSize,       ///< Size of memory block to allocate in bytes
            size_t nAlignment   ///< Alignment
        )
        {
            if ( nAlignment <= c_nDefaultBlockAlignment ) {
                void * p = alloc( nSize );
                assert( (reinterpret_cast<uintptr_t>(p) & (nAlignment - 1)) == 0 );
                return p;
            }

            block_header * pBlock = int_alloc( nSize + nAlignment + sizeof(block_header) + bound_checker::trailer_size );

            block_header * pRedirect;
            if ( (reinterpret_cast<uintptr_t>( pBlock + 1) & (nAlignment - 1)) != 0 ) {
                pRedirect = reinterpret_cast<block_header *>( (reinterpret_cast<uintptr_t>( pBlock ) & ~(nAlignment - 1)) + nAlignment ) - 1;
                assert( pRedirect != pBlock );
                pRedirect->set( reinterpret_cast<superblock_desc *>(pBlock), 1 );

                assert( (reinterpret_cast<uintptr_t>(pRedirect + 1) & (nAlignment - 1)) == 0 );
            }
            else
                pRedirect = pBlock;


            // Bound checking is only for our blocks
            if ( !pBlock->isOSAllocated() ) {
                // the block is allocated from our heap - bound checker is applicable
                m_BoundChecker.make_trailer(
                    reinterpret_cast<byte *>(pRedirect + 1),
                    reinterpret_cast<byte *>(pBlock) + pBlock->desc()->nBlockSize,
                    nSize
                );
            }

            return pRedirect + 1;
        }

        /// Free aligned memory block previously allocated by \ref alloc_aligned
        void free_aligned(
            void * pMemory      ///< Pointer to memory block to free
        )
        {
            free( pMemory );
        }

    public:

        /// Get instant summary statistics
        void summaryStat( summary_stat& st )
        {
            size_t nProcHeapCount = m_SizeClassSelector.size();
            for ( unsigned int nProcessor = 0; nProcessor < m_nProcessorCount; ++nProcessor ) {
                processor_desc * pProcDesc = m_arrProcDesc[nProcessor].load(atomics::memory_order_relaxed);
                if ( pProcDesc ) {
                    for ( unsigned int i = 0; i < nProcHeapCount; ++i ) {
                        processor_heap_base * pProcHeap = pProcDesc->arrProcHeap + i;
                        if ( pProcHeap ) {
                            st.add_procheap_stat( pProcHeap->stat );
                        }
                    }
                }
            }

            st.add_heap_stat( m_OSAllocStat );
        }
    };

}}} // namespace cds::memory::michael

#endif // CDSLIB_MEMORY_MICHAEL_ALLOCATOR_TMPL_H
