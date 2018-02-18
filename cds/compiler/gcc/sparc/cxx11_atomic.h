// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_SPARC_CXX11_ATOMIC_H
#define CDSLIB_COMPILER_GCC_SPARC_CXX11_ATOMIC_H

#include <cstdint>

/*
    Source:

    1. [Doug Lea "JSR-133 Cookbook for Compiler Writers]:

        Acquire semantics: load; LoadLoad+LoadStore
        Release semantics: LoadStore+StoreStore; store

    2. boost::atomic library by Helge Bahman
    3. OpenSparc source code
*/

#if CDS_OS_TYPE == CDS_OS_LINUX
#   define CDS_SPARC_RMO_MEMORY_MODEL
#endif

#define CDS_SPARC_MB_FULL    "membar #Sync \n\t"
#ifdef CDS_SPARC_RMO_MEMORY_MODEL
        // RMO memory model (Linux only?..) Untested
#   define CDS_SPARC_MB_LL_LS       "membar #LoadLoad|#LoadStore \n\t"
#   define CDS_SPARC_MB_LS_SS       "membar #LoadStore|#StoreStore \n\t"
#   define CDS_SPARC_MB_LL_LS_SS    "membar #LoadLoad|#LoadStore|#StoreStore \n\t"
#else
        // TSO memory model (default; Solaris uses this model)
#   define CDS_SPARC_MB_LL_LS
#   define CDS_SPARC_MB_LS_SS
#   define CDS_SPARC_MB_LL_LS_SS
#endif

#define CDS_SPARC_MB_ACQ        CDS_SPARC_MB_LL_LS
#define CDS_SPARC_MB_REL        CDS_SPARC_MB_LS_SS
#define CDS_SPARC_MB_ACQ_REL    CDS_SPARC_MB_LL_LS_SS
#define CDS_SPARC_MB_SEQ_CST    CDS_SPARC_MB_FULL

//@cond
namespace cds { namespace cxx11_atomic {
    namespace platform { inline namespace gcc { inline namespace Sparc {

        static inline void fence_before( memory_order order ) noexcept
        {
            switch(order) {
            case memory_order_relaxed:
            case memory_order_acquire:
            case memory_order_consume:
                break;
            case memory_order_release:
            case memory_order_acq_rel:
                __asm__ __volatile__ ( "" CDS_SPARC_MB_REL ::: "memory" );
                break;
            case memory_order_seq_cst:
                __asm__ __volatile__ ( "" CDS_SPARC_MB_FULL ::: "memory" );
                break;
            }
        }

        static inline void fence_after( memory_order order ) noexcept
        {
            switch(order) {
            case memory_order_relaxed:
            case memory_order_consume:
            case memory_order_release:
                break;
            case memory_order_acquire:
            case memory_order_acq_rel:
                __asm__ __volatile__ ( "" CDS_SPARC_MB_ACQ ::: "memory" );
                break;
            case memory_order_seq_cst:
                __asm__ __volatile__ ( "" CDS_SPARC_MB_FULL ::: "memory" );
                break;
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
                case memory_order_acquire:
                    __asm__ __volatile__ ( "" CDS_SPARC_MB_ACQ ::: "memory" );
                    break;
                case memory_order_release:
                    __asm__ __volatile__ ( "" CDS_SPARC_MB_REL ::: "memory" );
                    break;
                case memory_order_acq_rel:
                    __asm__ __volatile__ ( "" CDS_SPARC_MB_ACQ_REL ::: "memory" );
                    break;
                case memory_order_seq_cst:
                    __asm__ __volatile__ ( "" CDS_SPARC_MB_SEQ_CST ::: "memory"  );
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
        // atomic flag primitives
        //-----------------------------------------------------------------------------

        typedef unsigned char atomic_flag_type;
        static inline bool atomic_flag_tas( atomic_flag_type volatile * pFlag, memory_order order ) noexcept
        {
            atomic_flag_type fCur;
            fence_before( order );
            __asm__ __volatile__(
                "ldstub    [%[pFlag]], %[fCur] \n\t"
                : [fCur] "=r"(fCur)
                : [pFlag] "r"(pFlag)
                : "memory", "cc"
                );
            fence_after( order );
            return fCur != 0;
        }

        static inline void atomic_flag_clear( atomic_flag_type volatile * pFlag, memory_order order ) noexcept
        {
            fence_before( order );
            __asm__ __volatile__(
                CDS_SPARC_MB_REL
                "stub    %%g0, [%[pFlag]] \n\t"
                :: [pFlag] "r"(pFlag)
                : "memory"
            );
            fence_after( order );
        }

        //-----------------------------------------------------------------------------
        // 32bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline void store32( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );

            fence_before(order);
            *pDest = src;
            fence_after(order);
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
            assert( pSrc );

            fence_before(order);
            T v = *pSrc;
            fence_after(order);
            return v;
        }

        template <typename T>
        static inline bool cas32_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( pDest );

            fence_before( mo_success );
            __asm__ __volatile__(
                "cas [%[pDest]], %[expected], %[desired]"
                : [desired] "+r" (desired)
                : [pDest] "r" (pDest), [expected] "r" (expected)
                : "memory"
                );

            // desired contains current value

            bool bSuccess = desired == expected;
            if ( bSuccess )
                fence_after( mo_success );
            else {
                fence_after(mo_fail);
                expected = desired;
            }

            return bSuccess;
        }

