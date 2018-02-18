// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_X86_CXX11_ATOMIC32_H
#define CDSLIB_COMPILER_GCC_X86_CXX11_ATOMIC32_H

#include <cstdint>
#include <cds/details/is_aligned.h>

//@cond
namespace cds { namespace cxx11_atomic {
    namespace platform { inline namespace gcc { inline namespace x86 {

        static inline void fence_before( memory_order order ) noexcept
        {
            switch(order) {
            case memory_order_relaxed:
            case memory_order_acquire:
            case memory_order_consume:
                break;
            case memory_order_release:
            case memory_order_acq_rel:
                CDS_COMPILER_RW_BARRIER;
                break;
            case memory_order_seq_cst:
                CDS_COMPILER_RW_BARRIER;
                break;
            }
        }

        static inline void fence_after( memory_order order ) noexcept
        {
            switch(order) {
            case memory_order_acquire:
            case memory_order_acq_rel:
                CDS_COMPILER_RW_BARRIER;
                break;
            case memory_order_relaxed:
            case memory_order_consume:
            case memory_order_release:
                break;
            case memory_order_seq_cst:
                CDS_COMPILER_RW_BARRIER;
                break;
            }
        }


        static inline void fence_after_load(memory_order order) noexcept
        {
            switch(order) {
            case memory_order_relaxed:
            case memory_order_release:
                break;
            case memory_order_acquire:
            case memory_order_acq_rel:
                CDS_COMPILER_RW_BARRIER;
                break;
            case memory_order_consume:
                break;
            case memory_order_seq_cst:
                __asm__ __volatile__ ( "mfence" ::: "memory"  );
                break;
            default:;
            }
        }

        //-----------------------------------------------------------------------------
        // fences
        //-----------------------------------------------------------------------------
        static inline void thread_fence(memory_order order) noexcept
        {
            switch(order)
            {
                case memory_order_relaxed:
                case memory_order_consume:
                    break;
                case memory_order_release:
                case memory_order_acquire:
                case memory_order_acq_rel:
                    CDS_COMPILER_RW_BARRIER;
                    break;
                case memory_order_seq_cst:
                    __asm__ __volatile__ ( "mfence" ::: "memory"  );
                    break;
                default:;
            }
        }

        static inline void signal_fence(memory_order order) noexcept
        {
            // C++11: 29.8.8: only compiler optimization, no hardware instructions
            switch(order)
            {
                case memory_order_relaxed:
                    break;
                case memory_order_consume:
                case memory_order_release:
                case memory_order_acquire:
                case memory_order_acq_rel:
                case memory_order_seq_cst:
                    CDS_COMPILER_RW_BARRIER;
                    break;
                default:;
            }
        }

        //-----------------------------------------------------------------------------
        // 8bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline bool cas8_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );

            T prev = expected;
            fence_before(mo_success);
            __asm__ __volatile__  (
                "lock ; cmpxchgb %[desired], %[pDest]"
                : [prev] "+a" (prev), [pDest] "+m" (*pDest)
                : [desired] "q" (desired)
                );
            bool success = (prev == expected);
            expected = prev;
            if (success)
                fence_after(mo_success);
            else
                fence_after(mo_fail);
            return success;
        }

        template <typename T>
        static inline bool cas8_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas8_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        template <typename T>
        static inline T exchange8( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );

            fence_before(order);
            __asm__ __volatile__  (
                "xchgb %[v], %[pDest]"
                : [v] "+q" (v), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return v;
        }

        template <typename T>
        static inline void store8( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest != NULL );

            if ( order != memory_order_seq_cst ) {
                fence_before( order );
                *pDest = src;
            }
            else {
                exchange8( pDest, src, order );
            }
        }

        template <typename T>
        static inline T load8( T volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order == memory_order_seq_cst
                );
            assert( pSrc != NULL );

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }

#       define CDS_ATOMIC_fetch8_add_defined
        template <typename T>
        static inline T fetch8_add( T volatile * pDest, T val, memory_order order ) noexcept
        {
            fence_before(order);
            __asm__ __volatile__  (
                "lock ; xaddb %[val], %[pDest]"
                : [val] "+q" (val), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return val;
        }

#       define CDS_ATOMIC_fetch8_sub_defined
        template <typename T>
        static inline T fetch8_sub( T volatile * pDest, T val, memory_order order ) noexcept
        {
            fence_before(order);
            __asm__ __volatile__  (
                "negb %[val] ; \n"
                "lock ; xaddb %[val], %[pDest]"
                : [val] "+q" (val), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return val;
        }

        //-----------------------------------------------------------------------------
        // atomic flag primitives
        //-----------------------------------------------------------------------------

        typedef bool atomic_flag_type;
        static inline bool atomic_flag_tas( atomic_flag_type volatile * pFlag, memory_order order ) noexcept
        {
            return exchange8( pFlag, true, order );
        }

        static inline void atomic_flag_clear( atomic_flag_type volatile * pFlag, memory_order order ) noexcept
        {
            store8( pFlag, false, order );
        }

        //-----------------------------------------------------------------------------
        // 16bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T exchange16( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 2 ));

            fence_before(order);
            __asm__ __volatile__  (
                "xchgw %[v], %[pDest]"
                : [v] "+q" (v), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return v;
        }

        template <typename T>
        static inline void store16( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest != NULL );
            assert( cds::details::is_aligned( pDest, 2 ));

            if ( order != memory_order_seq_cst ) {
                fence_before( order );
                *pDest = src;
            }
            else {
                exchange16( pDest, src, order );
            }
        }

        template <typename T>
        static inline T load16( T volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc != NULL );
            assert( cds::details::is_aligned( pSrc, 2 ));

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }

