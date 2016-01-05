/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_GC_DETAILS_HP_ALLOC_H
#define CDSLIB_GC_DETAILS_HP_ALLOC_H

#include <cds/algo/atomic.h>
#include <cds/details/allocator.h>
#include <cds/gc/details/hp_type.h>

//@cond
namespace cds {
    namespace gc { namespace hp {
        // forwards
        class GarbageCollector;
        class ThreadGC;

    /// Hazard Pointer schema implementation details
    namespace details {

        /// Hazard pointer guard
        /**
            It is unsafe to use this class directly.
            Instead, the \p hp::guard class should be used.
        */
        class hp_guard : protected atomics::atomic < hazard_pointer >
        {
        public:
            typedef hazard_pointer   hazard_ptr;    ///< Hazard pointer type
        private:
            typedef atomics::atomic<hazard_ptr>  base_class;

        protected:
            template <class Allocator> friend class hp_allocator;

        public:
            hp_guard() CDS_NOEXCEPT
                : base_class( nullptr )
            {}
            ~hp_guard() CDS_NOEXCEPT
            {}

            /// Sets HP value. Guards pointer \p p from reclamation.
            /**
                Storing has release semantics.
                */
                template <typename T>
            T * operator =(T * p) CDS_NOEXCEPT
            {
                // We use atomic store with explicit memory order because other threads may read this hazard pointer concurrently
                set( p );
                return p;
            }

            std::nullptr_t operator=(std::nullptr_t) CDS_NOEXCEPT
            {
                clear();
                return nullptr;
            }

            /// Returns current value of hazard pointer
            /**
                Loading has acquire semantics
            */
            operator hazard_ptr() const CDS_NOEXCEPT
            {
                return get();
            }

            /// Returns current value of hazard pointer
            /**
                Loading has acquire semantics
            */
            hazard_ptr get( atomics::memory_order order = atomics::memory_order_acquire ) const CDS_NOEXCEPT
            {
                return base_class::load( order );
            }

            template <typename T>
            void set( T * p, atomics::memory_order order = atomics::memory_order_release ) CDS_NOEXCEPT
            {
                base_class::store( reinterpret_cast<hazard_ptr>(p), order );
            }

            /// Clears HP
            /**
                Clearing has relaxed semantics.
            */
            void clear( atomics::memory_order order = atomics::memory_order_release ) CDS_NOEXCEPT
            {
                // memory order is not necessary here
                base_class::store( nullptr, order );
            }
        };

        /// Array of hazard pointers.
        /**
            Array of hazard-pointer. Placing a pointer into this array guards the pointer against reclamation.
            Template parameter \p Count defines the size of hazard pointer array. \p Count parameter should not exceed
            GarbageCollector::getHazardPointerCount().

            It is unsafe to use this class directly. Instead, the \p hp::array should be used.

            While creating the object of \p hp_array class an array of size \p Count of hazard pointers is reserved by
            the HP Manager of current thread. The object's destructor cleans all of reserved hazard pointer and
            returns reserved HP to the HP pool of ThreadGC.

            Usually, it is not necessary to create an object of this class. The object of class ThreadGC contains
            the \p hp_array object and implements interface for HP setting and freeing.

            Template parameter:
                \li Count - capacity of array

        */
        template <size_t Count>
        class hp_array
        {
        public:
            typedef hazard_pointer  hazard_ptr_type;   ///< Hazard pointer type
            typedef hp_guard        atomic_hazard_ptr; ///< Element type of the array
            static CDS_CONSTEXPR const size_t c_nCapacity = Count ;   ///< Capacity of the array

        private:
            atomic_hazard_ptr *     m_arr               ;   ///< Hazard pointer array of size = \p Count
            template <class Allocator> friend class hp_allocator;

        public:
            /// Constructs uninitialized array.
            hp_array() CDS_NOEXCEPT
            {}

            /// Destructs object
            ~hp_array() CDS_NOEXCEPT
            {}

            /// Returns max count of hazard pointer for this array
            CDS_CONSTEXPR size_t capacity() const
            {
                return c_nCapacity;
            }

            /// Set hazard pointer \p nIndex. 0 <= \p nIndex < \p Count
            void set( size_t nIndex, hazard_ptr_type hzPtr ) CDS_NOEXCEPT
            {
                assert( nIndex < capacity() );
                m_arr[nIndex] = hzPtr;
            }

            /// Returns reference to hazard pointer of index \p nIndex (0 <= \p nIndex < \p Count)
            atomic_hazard_ptr& operator []( size_t nIndex ) CDS_NOEXCEPT
            {
                assert( nIndex < capacity() );
                return m_arr[nIndex];
            }

