// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
        pool_allocator() noexcept
        {}

        pool_allocator(const pool_allocator&) noexcept
        {}
        template <class U> pool_allocator(const pool_allocator<U, accessor_type>&) noexcept
        {}
        ~pool_allocator()
        {}

        pointer address(reference x) const noexcept
        {
            return &x;
        }
        const_pointer address(const_reference x) const noexcept
        {
            return &x;
        }
        pointer allocate( size_type n, void const * /*hint*/ = 0)
        {
            static_assert( sizeof(value_type) <= sizeof(typename accessor_type::value_type), "Incompatible type" );

            return reinterpret_cast<pointer>( accessor_type()().allocate( n ));
        }
        void deallocate(pointer p, size_type n) noexcept
        {
            accessor_type()().deallocate( reinterpret_cast<typename accessor_type::value_type *>( p ), n );
        }
        size_type max_size() const noexcept
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
