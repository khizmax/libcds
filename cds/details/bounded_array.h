//$$CDS-header$$

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
            : m_arr( allocator_impl().NewArray( nCapacity ) )
            , m_nCapacity( nCapacity )
            {}

            ~bounded_array()
            {
                allocator_impl().Delete( m_arr, capacity() );
            }

            const value_type& operator []( size_t nItem ) const
            {
                assert( nItem < capacity() );
                return m_arr[nItem];
            }

            value_type& operator []( size_t nItem )
            {
                assert( nItem < capacity() );
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
