//$$CDS-header$$

#include <cds/memory/michael/allocator.h>
#include <iostream>

namespace misc {
    namespace ma = cds::memory::michael;

    typedef ma::Heap<
        ma::opt::procheap_stat<ma::procheap_empty_stat>,
        ma::opt::os_allocated_stat<ma::os_allocated_empty>,
        ma::opt::check_bounds<ma::debug_bound_checking>
    >      t_MichaelHeap_NoStat;

    typedef ma::Heap<
        ma::opt::procheap_stat<ma::procheap_atomic_stat >,
        ma::opt::os_allocated_stat<ma::os_allocated_atomic >,
        ma::opt::check_bounds<ma::debug_bound_checking>
    >  t_MichaelHeap_Stat;

    typedef ma::summary_stat            summary_stat;

    extern t_MichaelHeap_NoStat  s_MichaelHeap_NoStat;
    extern t_MichaelHeap_Stat    s_MichaelHeap_Stat;

    template <typename T>
    class MichaelHeap_NoStat
    {
    public:
        typedef T value_type;
        typedef T * pointer;

        enum {
            alignment = 1
        };


        pointer allocate( size_t nSize, const void * /*pHint*/ )
        {
            return reinterpret_cast<pointer>( s_MichaelHeap_NoStat.alloc( sizeof(T) * nSize ) );
        }

        void deallocate( pointer p, size_t /*nCount*/ )
        {
            s_MichaelHeap_NoStat.free( p );
        }

        static void stat(summary_stat& s)
        {
            s_MichaelHeap_NoStat.summaryStat(s);
        }
    };

    template <typename T>
    class std_allocator: public std::allocator<T>
    {
    public:
        enum {
            alignment = 1
        };

        static void stat(summary_stat& /*s*/)
        {}
    };

    template <typename T>
    class MichaelHeap_Stat
    {
    public:
        typedef T value_type;
        typedef T * pointer;

        enum {
            alignment = 1
        };

        pointer allocate( size_t nSize, const void * /*pHint*/ )
        {
            return reinterpret_cast<pointer>( s_MichaelHeap_Stat.alloc( sizeof(T) * nSize ) );
        }

        void deallocate( pointer p, size_t /*nCount*/ )
        {
            s_MichaelHeap_Stat.free( p );
        }

        static void stat(summary_stat& s)
        {
            s_MichaelHeap_Stat.summaryStat(s);
        }
    };

    template <typename T, size_t ALIGN>
    class MichaelAlignHeap_NoStat
    {
    public:
        typedef T value_type;
        typedef T * pointer;

        enum {
            alignment = ALIGN
        };

        pointer allocate( size_t nSize, const void * /*pHint*/ )
        {
            return reinterpret_cast<pointer>( s_MichaelHeap_NoStat.alloc_aligned( sizeof(T) * nSize, ALIGN ) );
        }

        void deallocate( pointer p, size_t /*nCount*/ )
        {
            s_MichaelHeap_NoStat.free_aligned( p );
        }

        static void stat(summary_stat& s)
        {
            s_MichaelHeap_NoStat.summaryStat(s);
        }
    };

    template <typename T, size_t ALIGN>
    class MichaelAlignHeap_Stat {
    public:
        typedef T value_type;
        typedef T * pointer;

        enum {
            alignment = ALIGN
        };

        pointer allocate( size_t nSize, const void * /*pHint*/ )
        {
            return reinterpret_cast<pointer>( s_MichaelHeap_Stat.alloc_aligned( sizeof(T) * nSize, ALIGN ) );
        }

        void deallocate( pointer p, size_t /*nCount*/ )
        {
            s_MichaelHeap_Stat.free_aligned( p );
        }

        static void stat(summary_stat& s)
        {
            s_MichaelHeap_Stat.summaryStat(s);
        }
    };

    template <typename T, size_t ALIGN>
    class system_aligned_allocator
    {
    public:
        typedef T value_type;
        typedef T * pointer;

        enum {
            alignment = ALIGN
        };

        pointer allocate( size_t nSize, const void * /*pHint*/ )
        {
            return reinterpret_cast<pointer>( cds::OS::aligned_malloc( sizeof(T) * nSize, ALIGN ) );
        }

        void deallocate( pointer p, size_t /*nCount*/ )
        {
            cds::OS::aligned_free( p );
        }

        static void stat(summary_stat& /*s*/)
        {}
    };

    static inline std::ostream& operator <<(std::ostream& os, const summary_stat& s)
    {
        os  << "\t         alloc from active: " << s.nAllocFromActive << "\n"
            << "\t        alloc from partial: " << s.nAllocFromPartial << "\n"
            << "\t            alloc from new: " << s.nAllocFromNew << "\n"
            << "\t           free call count: " << s.nFreeCount << "\n"
            << "\t      superblock allocated: " << s.nPageAllocCount << "\n"
            << "\t    superblock deallocated: " << s.nPageDeallocCount << "\n"
            << "\t superblock desc allocated: " << s.nDescAllocCount << "\n"
            << "\t      superblock full desc: " << s.nDescFull << "\n"
            << "\t     total allocated bytes: " << s.nBytesAllocated << "\n"
            << "\t   total deallocated bytes: " << s.nBytesDeallocated << "\n"
            << "\tOS-allocated large blocks\n"
            << "\t          alloc call count: " << s.nSysAllocCount << "\n"
            << "\t           free call count: " << s.nSysFreeCount << "\n"
            << "\t     total allocated bytes: " << s.nSysBytesAllocated << "\n"
            << "\t   total deallocated bytes: " << s.nSysBytesDeallocated << "\n"
            << "\tCAS contention indicators\n"
            << "\t updating active field of active block: " << s.nActiveDescCASFailureCount << "\n"
            << "\t updating anchor field of active block: " << s.nActiveAnchorCASFailureCount << "\n"
            << "\tupdating active field of partial block: " << s.nPartialDescCASFailureCount << "\n"
            << "\tupdating anchor field of partial block: " << s.nPartialAnchorCASFailureCount
            << std::endl;

        return os;
    }
}
