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

#ifndef CDSSTRESS_MICHAEL_ALLOC_H
#define CDSSTRESS_MICHAEL_ALLOC_H

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

#endif // #ifndef CDSSTRESS_MICHAEL_ALLOC_H