        template <typename T>
        static inline bool cas32_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas32_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        template <typename T>
        static inline T exchange32( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( pDest );

            // This primitive could be implemented via "swap" instruction but "swap" is deprecated in UltraSparc

            T cur = load32( pDest, memory_order_relaxed );
            do {} while ( !cas32_strong( pDest, cur, v, order, memory_order_relaxed ));
            return cur;
        }

        //-----------------------------------------------------------------------------
        // 64bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T load64( T volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );

            fence_before(order);
            T v = *pSrc;
            fence_after(order);
            return v;
        }

        template <typename T>
        static inline void store64( T volatile * pDest, T val, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );

            fence_before(order);
            *pDest = val;
            fence_after(order);

        }

        template <typename T>
        static inline bool cas64_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( pDest );

            fence_before( mo_success );
            __asm__ __volatile__(
                "casx [%[pDest]], %[expected], %[desired]"
                : [desired] "+r" (desired)
                : [pDest] "r" (pDest), [expected] "r" (expected)
                : "memory"
                );

            // desired contains current value

            bool bSuccess = desired == expected;
            if ( bSuccess ) {
                fence_after( mo_success );
            }
            else {
                fence_after(mo_fail);
                expected = desired;
            }

            return bSuccess;
        }

        template <typename T>
        static inline bool cas64_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas64_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        template <typename T>
        static inline T exchange64( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( pDest );

            T cur = load64( pDest, memory_order_relaxed );
            do {} while ( !cas64_strong( pDest, cur, v, order, memory_order_relaxed ));
            return cur;
        }

        //-----------------------------------------------------------------------------
        // 8bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline void store8( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );

            fence_before( order );
            *pDest = src;
            fence_after( order );
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
            assert( pSrc );

            fence_before( order );
            T v = *pSrc;
            fence_after( order );
            return v;
        }

        template <typename T>
        static inline bool cas8_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
            assert( pDest );

            union u32 {
                uint32_t    w;
                T           c[4];
            };
            static_assert( sizeof(u32) == sizeof(uint32_t), "Argument size error" );

            u32 volatile *  pDest32 = (u32 *)( uintptr_t( pDest ) & ~0x03 );
            size_t const  nCharIdx = (size_t)( uintptr_t( pDest ) & 0x03 );
            u32 uExpected;
            u32 uDesired;

            bool bSuccess;
            for (;;) {
                uExpected.w =
                    uDesired.w = pDest32->w;
                uExpected.c[nCharIdx] = expected;
                uDesired.c[nCharIdx] = desired;

                bSuccess = cas32_weak( reinterpret_cast<uint32_t volatile *>(pDest32), uExpected.w, uDesired.w, mo_success, mo_fail );
                if ( bSuccess || uExpected.c[nCharIdx] != expected )
                    break;
            }

