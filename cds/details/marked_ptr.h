// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_MARKED_PTR_H
#define CDSLIB_DETAILS_MARKED_PTR_H

#include <cds/algo/atomic.h>

namespace cds {
    namespace details {

        /// Marked pointer
        /**
            On the modern architectures, the default data alignment is 4 (for 32bit) or 8 byte for 64bit.
            Therefore, the least 2 or 3 bits of the pointer is always zero and can
            be used as a bitfield to store logical flags. This trick is widely used in
            lock-free programming to operate with the pointer and its flags atomically.

            Template parameters:
            - T - type of pointer
            - Bitmask - bitmask of least unused bits
        */
        template <typename T, int Bitmask>
        class marked_ptr
        {
            T *         m_ptr   ;   ///< pointer and its mark bits

        public:
            typedef T       value_type      ;       ///< type of value the class points to
            typedef T *     pointer_type    ;       ///< type of pointer
            static constexpr const uintptr_t bitmask = Bitmask;   ///< bitfield bitmask
            static constexpr const uintptr_t pointer_bitmask = ~bitmask; ///< pointer bitmask

        public:
            /// Constructs null marked pointer. The flag is cleared.
            constexpr marked_ptr() noexcept
                : m_ptr( nullptr )
            {}

            /// Constructs marked pointer with \p ptr value. The least bit(s) of \p ptr is the flag.
            constexpr explicit marked_ptr( value_type * ptr ) noexcept
                : m_ptr( ptr )
            {}

            /// Constructs marked pointer with \p ptr value and \p nMask flag.
            /**
                The \p nMask argument defines the OR-bits
            */
            marked_ptr( value_type * ptr, int nMask ) noexcept
                : m_ptr( ptr )
            {
                assert( bits() == 0 );
                *this |= nMask;
            }

            /// Copy constructor
            marked_ptr( marked_ptr const& src ) noexcept = default;
            /// Copy-assignment operator
            marked_ptr& operator =( marked_ptr const& p ) noexcept = default;
#       if !defined(CDS_DISABLE_DEFAULT_MOVE_CTOR)
            //@cond
            marked_ptr( marked_ptr&& src ) noexcept = default;
            marked_ptr& operator =( marked_ptr&& p ) noexcept = default;
            //@endcond
#       endif

            //TODO: make move ctor

        private:
            //@cond
            union pointer_cast {
                T *       ptr;
                uintptr_t n;

                pointer_cast(T * p) : ptr(p) {}
                pointer_cast(uintptr_t i) : n(i) {}
            };
            static uintptr_t   to_int( value_type * p ) noexcept
            {
                return pointer_cast(p).n;
            }

            static value_type * to_ptr( uintptr_t n ) noexcept
            {
                return pointer_cast(n).ptr;
            }

            uintptr_t   to_int() const noexcept
            {
                return to_int( m_ptr );
            }
            //@endcond

        public:
            /// Returns the pointer without mark bits (real pointer) const version
            value_type *        ptr() const noexcept
            {
                return to_ptr( to_int() & ~bitmask );
            }

            /// Returns the pointer and bits together
            value_type *        all() const noexcept
            {
                return m_ptr;
            }

            /// Returns the least bits of pointer according to \p Bitmask template argument of the class
            uintptr_t bits() const noexcept
            {
                return to_int() & bitmask;
            }

            /// Analogue for \ref ptr
            value_type * operator ->() const noexcept
            {
                return ptr();
            }

            /// Assignment operator sets markup bits to zero
            marked_ptr operator =( T * p ) noexcept
            {
                m_ptr = p;
                return *this;
            }

            /// Set LSB bits as <tt>bits() | nBits</tt>
            marked_ptr& operator |=( int nBits ) noexcept
            {
                assert( (nBits & pointer_bitmask) == 0 );
                m_ptr = to_ptr( to_int() | nBits );
                return *this;
            }

