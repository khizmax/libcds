//$$CDS-header$$

#ifndef __CDS_DETAILS_ALLOCATOR_H
#define __CDS_DETAILS_ALLOCATOR_H

/*
    Allocator class for the library. Supports allocating and constructing of objects

    Editions:
        2008.03.08    Maxim.Khiszinsky    Created
*/

#include <cds/details/defs.h>
#include <cds/user_setup/allocator.h>
#include <cds/details/std/type_traits.h>
#include <memory>
#include <boost/type_traits/has_trivial_destructor.hpp>

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
                        , typename Alloc::template rebind<T>::other
                     >::type
        {
        public:
            /// Underlying allocator type
            typedef typename std::conditional<
                std::is_same< T, typename Alloc::value_type>::value
                , Alloc
                , typename Alloc::template rebind<T>::other
            >::type allocator_type;

            /// Element type
            typedef T   value_type;

#       ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
            /// Analogue of operator new T(\p src... )
            template <typename... S>
            value_type *  New( S const&... src )
            {
                return Construct( allocator_type::allocate(1), src... );
            }
#       else
            //@cond
            /// Analogue of operator new T
            value_type *  New()
            {
                return Construct( allocator_type::allocate(1) );
            }

            /// Analogue of operator new T(\p src )
            template <typename S>
            value_type *  New( S const& src )
            {
                return Construct( allocator_type::allocate(1), src );
            }

            /// Analogue of operator new T( \p s1, \p s2 )
            template <typename S1, typename S2>
            value_type *  New( S1 const& s1, S2 const& s2 )
            {
                return Construct( allocator_type::allocate(1), s1, s2 );
            }

            /// Analogue of operator new T( \p s1, \p s2, \p s3 )
            template <typename S1, typename S2, typename S3>
            value_type *  New( S1 const& s1, S2 const& s2, S3 const& s3 )
            {
                return Construct( allocator_type::allocate(1), s1, s2, s3 );
            }
            //@endcond
#       endif

#       ifdef CDS_EMPLACE_SUPPORT
            /// Analogue of <tt>operator new T( std::forward<Args>(args)... )</tt> (move semantics)
            /**
                This function is available only for compiler that supports
                variadic template and move semantics
            */
            template <typename... Args>
            value_type * MoveNew( Args&&... args )
            {
                return MoveConstruct( allocator_type::allocate(1), std::forward<Args>(args)... );
            }
#       endif


            /// Analogue of operator new T[\p nCount ]
            value_type * NewArray( size_t nCount )
            {
                value_type * p = allocator_type::allocate( nCount );
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
                value_type * p = allocator_type::allocate( nCount );
                for ( size_t i = 0; i < nCount; ++i )
                    Construct( p + i, src );
                return p;
            }

#       ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
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
#       else
            //@cond
            value_type * NewBlock( size_t nSize )
            {
                return Construct( heap_alloc( nSize ));
            }
            template <typename S>
            value_type * NewBlock( size_t nSize, S const& arg )
            {
                return Construct( heap_alloc( nSize ), arg );
            }
            template <typename S1, typename S2>
            value_type * NewBlock( size_t nSize, S1 const& arg1, S2 const& arg2 )
            {
                return Construct( heap_alloc( nSize ), arg1, arg2 );
            }
            template <typename S1, typename S2, typename S3>
            value_type * NewBlock( size_t nSize, S1 const& arg1, S2 const& arg2, S3 const& arg3 )
            {
                return Construct( heap_alloc( nSize ), arg1, arg2, arg3 );
            }
            //@endcond
#       endif

            /// Analogue of operator delete
            void Delete( value_type * p )
            {
                allocator_type::destroy( p );
                allocator_type::deallocate( p, 1 );
            }

            /// Analogue of operator delete []
            void Delete( value_type * p, size_t nCount )
            {
                 for ( size_t i = 0; i < nCount; ++i )
                     allocator_type::destroy( p + i );
                allocator_type::deallocate( p, nCount );
            }

#       ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
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
                return new( p ) value_type( src... );
            }
#       else
            //@cond
            /// Analogue of placement operator new( \p p ) T
            value_type * Construct( void * p )
            {
                return new( p ) value_type;
            }


            /// Analogue of placement operator new( \p p ) T( \p src )
            template <typename S>
            value_type * Construct( void * p, S const& src )
            {
                return new( p ) value_type( src );
            }

            /// Analogue of placement operator new( \p p ) T( \p s1, \p s2 )
            template <typename S1, typename S2>
            value_type *  Construct( void * p, S1 const& s1, S2 const& s2 )
            {
                return new( p ) value_type( s1, s2 );
            }

            /// Analogue of placement operator new( \p p ) T( \p s1, \p s2, \p s3 )
            template <typename S1, typename S2, typename S3>
            value_type *  Construct( void * p, S1 const& s1, S2 const& s2, S3 const& s3 )
            {
                return new( p ) value_type( s1, s2, s3 );
            }
            //@endcond
#       endif

#       ifdef CDS_EMPLACE_SUPPORT
            /// Analogue of placement <tt>operator new( p ) T( std::forward<Args>(args)... )</tt>
            /**
                This function is available only for compiler that supports
                variadic template and move semantics
            */
            template <typename... Args>
            value_type * MoveConstruct( void * p, Args&&... args )
            {
                return new( p ) value_type( std::forward<Args>(args)... );
            }
#       endif

            /// Rebinds allocator to other type \p Q instead of \p T
            template <typename Q>
            struct rebind {
                typedef Allocator< Q, typename Alloc::template rebind<Q>::other >    other ; ///< Rebinding result
            };

        private:
            //@cond
            void * heap_alloc( size_t nByteSize )
            {
                assert( nByteSize >= sizeof(value_type));

                size_t const nPtrSize = ( nByteSize + sizeof(void *) - 1 ) / sizeof(void *);
                typedef typename allocator_type::template rebind< void * >::other void_allocator;
                return void_allocator().allocate( nPtrSize );
            }
            //@endcond
        };

        //@cond
        namespace {
            template <class T>
            static inline void impl_call_dtor(T* p, boost::false_type const&)
            {
                p->T::~T();
            }

            template <class T>
            static inline void impl_call_dtor(T* p, boost::true_type const&)
            {}
        }
        //@endcond

        /// Helper function to call destructor of type T
        /**
            This function is empty for the type T that has trivial destructor.
        */
        template <class T>
        static inline void call_dtor( T* p )
        {
            impl_call_dtor( p, ::boost::has_trivial_destructor<T>() );
        }


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
                Allocator<T, Alloc> a;
                a.Delete( p );
            }
        };

    }    // namespace details
}    // namespace cds

#endif    // #ifndef __CDS_DETAILS_ALLOCATOR_H