            expected = uExpected.c[nCharIdx];
            return bSuccess;
        }

        template <typename T>
        static inline bool cas8_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
            assert( pDest );

            union u32 {
                uint32_t    w;
                T           c[4];
            };
            static_assert( sizeof(u32) == sizeof(uint32_t), "Argument size error" );

            u32 volatile * pDest32 = (u32 *)( uintptr_t( pDest ) & ~0x03 );
            size_t const  nCharIdx = (size_t)( uintptr_t( pDest ) & 0x03 );
            u32 uExpected;
            u32 uDesired;

            uExpected.w =
                uDesired.w = pDest32->w;
            uExpected.c[nCharIdx] = expected;
            uDesired.c[nCharIdx] = desired;

            bool bSuccess = cas32_weak( reinterpret_cast<uint32_t volatile *>(pDest32), uExpected.w, uDesired.w, mo_success, mo_fail );

            expected = uExpected.c[nCharIdx];
            return bSuccess;
        }

        template <typename T>
        static inline T exchange8( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
            assert( pDest );

            T cur = load8( pDest, memory_order_relaxed );
            do {} while ( !cas8_strong( pDest, cur, v, order, memory_order_relaxed ));
            return cur;
        }

        //-----------------------------------------------------------------------------
        // 16bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T load16( T volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );

            fence_before( order );
            T v = *pSrc;
            fence_after( order );
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
            assert( pDest );

            fence_before(order);
            *pDest = src;
            fence_after(order);
        }

        template <typename T>
        static inline bool cas16_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( pDest );

            union u32 {
                uint32_t    w;
                T           c[2];
            };
            static_assert( sizeof(u32) == sizeof(uint32_t), "Argument size error" );

            u32 volatile *  pDest32 = (u32 *)( uintptr_t( pDest ) & ~0x03 );
            size_t const  nIdx = (size_t)( (uintptr_t( pDest ) >> 1) & 0x01 );
            u32 uExpected;
            u32 uDesired;

            bool bSuccess;
            for (;;) {
                uExpected.w =
                    uDesired.w = pDest32->w;
                uExpected.c[nIdx] = expected;
                uDesired.c[nIdx] = desired;

                bSuccess = cas32_weak( reinterpret_cast<uint32_t volatile *>(pDest32), uExpected.w, uDesired.w, mo_success, mo_fail );
                if ( bSuccess || uExpected.c[nIdx] != expected )
                    break;
            }

            expected = uExpected.c[nIdx];
            return bSuccess;
        }

        template <typename T>
        static inline bool cas16_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( pDest );

            union u32 {
                uint32_t    w;
                T           c[2];
            };
            static_assert( sizeof(u32) == sizeof(uint32_t), "Argument size error" );

            u32 volatile * pDest32 = (u32 *)( uintptr_t( pDest ) & ~0x03 );
            size_t const  nIdx = (size_t)( (uintptr_t( pDest ) >> 1) & 0x01 );
            u32 uExpected;
            u32 uDesired;

            uExpected.w =
                uDesired.w = pDest32->w;
            uExpected.c[nIdx] = expected;
            uDesired.c[nIdx] = desired;

            bool bSuccess = cas32_weak( reinterpret_cast<uint32_t volatile *>(pDest32), uExpected.w, uDesired.w, mo_success, mo_fail );

            expected = uExpected.c[nIdx];
            return bSuccess;
        }

        template <typename T>
        static inline T exchange16( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( pDest );

            T cur = load16( pDest, memory_order_relaxed );
            do {} while ( !cas16_strong( pDest, cur, v, order, memory_order_relaxed ));
            return cur;
        }

        //-----------------------------------------------------------------------------
        // pointer primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline void store_ptr( T * volatile * pDest, T * src, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );

            fence_before(order);
            *pDest = src;
            fence_after(order);
        }

        template <typename T>
        static inline T * load_ptr( T * volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );

            fence_before( order );
            T * v = *pSrc;
            fence_after( order );
            return v;
        }

        template <typename T>
        static inline bool cas_ptr_strong( T * volatile * pDest, T *& expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );

            return cas64_strong( (uint64_t volatile *) pDest, *reinterpret_cast<uint64_t *>( &expected ), (uint64_t) desired, mo_success, mo_fail );
        }

        template <typename T>
        static inline bool cas_ptr_weak( T * volatile * pDest, T *& expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas_ptr_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        template <typename T>
        static inline T * exchange_ptr( T * volatile * pDest, T * v, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );
            return (T *) exchange64( (uint64_t volatile *) pDest, (uint64_t) v, order );
        }

    }} // namespace gcc::Sparc

    }   // namespace platform
}}  // namespace cds::cxx11_atomic
//@endcond

#undef CDS_SPARC_MB_ACQ
#undef CDS_SPARC_MB_REL
#undef CDS_SPARC_MB_SEQ_CST
#undef CDS_SPARC_MB_FULL
#undef CDS_SPARC_MB_LL_LS
#undef CDS_SPARC_MB_LS_SS
#undef CDS_SPARC_MB_LL_LS_SS

#endif // #ifndef CDSLIB_COMPILER_GCC_AMD64_CXX11_ATOMIC_H
