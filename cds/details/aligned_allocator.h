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
