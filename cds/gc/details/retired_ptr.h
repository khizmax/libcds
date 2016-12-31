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

#ifndef CDSLIB_GC_DETAILS_RETIRED_PTR_H
#define CDSLIB_GC_DETAILS_RETIRED_PTR_H

#include <cds/details/defs.h>
#include <cds/details/static_functor.h>

//@cond
namespace cds { namespace gc {
    /// Common implementation details for any GC
    namespace details {

        /// Pointer to function to free (destruct and deallocate) retired pointer of specific type
        typedef void (* free_retired_ptr_func )( void * );

        /// Retired pointer
        /**
            Pointer to an object that is ready to delete.
        */
        struct retired_ptr {
            /// Pointer type
            typedef void *          pointer;

            union {
                pointer                 m_p;        ///< retired pointer
                uintptr_t               m_n;
            };
            free_retired_ptr_func   m_funcFree; ///< pointer to the destructor function

            /// Comparison of two retired pointers
            static bool less( const retired_ptr& p1, const retired_ptr& p2 ) CDS_NOEXCEPT
            {
                return p1.m_p < p2.m_p;
            }

            /// Default ctor initializes pointer to \p nullptr
            retired_ptr() CDS_NOEXCEPT
                : m_p( nullptr )
                , m_funcFree( nullptr )
            {}

            /// Ctor
            retired_ptr( pointer p, free_retired_ptr_func func ) CDS_NOEXCEPT
                : m_p( p )
                , m_funcFree( func )
            {}

            /// Typecasting ctor
            template <typename T>
            retired_ptr( T * p, void (* pFreeFunc)(T *)) CDS_NOEXCEPT
                : m_p( reinterpret_cast<pointer>(p))
                , m_funcFree( reinterpret_cast< free_retired_ptr_func >( pFreeFunc ))
            {}

            /// Assignment operator
            retired_ptr& operator =( retired_ptr const& s) CDS_NOEXCEPT
            {
                m_p = s.m_p;
                m_funcFree = s.m_funcFree;
                return *this;
            }

            /// Invokes destructor function for the pointer
            void free()
            {
                assert( m_funcFree );
                assert( m_p );
                m_funcFree( m_p );

                CDS_STRICT_DO( clear());
            }

            /// Checks if the retired pointer is not empty
            explicit operator bool() const CDS_NOEXCEPT
            {
                return m_p != nullptr;
            }

            /// Clears retired pointer without \p free() call
            void clear()
            {
                m_p = nullptr;
                m_funcFree = nullptr;
            }
        };

        static inline bool operator <( const retired_ptr& p1, const retired_ptr& p2 ) CDS_NOEXCEPT
        {
            return retired_ptr::less( p1, p2 );
        }

        static inline bool operator ==( const retired_ptr& p1, const retired_ptr& p2 ) CDS_NOEXCEPT
        {
            return p1.m_p == p2.m_p;
        }

        static inline bool operator !=( const retired_ptr& p1, const retired_ptr& p2 ) CDS_NOEXCEPT
        {
            return !(p1 == p2);
        }
    }  // namespace details

    template <typename Func, typename T>
    static inline cds::gc::details::retired_ptr make_retired_ptr( T * p )
    {
        return cds::gc::details::retired_ptr( p, cds::details::static_functor<Func, T>::call );
    }

}}   // namespace cds::gc
//@endcond

#endif // #ifndef CDSLIB_GC_DETAILS_RETIRED_PTR_H
