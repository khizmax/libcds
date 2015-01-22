//$$CDS-header$$

#ifndef CDSUNIT_MICHAEL_ALLOC_H
#define CDSUNIT_MICHAEL_ALLOC_H

#include <cds/memory/michael/allocator.h>
#include <memory>

namespace memory {

    typedef cds::memory::michael::Heap<
        cds::memory::michael::opt::check_bounds< cds::memory::michael::debug_bound_checking >
    >    michael_heap;
    extern michael_heap s_MichaelHeap;

    template <class T>
    class MichaelAllocator
    {
        typedef std::allocator<T>               std_allocator;
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
        pointer allocate( size_type _Count, const void* /*_Hint*/ = nullptr )
        {
            return reinterpret_cast<pointer>( s_MichaelHeap.alloc( sizeof(T) * _Count ));
        }

        // Deallocation function
        void deallocate( pointer _Ptr, size_type /*_Count*/ )
        {
            s_MichaelHeap.free( _Ptr );
        }

        pointer address( reference r ) const
        {
            return &r;
        }
        const_pointer address( const_reference r ) const
        {
            return &r;
        }
        void construct( pointer p, const T& val )
        {
            return new( p ) T( val );
        }
        void destroy( pointer p )
        {
            p->T::~T();
        }

        // Rebinding allocator to other type
        template <class Other>
        struct rebind {
            typedef MichaelAllocator<Other> other;
        };
    };
}   // namespace memory

#endif // #ifndef CDSUNIT_MICHAEL_ALLOC_H
