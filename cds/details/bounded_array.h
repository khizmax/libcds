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

#ifndef CDSLIB_IMPL_BOUNDED_ARRAY_H
#define CDSLIB_IMPL_BOUNDED_ARRAY_H

/*
    Dynamic non-growing array

    Editions:
        2008.03.08    Maxim.Khiszinsky    Created
*/

#include <cds/details/allocator.h>
#include <vector>
#include <assert.h>

//@cond
namespace cds {
    namespace details {
        /// Bounded dynamic array
        /**
            The class template is intended for storing fixed-size sequences of objects.
            Array capacity is constant and cannot be changed after creation of object of the class.
            It is suitable for managing objects of non-copyable type \p T.

            \par Template parameters
                - \p T type of elements
                - \p Allocator dynamic memory allocator class (<tt>std::allocator</tt> semantics)

        */
        template <typename T, class Allocator = CDS_DEFAULT_ALLOCATOR >
        class bounded_array
        {
        public:
            typedef T value_type ;    ///< value type stored in the array
            typedef Allocator allocator_type ;  ///< allocator type

            typedef value_type *          iterator          ; ///< item iterator
            typedef value_type const *    const_iterator    ; ///< item const iterator

        private:
            typedef cds::details::Allocator< T, allocator_type> allocator_impl;

            value_type *    m_arr;
            const size_t    m_nCapacity;

        public:
            /// Default ctor
            explicit bounded_array(
                size_t nCapacity            ///< capacity
            )
            : m_arr( allocator_impl().NewArray( nCapacity ))
            , m_nCapacity( nCapacity )
            {}

            ~bounded_array()
            {
                allocator_impl().Delete( m_arr, capacity());
            }

            const value_type& operator []( size_t nItem ) const
            {
                assert( nItem < capacity());
                return m_arr[nItem];
            }

            value_type& operator []( size_t nItem )
            {
                assert( nItem < capacity());
                return m_arr[nItem];
            }

            size_t   size() const CDS_NOEXCEPT
            {
                return capacity();
            }

            size_t   capacity() const CDS_NOEXCEPT
            {
                return m_nCapacity;
            }

            /// Returns pointer to the first item in the array
            value_type * top() CDS_NOEXCEPT
            {
                return m_arr;
            }

            /// Get begin iterator
            const_iterator  begin() const CDS_NOEXCEPT
            {
                return m_arr;
            }
            iterator        begin() CDS_NOEXCEPT
            {
                return m_arr;
            }

            /// Get end iterator
            const_iterator  end() const CDS_NOEXCEPT
            {
                return begin() + capacity();
            }
            iterator        end() CDS_NOEXCEPT
            {
                return begin() + capacity();
            }
        };

    }    // namespace details
}    // namespace cds
//@endcond

#endif    // #ifndef CDSLIB_IMPL_BOUNDED_ARRAY_H