            /// Set LSB bits as <tt>bits() & nBits</tt>
            marked_ptr& operator &=( int nBits ) noexcept
            {
                assert( (nBits & pointer_bitmask) == 0 );
                m_ptr = to_ptr( to_int() & (pointer_bitmask | nBits));
                return *this;
            }

            /// Set LSB bits as <tt>bits() ^ nBits</tt>
            marked_ptr& operator ^=( int nBits ) noexcept
            {
                assert( (nBits & pointer_bitmask) == 0 );
                m_ptr = to_ptr( to_int() ^ nBits );
                return *this;
            }

            /// Returns <tt>p |= nBits</tt>
            friend marked_ptr operator |( marked_ptr p, int nBits) noexcept
            {
                p |= nBits;
                return p;
            }

            /// Returns <tt>p |= nBits</tt>
            friend marked_ptr operator |( int nBits, marked_ptr p ) noexcept
            {
                p |= nBits;
                return p;
            }

            /// Returns <tt>p &= nBits</tt>
            friend marked_ptr operator &( marked_ptr p, int nBits) noexcept
            {
                p &= nBits;
                return p;
            }

            /// Returns <tt>p &= nBits</tt>
            friend marked_ptr operator &( int nBits, marked_ptr p ) noexcept
            {
                p &= nBits;
                return p;
            }

            /// Returns <tt>p ^= nBits</tt>
            friend marked_ptr operator ^( marked_ptr p, int nBits) noexcept
            {
                p ^= nBits;
                return p;
            }
            /// Returns <tt>p ^= nBits</tt>
            friend marked_ptr operator ^( int nBits, marked_ptr p ) noexcept
            {
                p ^= nBits;
                return p;
            }

            /// Inverts LSBs of pointer \p p
            friend marked_ptr operator ~( marked_ptr p ) noexcept
            {
                return p ^ marked_ptr::bitmask;
            }


            /// Comparing two marked pointer including its mark bits
            friend bool operator ==( marked_ptr p1, marked_ptr p2 ) noexcept
            {
                return p1.all() == p2.all();
            }

            /// Comparing marked pointer and raw pointer, mark bits of \p p1 is ignored
            friend bool operator ==( marked_ptr p1, value_type const * p2 ) noexcept
            {
                return p1.ptr() == p2;
            }

            /// Comparing marked pointer and raw pointer, mark bits of \p p2 is ignored
            friend bool operator ==( value_type const * p1, marked_ptr p2 ) noexcept
            {
                return p1 == p2.ptr();
            }

            /// Comparing two marked pointer including its mark bits
            friend bool operator !=( marked_ptr p1, marked_ptr p2 ) noexcept
            {
                return p1.all() != p2.all();
            }

            /// Comparing marked pointer and raw pointer, mark bits of \p p1 is ignored
            friend bool operator !=( marked_ptr p1, value_type const * p2 ) noexcept
            {
                return p1.ptr() != p2;
            }

            /// Comparing marked pointer and raw pointer, mark bits of \p p2 is ignored
            friend bool operator !=( value_type const * p1, marked_ptr p2 ) noexcept
            {
                return p1 != p2.ptr();
            }

            //@cond
            /// atomic< marked_ptr< T, Bitmask > > support
            T *& impl_ref() noexcept
            {
                return m_ptr;
            }
            //@endcond
        };
    }   // namespace details

}   // namespace cds

