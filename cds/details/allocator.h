// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_ALLOCATOR_H
#define CDSLIB_DETAILS_ALLOCATOR_H

#include <type_traits>
#include <memory>
#include <cds/details/defs.h>
#include <cds/user_setup/allocator.h>

namespace cds {
    namespace details {

        /// Extends \p std::allocator interface to provide semantics like operator \p new and \p delete
        /**
            The class is the wrapper around underlying \p Alloc class.
            \p Alloc provides the \p std::allocator interface.
        */
        template <typename T, class Alloc = CDS_DEFAULT_ALLOCATOR >
        class Allocator
            : public std::conditional<
                        std::is_same< T, typename Alloc::value_type>::value
                        , Alloc
                        , typename std::allocator_traits<Alloc>::template rebind_alloc<T>
                     >::type
        {
        public:
            /// Underlying allocator type
            typedef typename std::conditional<
                std::is_same< T, typename Alloc::value_type>::value
                , Alloc
                , typename std::allocator_traits<Alloc>::template rebind_alloc<T>
            >::type allocator_type;

            /// Allocator traits
            typedef std::allocator_traits< allocator_type > allocator_traits;

            /// \p true if underlined allocator is \p std::allocator, \p false otherwise
            static constexpr bool const c_bStdAllocator = std::is_same< allocator_type, std::allocator<T>>::value;

            /// Element type
            typedef T   value_type;

            /// Analogue of operator new T(\p src... )
            template <typename... S>
            value_type *  New( S const&... src )
            {
                return Construct( allocator_traits::allocate( *this, 1, nullptr ), src... );
            }

            /// Analogue of <tt>operator new T( std::forward<Args>(args)... )</tt> (move semantics)
            template <typename... Args>
            value_type * MoveNew( Args&&... args )
            {
                return MoveConstruct( allocator_traits::allocate( *this, 1, nullptr ), std::forward<Args>(args)... );
            }

            /// Analogue of operator new T[\p nCount ]
            value_type * NewArray( size_t nCount )
            {
                value_type * p = allocator_traits::allocate( *this, nCount, nullptr );
                for ( size_t i = 0; i < nCount; ++i )
                    Construct( p + i );
                return p;
            }

            /// Analogue of operator new T[\p nCount ].
            /**
                Each item of array of type T is initialized by parameter \p src: T( src )
            */
            template <typename S>
            value_type * NewArray( size_t nCount, S const& src )
            {
                value_type * p = allocator_traits::allocate( *this, nCount, nullptr );
                for ( size_t i = 0; i < nCount; ++i )
                    Construct( p + i, src );
                return p;
            }

#       if CDS_COMPILER == CDS_COMPILER_INTEL
            //@cond
            value_type * NewBlock( size_t nSize )
            {
                return Construct( heap_alloc( nSize ));
            }
            //@endcond
#       endif
            /// Allocates block of memory of size at least \p nSize bytes.
            /**
                Internally, the block is allocated as an array of \p void* pointers,
                then \p Construct() method is called to initialize \p T.

                Precondition: <tt> nSize >= sizeof(T) </tt>
            */
            template <typename... S>
            value_type *  NewBlock( size_t nSize, S const&... src )
            {
                return Construct( heap_alloc( nSize ), src... );
            }

            /// Analogue of operator delete
            void Delete( value_type * p )
            {
                allocator_traits::destroy( *this, p );
                allocator_traits::deallocate( *this, p, 1 );
            }

            /// Analogue of operator delete []
            void Delete( value_type * p, size_t nCount )
            {
                 for ( size_t i = 0; i < nCount; ++i )
                     allocator_traits::destroy( *this, p + i );
                 allocator_traits::deallocate( *this, p, nCount );
            }

#       if CDS_COMPILER == CDS_COMPILER_INTEL
            //@cond
            value_type * Construct( void * p )
            {
                return new( p ) value_type;
            }
            //@endcond
#       endif
            /// Analogue of placement operator new( \p p ) T( src... )
            template <typename... S>
            value_type * Construct( void * p, S const&... src )
            {
                value_type * pv = new( p ) value_type( src... );
                return pv;
            }

            /// Analogue of placement <tt>operator new( p ) T( std::forward<Args>(args)... )</tt>
            template <typename... Args>
            value_type * MoveConstruct( void * p, Args&&... args )
            {
                value_type * pv = new( p ) value_type( std::forward<Args>(args)... );
                return pv;
            }

            /// Rebinds allocator to other type \p Q instead of \p T
            template <typename Q>
            struct rebind {
                typedef Allocator< Q, typename allocator_traits::template rebind_alloc<Q>> other ; ///< Rebinding result
            };

        private:
            //@cond
            void * heap_alloc( size_t nByteSize )
            {
                assert( nByteSize >= sizeof(value_type));

                size_t const nPtrSize = ( nByteSize + sizeof(void *) - 1 ) / sizeof(void *);
                typedef typename std::allocator_traits<allocator_type>::template rebind_alloc< void * > void_allocator;
                return void_allocator().allocate( nPtrSize );
            }
            //@endcond
        };

        /// Deferral removing of the object of type \p T. Helper class
        template <typename T, typename Alloc = CDS_DEFAULT_ALLOCATOR>
        struct deferral_deleter {
            typedef T           type            ; ///< Type
            typedef Alloc       allocator_type  ; ///< Allocator for removing

            /// Frees the object \p p
            /**
                Caveats: this function uses temporary object of type \ref cds::details::Allocator to free the node \p p.
                So, the node allocator should be stateless. It is standard requirement for \p std::allocator class objects.

                Do not use this function directly.
            */
            static void free( T * p )
            {
                Allocator<type, allocator_type> a;
                a.Delete( p );
            }
        };

    }    // namespace details
}    // namespace cds

#endif    // #ifndef CDSLIB_DETAILS_ALLOCATOR_H