            /// Returns reference to hazard pointer of index \p nIndex (0 <= \p nIndex < \p Count) [const version]
            atomic_hazard_ptr& operator []( size_t nIndex ) const CDS_NOEXCEPT
            {
                assert( nIndex < capacity() );
                return m_arr[nIndex];
            }

            /// Clears (sets to \p nullptr) hazard pointer \p nIndex
            void clear( size_t nIndex ) CDS_NOEXCEPT
            {
                assert( nIndex < capacity() );
                m_arr[ nIndex ].clear();
            }
        };

        /// Allocator of hazard pointers for the thread
        /**
            The hazard pointer array is the free-list of unused hazard pointer for the thread.
            The array is managed as a stack.
            The max size (capacity) of array is defined at ctor time and cannot be changed during object's lifetime

            Each allocator object is thread-private.

            Template parameters:
                \li Allocator - memory allocator class, default is \ref CDS_DEFAULT_ALLOCATOR

            This helper class should not be used directly.
        */
        template <class Allocator = CDS_DEFAULT_ALLOCATOR >
        class hp_allocator
        {
        public:
            typedef hazard_pointer  hazard_ptr_type;    ///< type of hazard pointer
            typedef hp_guard        atomic_hazard_ptr;  ///< Atomic hazard pointer type
            typedef Allocator       allocator_type;     ///< allocator type

        private:
            typedef cds::details::Allocator< atomic_hazard_ptr, allocator_type > allocator_impl;

            atomic_hazard_ptr * m_arrHazardPtr  ;   ///< Array of hazard pointers
            size_t              m_nTop          ;   ///< The top of stack
            const size_t        m_nCapacity     ;   ///< Array capacity

        public:
            /// Default ctor
            explicit hp_allocator(
                size_t  nCapacity            ///< max count of hazard pointer per thread
                )
                : m_arrHazardPtr( alloc_array( nCapacity ) )
                , m_nCapacity( nCapacity )
            {
                make_free();
            }

            /// Dtor
            ~hp_allocator()
            {
                allocator_impl().Delete( m_arrHazardPtr, capacity() );
            }

            /// Get capacity of array
            size_t capacity() const CDS_NOEXCEPT
            {
                return m_nCapacity;
            }

            /// Get size of array. The size is equal to the capacity of array
            size_t size() const CDS_NOEXCEPT
            {
                return capacity();
            }

            /// Checks if all items are allocated
            bool isFull() const CDS_NOEXCEPT
            {
                return m_nTop == 0;
            }

            /// Allocates hazard pointer
            atomic_hazard_ptr& alloc()
            {
                assert( m_nTop > 0 );
                --m_nTop;
                return m_arrHazardPtr[m_nTop];
            }

            /// Frees previously allocated hazard pointer
            void free( atomic_hazard_ptr& hp ) CDS_NOEXCEPT
            {
                assert( m_nTop < capacity() );
                hp.clear();
                ++m_nTop;
            }

            /// Allocates hazard pointers array
            /**
                Allocates \p Count hazard pointers from array \p m_arrHazardPtr
                Returns initialized object \p arr
            */
            template <size_t Count>
            void alloc( hp_array<Count>& arr )
            {
                assert( m_nTop >= Count );
                m_nTop -= Count;
                arr.m_arr = m_arrHazardPtr + m_nTop;
            }

            /// Frees hazard pointer array
            /**
                Frees the array of hazard pointers allocated by previous call \p this->alloc.
            */
            template <size_t Count>
            void free( hp_array<Count> const& arr ) CDS_NOEXCEPT
            {
                CDS_UNUSED( arr );

                assert( m_nTop + Count <= capacity());
                for ( size_t i = m_nTop; i < m_nTop + Count; ++i )
                    m_arrHazardPtr[i].clear();
                m_nTop += Count;
            }

            /// Makes all HP free
            void clear() CDS_NOEXCEPT
            {
                make_free();
            }

            /// Returns to i-th hazard pointer
            atomic_hazard_ptr& operator []( size_t i ) CDS_NOEXCEPT
            {
                assert( i < capacity() );
                return m_arrHazardPtr[i];
            }

        private:
            void make_free() CDS_NOEXCEPT
            {
                for ( size_t i = 0; i < capacity(); ++i )
                    m_arrHazardPtr[i].clear();
                m_nTop = capacity();
            }

            atomic_hazard_ptr * alloc_array( size_t nCapacity )
            {
                return allocator_impl().NewArray( nCapacity );
            }
        };

    }}} // namespace gc::hp::details
}   // namespace cds
//@endcond

#endif // #ifndef CDSLIB_GC_DETAILS_HP_ALLOC_H