//@cond
CDS_CXX11_ATOMIC_BEGIN_NAMESPACE

    template <typename T, int Bitmask >
    class atomic< cds::details::marked_ptr<T, Bitmask> >
    {
    private:
        typedef cds::details::marked_ptr<T, Bitmask> marked_ptr;
        typedef atomics::atomic<T *>  atomic_impl;

        atomic_impl m_atomic;
    public:
        bool is_lock_free() const volatile noexcept
        {
            return m_atomic.is_lock_free();
        }
        bool is_lock_free() const noexcept
        {
            return m_atomic.is_lock_free();
        }

        void store(marked_ptr val, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            m_atomic.store( val.all(), order );
        }
        void store(marked_ptr val, memory_order order = memory_order_seq_cst) noexcept
        {
            m_atomic.store( val.all(), order );
        }

        marked_ptr load(memory_order order = memory_order_seq_cst) const volatile noexcept
        {
            return marked_ptr( m_atomic.load( order ));
        }
        marked_ptr load(memory_order order = memory_order_seq_cst) const noexcept
        {
            return marked_ptr( m_atomic.load( order ));
        }

        operator marked_ptr() const volatile noexcept
        {
            return load();
        }
        operator marked_ptr() const noexcept
        {
            return load();
        }

        marked_ptr exchange(marked_ptr val, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return marked_ptr( m_atomic.exchange( val.all(), order ));
        }
        marked_ptr exchange(marked_ptr val, memory_order order = memory_order_seq_cst) noexcept
        {
            return marked_ptr( m_atomic.exchange( val.all(), order ));
        }

        bool compare_exchange_weak(marked_ptr& expected, marked_ptr desired, memory_order success_order, memory_order failure_order) volatile noexcept
        {
            return m_atomic.compare_exchange_weak( expected.impl_ref(), desired.all(), success_order, failure_order );
        }
        bool compare_exchange_weak(marked_ptr& expected, marked_ptr desired, memory_order success_order, memory_order failure_order) noexcept
        {
            return m_atomic.compare_exchange_weak( expected.impl_ref(), desired.all(), success_order, failure_order );
        }
        bool compare_exchange_strong(marked_ptr& expected, marked_ptr desired, memory_order success_order, memory_order failure_order) volatile noexcept
        {
            return m_atomic.compare_exchange_strong( expected.impl_ref(), desired.all(), success_order, failure_order );
        }
        bool compare_exchange_strong(marked_ptr& expected, marked_ptr desired, memory_order success_order, memory_order failure_order) noexcept
        {
            return m_atomic.compare_exchange_strong( expected.impl_ref(), desired.all(), success_order, failure_order );
        }
        bool compare_exchange_weak(marked_ptr& expected, marked_ptr desired, memory_order success_order = memory_order_seq_cst) volatile noexcept
        {
            return m_atomic.compare_exchange_weak( expected.impl_ref(), desired.all(), success_order );
        }
        bool compare_exchange_weak(marked_ptr& expected, marked_ptr desired, memory_order success_order = memory_order_seq_cst) noexcept
        {
            return m_atomic.compare_exchange_weak( expected.impl_ref(), desired.all(), success_order );
        }
        bool compare_exchange_strong(marked_ptr& expected, marked_ptr desired, memory_order success_order = memory_order_seq_cst) volatile noexcept
        {
            return m_atomic.compare_exchange_strong( expected.impl_ref(), desired.all(), success_order );
        }
        bool compare_exchange_strong(marked_ptr& expected, marked_ptr desired, memory_order success_order = memory_order_seq_cst) noexcept
        {
            return m_atomic.compare_exchange_strong( expected.impl_ref(), desired.all(), success_order );
        }

        constexpr atomic() noexcept
            : m_atomic( nullptr )
        {}

        constexpr explicit atomic(marked_ptr val) noexcept
            : m_atomic( val.all())
        {}
        constexpr explicit atomic(T * p) noexcept
            : m_atomic( p )
        {}

        atomic(const atomic&) = delete;
        atomic& operator=(const atomic&) = delete;

#if !(CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION < CDS_COMPILER_MSVC15)
        // MSVC12, MSVC14, MSVC14.1: warning C4522: multiple assignment operators specified
        atomic& operator=(const atomic&) volatile = delete;
        marked_ptr operator=(marked_ptr val) volatile noexcept
        {
            store( val );
            return val;
        }
#endif
        marked_ptr operator=(marked_ptr val) noexcept
        {
            store( val );
            return val;
        }
    };

CDS_CXX11_ATOMIC_END_NAMESPACE
//@endcond

#endif  // #ifndef CDSLIB_DETAILS_MARKED_PTR_H
