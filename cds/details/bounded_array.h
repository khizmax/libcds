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

        /// Upper bounded dynamic array
        /**
            BoundedArray is dynamic allocated C-array of item of type T with the interface like STL.
            The max size (capacity) of array is defined at ctor time and cannot be changed during object's lifetime

            \par Template parameters
                - \p T type of elements
                - \p Allocator dynamic memory allocator class (<tt>std::allocator</tt> semantics)

            This class is deprecated: it is based on std::vector and does not support non-copyable type \p T.
            See \ref bounded_array.
        */
        template <typename T, class Allocator = CDS_DEFAULT_ALLOCATOR >
        class BoundedArray: private std::vector< T, typename Allocator::template rebind<T>::other >
        {
        public:
            typedef T value_type ;    ///< value type stored in the array
            typedef Allocator allocator_type ;  ///< allocator type
            typedef std::vector<T, typename Allocator::template rebind<T>::other>   vector_type ; ///< underlying vector type

            typedef typename vector_type::iterator          iterator        ;    ///< item iterator
            typedef typename vector_type::const_iterator    const_iterator    ;    ///< item const iterator

        public:
            /// Default ctor
            explicit BoundedArray(
                size_t nCapacity            ///< capacity
            )
            {
                vector_type::resize( nCapacity );
                assert( size() == capacity() );
            }

            /// Ctor with item's initialization
            BoundedArray(
                size_t nCapacity,            ///< capacity of array
                const value_type& init,                ///< initial value of any item
                size_t nInitCount = 0        ///< how many items will be initialized; 0 - all items
            )
            {
                assert( nInitCount <= nCapacity );
                vector_type::resize( nCapacity );
                assign( nInitCount ? nInitCount : vector_type::capacity(), init );
                assert( size() == capacity() );
            }

            const value_type& operator []( size_t nItem ) const
            {
                return vector_type::operator[](nItem);
            }

            value_type& operator []( size_t nItem )
            {
                return vector_type::operator[](nItem);
            }

            size_t   size() const
            {
                return vector_type::size();
            }

            size_t   capacity() const
            {
                return vector_type::capacity();
            }

            /// Returns sizeof(T)
            static size_t itemSize()
            {
                return sizeof(T);
            }

            /// Returns pointer to the first item in the array
            value_type * top()
            {
                return & vector_type::front();
            }

            friend value_type * operator +( BoundedArray<value_type, allocator_type>& arr, size_t i )
            {
                return &( arr[i] );
            }

            /// Get begin iterator
            const_iterator  begin() const
            {
                return vector_type::begin();
            }
            iterator        begin()
            {
                return vector_type::begin();
            }

            /// Get end iterator
            const_iterator  end() const
            {
                return vector_type::end();
            }
            iterator        end()
            {
                return vector_type::end();
            }

            /// Get end iterator for \p nMax-th item
            const_iterator    end( size_t nMax )    const
            {
                assert( nMax <= vector_type::capacity());
                return vector_type::begin() + nMax;
            }
            iterator        end( size_t nMax )
            {
                assert( nMax <= vector_type::capacity());
                return vector_type::begin() + nMax;
            }
        };

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
