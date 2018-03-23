// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_ALIGNED_ALLOCATOR_H
#define CDSLIB_DETAILS_ALIGNED_ALLOCATOR_H

#include <cds/details/defs.h>
#include <cds/user_setup/allocator.h>

namespace cds { namespace details {

    /// Allocator for aligned data
    /**
        The class is the wrapper around user-defined aligned allocator.
        Template parameters:
        \li \p T is a type to allocate
        \li \p ALIGNED_ALLOCATOR is an aligned allocator implementation. Default implementation is defined by macro
        CDS_DEFAULT_ALIGNED_ALLOCATOR from cds/user_setup/allocator.h header file.

        The \p nAlign parameter of member function specifyes desired aligment of data allocated.

        \par Note
        When an array allocation is performed the allocator guarantees the alignment for first element of array only.
        To guarantee the alignment for each element of the array the size of type \p T must be multiple of \p nAlign:
        \code
        sizeof(T) % nAlign == 0
        \endcode
    */
    template <
        typename T
        , typename ALIGNED_ALLOCATOR = CDS_DEFAULT_ALIGNED_ALLOCATOR
    >
    class AlignedAllocator: public ALIGNED_ALLOCATOR::template rebind<T>::other
    {
    public:
        /// Underlying aligned allocator type
        typedef typename ALIGNED_ALLOCATOR::template rebind<T>::other   allocator_type;

        /// Analogue of operator new T(\p src... )
        template <typename... S>
        T *  New( size_t nAlign, const S&... src )
        {
            return Construct( allocator_type::allocate( nAlign, 1), src... );
        }

        /// Analogue of operator new T[\p nCount ]
        T * NewArray( size_t nAlign, size_t nCount )
        {
            T * p = allocator_type::allocate( nAlign, nCount );
            for ( size_t i = 0; i < nCount; ++i )
                Construct( p + i );
            return p;
        }

        /// Analogue of operator new T[\p nCount ].
        /**
            Each item of array of type T is initialized by parameter \p src.
        */
        template <typename S>
        T * NewArray( size_t nAlign, size_t nCount, const S& src )
        {
            T * p = allocator_type::allocate( nAlign, nCount );
            for ( size_t i = 0; i < nCount; ++i )
                Construct( p + i, src );
            return p;
        }

        /// Analogue of operator delete
        void Delete( T * p )
        {
            allocator_type::destroy( p );
            allocator_type::deallocate( p, 1 );
        }

        /// Analogue of operator delete []
        void Delete( T * p, size_t nCount )
        {
            for ( size_t i = 0; i < nCount; ++i )
                allocator_type::destroy( p + i );
            allocator_type::deallocate( p, nCount );
        }

        /// Analogue of placement operator new( \p p ) T( \p src... )
        template <typename... S>
        T * Construct( void * p, const S&... src )
        {
            return new( p ) T( src... );
        }

        /// Rebinds allocator to other type \p Q instead of \p T
        template <typename Q>
        struct rebind {
            typedef AlignedAllocator< Q, typename ALIGNED_ALLOCATOR::template rebind<Q>::other >    other ; ///< Rebinding result
        };
    };

}} // namespace cds::details

#endif // #ifndef CDSLIB_DETAILS_ALIGNED_ALLOCATOR_H
