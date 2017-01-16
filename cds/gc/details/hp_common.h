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

#ifndef CDSLIB_GC_DETAILS_HP_COMMON_H
#define CDSLIB_GC_DETAILS_HP_COMMON_H

#include <cds/algo/atomic.h>
#include <cds/gc/details/retired_ptr.h>

#ifdef CDS_ENABLE_HPSTAT
#   define CDS_HPSTAT( expr ) expr
#else
#   define CDS_HPSTAT( expr )
#endif

//@cond
namespace cds { namespace gc { namespace hp { namespace common {

    /// Hazard pointer type
    typedef void*   hazard_ptr;

    /// Retired pointer
    using cds::gc::details::retired_ptr;
    using cds::gc::make_retired_ptr;

    /// Hazard pointer guard
    class guard
    {
    public:
        guard() noexcept
            : hp_( nullptr )
            , next_( nullptr )
        {}

        template <typename T>
        T* operator=( T* ptr ) noexcept
        {
            set( ptr );
            return ptr;
        }

        std::nullptr_t operator=( std::nullptr_t ) noexcept
        {
            clear();
            return nullptr;
        }

        hazard_ptr get() const noexcept
        {
            return hp_.load( atomics::memory_order_acquire );
        }

        hazard_ptr get( atomics::memory_order order ) const noexcept
        {
            return hp_.load( order );
        }

        template <typename T>
        T* get_as() const noexcept
        {
            return reinterpret_cast<T*>( get() );
        }

        template <typename T>
        void set( T* ptr ) noexcept
        {
            hp_.store( reinterpret_cast<hazard_ptr>( ptr ), atomics::memory_order_release );
        }

        void clear( atomics::memory_order order ) noexcept
        {
            hp_.store( nullptr, order );
        }

        void clear() noexcept
        {
            clear( atomics::memory_order_release );
        }

    private:
        atomics::atomic<hazard_ptr>   hp_;

    public:
        guard* next_;   // free guard list
    };

    /// Array of guards
    template <size_t Capacity>
    class guard_array
    {
    public:
        static size_t const c_nCapacity = Capacity;

    public:
        guard_array()
            : arr_{ nullptr }
        {}

        static constexpr size_t capacity()
        {
            return c_nCapacity;
        }

        guard* operator[]( size_t idx ) const noexcept
        {
            assert( idx < capacity() );
            return arr_[idx];
        }

        template <typename T>
        void set( size_t idx, T* ptr ) noexcept
        {
            assert( idx < capacity());
            assert( arr_[idx] != nullptr );

            arr_[idx]->set( ptr );
        }

        void clear( size_t idx ) noexcept
        {
            assert( idx < capacity() );
            assert( arr_[idx] != nullptr );

            arr_[idx]->clear();
        }

        guard* release( size_t idx ) noexcept
        {
            assert( idx < capacity() );

            guard* g = arr_[idx];
            arr_[idx] = nullptr;
            return g;
        }

        void reset( size_t idx, guard* g ) noexcept
        {
            assert( idx < capacity() );
            assert( arr_[idx] == nullptr );

            arr_[idx] = g;
        }

    private:
        guard*  arr_[c_nCapacity];
    };


    /// Retired pointer disposer
    typedef void ( *disposer_func )( void* );

}}}} // namespace cds::gc::hp::common
//@endcond

#endif // #ifndef CDSLIB_GC_DETAILS_HP_COMMON_H