        template <typename T>
        static inline bool cas16_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 2 ));

            T prev = expected;
            fence_before(mo_success);
            __asm__ __volatile__  (
                "lock ; cmpxchgw %[desired], %[pDest]"
                : [prev] "+a" (prev), [pDest] "+m" (*pDest)
                : [desired] "q" (desired)
                );
            bool success = prev == expected;
            if (success)
                fence_after(mo_success);
            else {
                fence_after(mo_fail);
                expected = prev;
            }

            return success;
        }

        template <typename T>
        static inline bool cas16_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas16_strong( pDest, expected, desired, mo_success, mo_fail );
        }

#       define CDS_ATOMIC_fetch16_add_defined
        template <typename T>
        static inline T fetch16_add( T volatile * pDest, T val, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 2 ));

            fence_before(order);
            __asm__ __volatile__  (
                "lock ; xaddw %[val], %[pDest]"
                : [val] "+q" (val), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return val;
        }

#       define CDS_ATOMIC_fetch16_sub_defined
        template <typename T>
        static inline T fetch16_sub( T volatile * pDest, T val, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 2 ));

            fence_before(order);
            __asm__ __volatile__  (
                "negw %[val] ; \n"
                "lock ; xaddw %[val], %[pDest]"
                : [val] "+q" (val), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return val;
        }

        //-----------------------------------------------------------------------------
        // 32bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T exchange32( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 4 ));

            fence_before(order);
            __asm__ __volatile__  (
                "xchgl %[v], %[pDest]"
                : [v] "+r" (v), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return v;
        }

        template <typename T>
        static inline void store32( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest != NULL );
            assert( cds::details::is_aligned( pDest, 4 ));

            if ( order != memory_order_seq_cst ) {
                fence_before( order );
                *pDest = src;
            }
            else {
                exchange32( pDest, src, order );
            }
        }

        template <typename T>
        static inline T load32( T volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc != NULL );
            assert( cds::details::is_aligned( pSrc, 4 ));

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }

        template <typename T>
        static inline bool cas32_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 4 ));

            T prev = expected;
            fence_before(mo_success);
            __asm__ __volatile__  (
                "lock ; cmpxchgl %[desired], %[pDest]"
                : [prev] "+a" (prev), [pDest] "+m" (*pDest)
                : [desired] "r" (desired)
                );
            bool success = prev == expected;
            if (success)
                fence_after(mo_success);
            else {
                fence_after(mo_fail);
                expected = prev;
            }
            return success;
        }

        template <typename T>
        static inline bool cas32_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas32_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        // fetch_xxx may be emulated via cas32
        // If the platform has special fetch_xxx instruction
        // then it should define CDS_ATOMIC_fetch32_xxx_defined macro

#       define CDS_ATOMIC_fetch32_add_defined
        template <typename T>
        static inline T fetch32_add( T volatile * pDest, T v, memory_order order) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 4 ));

            fence_before(order);
            __asm__ __volatile__  (
                "lock ; xaddl %[v], %[pDest]"
                : [v] "+r" (v), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return v;
        }

#       define CDS_ATOMIC_fetch32_sub_defined
        template <typename T>
        static inline T fetch32_sub( T volatile * pDest, T v, memory_order order) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 4 ));

            fence_before(order);
            __asm__ __volatile__  (
                "negl   %[v] ; \n"
                "lock ; xaddl %[v], %[pDest]"
                : [v] "+r" (v), [pDest] "+m" (*pDest)
                );
            fence_after(order);
            return v;
        }

    }}} // namespace platform::gcc::x86
}}  // namespace cds::cxx11_atomic
//@endcond

#endif // #ifndef CDSLIB_COMPILER_GCC_X86_CXX11_ATOMIC32_H
