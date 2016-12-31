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

#ifndef CDSLIB_MEMORY_POOL_ALLOCATOR_H
#define CDSLIB_MEMORY_POOL_ALLOCATOR_H

#include <cds/details/defs.h>
#include <utility>

namespace cds { namespace memory {

    ///@defgroup cds_memory_pool Simple memory pool

    /// Pool allocator adapter
    /**
        This class is an adapter for an object pool. It gives \p std::allocator interface
        for the @ref cds_memory_pool "pool".

        Template arguments:
        - \p T - value type
        - \p Accessor - a functor to access to the pool object. The pool has the following interface:
            \code
            template <typename T>
            class pool {
                typedef T value_type    ;   // Object type maintained by pool
                T * allocate( size_t n )            ;   // Allocate an array of object of type T
                void deallocate( T * p, size_t n )  ;   // Deallocate the array p of size n
            };
            \endcode

        <b>Usage</b>

            Suppose, we have a pool with interface above. Usually, the pool is a static object:
            \code
                static pool<Foo>     thePool;
            \endcode

            The \p %pool_allocator gives \p std::allocator interface for the pool.
            It is needed to declare an <i>accessor</i> functor to access to \p thePool:
            \code
                struct pool_accessor {
                    typedef typename pool::value_type   value_type;

                    pool& operator()() const
                    {
                        return thePool;
                    }
                };
            \endcode

            Now, <tt>cds::memory::pool_allocator< T, pool_accessor > </tt> can be used instead of \p std::allocator.
    */
    template <typename T, typename Accessor>
    class pool_allocator
    {
    //@cond
    public:
        typedef Accessor    accessor_type;

        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef T           value_type;

        template <class U> struct rebind {
            typedef pool_allocator<U, accessor_type> other;
        };

    public:
        pool_allocator() CDS_NOEXCEPT
        {}

        pool_allocator(const pool_allocator&) CDS_NOEXCEPT
        {}
        template <class U> pool_allocator(const pool_allocator<U, accessor_type>&) CDS_NOEXCEPT
        {}
        ~pool_allocator()
        {}

        pointer address(reference x) const CDS_NOEXCEPT
        {
            return &x;
        }
        const_pointer address(const_reference x) const CDS_NOEXCEPT
        {
            return &x;
        }
        pointer allocate( size_type n, void const * /*hint*/ = 0)
        {
            static_assert( sizeof(value_type) <= sizeof(typename accessor_type::value_type), "Incompatible type" );

            return reinterpret_cast<pointer>( accessor_type()().allocate( n ));
        }
        void deallocate(pointer p, size_type n) CDS_NOEXCEPT
        {
            accessor_type()().deallocate( reinterpret_cast<typename accessor_type::value_type *>( p ), n );
        }
        size_type max_size() const CDS_NOEXCEPT
        {
            return size_t(-1) / sizeof(value_type);
        }

        template <class U, class... Args>
        void construct(U* p, Args&&... args)
        {
            new((void *)p) U( std::forward<Args>(args)...);
        }

        template <class U>
        void destroy(U* p)
        {
            p->~U();
        }
    //@endcond
    };

}} // namespace cds::memory


#endif // #ifndef CDSLIB_MEMORY_POOL_ALLOCATOR_H
