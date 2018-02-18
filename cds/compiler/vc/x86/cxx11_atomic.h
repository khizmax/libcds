// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_VC_X86_CXX11_ATOMIC_H
#define CDSLIB_COMPILER_VC_X86_CXX11_ATOMIC_H

#include <intrin.h>
#include <emmintrin.h>  // for 64bit atomic load/store
#include <cds/details/is_aligned.h>

#pragma intrinsic( _InterlockedIncrement )
#pragma intrinsic( _InterlockedDecrement )
#pragma intrinsic( _InterlockedCompareExchange )
//#pragma intrinsic( _InterlockedCompareExchangePointer )   // On the x86 architecture, _InterlockedCompareExchangePointer is a macro that calls _InterlockedCompareExchange
#pragma intrinsic( _InterlockedCompareExchange16 )
#pragma intrinsic( _InterlockedCompareExchange64 )
#pragma intrinsic( _InterlockedExchange )
//#pragma intrinsic( _InterlockedExchangePointer )  // On the x86 architecture, _InterlockedExchangePointer is a macro that calls _InterlockedExchange
#pragma intrinsic( _InterlockedExchangeAdd )
#pragma intrinsic( _InterlockedXor )
#pragma intrinsic( _InterlockedOr )
#pragma intrinsic( _InterlockedAnd )
#pragma intrinsic( _interlockedbittestandset )
#if _MSC_VER >= 1600
#   pragma intrinsic( _InterlockedCompareExchange8 )
#   pragma intrinsic( _InterlockedExchange8 )
#   pragma intrinsic( _InterlockedExchange16 )
#endif

//@cond
namespace cds { namespace cxx11_atomic {
    namespace platform { inline namespace vc { inline namespace x86 {

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
                __asm { mfence };
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
                    __asm { mfence };
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
        static inline bool atomic_flag_tas( atomic_flag_type volatile * pFlag, memory_order /*order*/ ) noexcept
        {
            return _interlockedbittestandset( (long volatile *) pFlag, 0 ) != 0;
        }

        static inline void atomic_flag_clear( atomic_flag_type volatile * pFlag, memory_order order ) noexcept
        {
            assert( order != memory_order_acquire
                && order != memory_order_acq_rel
                );

            fence_before( order );
            *pFlag = 0;
            fence_after( order );
        }


        //-----------------------------------------------------------------------------
        // 8bit primitives
        //-----------------------------------------------------------------------------

#if _MSC_VER >= 1600
#   pragma warning(push)
        // Disable warning C4800: 'char' : forcing value to bool 'true' or 'false' (performance warning)
#   pragma warning( disable: 4800 )
#endif
        template <typename T>
        static inline bool cas8_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal operand size"  );

#       if _MSC_VER >= 1600
            T prev = expected;
            expected = (T) _InterlockedCompareExchange8( reinterpret_cast<char volatile*>(pDest), (char) desired, (char) expected );
            return expected == prev;
#       else
            bool bRet = false;
            __asm {
                mov ecx, pDest;
                mov edx, expected;
                mov al, byte ptr [edx];
                mov ah, desired;
                lock cmpxchg byte ptr [ecx], ah;
                mov byte ptr [edx], al;
                setz bRet;
            }
            return bRet;
#       endif
        }
#if _MSC_VER >= 1600
#   pragma warning(pop)
#endif

        template <typename T>
        static inline bool cas8_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas8_strong( pDest, expected, desired, mo_success, mo_fail );
        }

#if _MSC_VER >= 1600
#   pragma warning(push)
    // Disable warning C4800: 'char' : forcing value to bool 'true' or 'false' (performance warning)
#   pragma warning( disable: 4800 )
#endif
        template <typename T>
        static inline T exchange8( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal operand size" );

#       if _MSC_VER >= 1600
            return (T) _InterlockedExchange8( reinterpret_cast<char volatile *>(pDest), (char) v );
#       else
            __asm {
                mov al, v;
                mov ecx, pDest;
                lock xchg byte ptr [ecx], al;
            }
#       endif
        }
#if _MSC_VER >= 1600
#   pragma warning(pop)
#endif

        template <typename T>
        static inline void store8( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );

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
            static_assert( sizeof(T) == 1, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order == memory_order_seq_cst
                );
            assert( pSrc );

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }

        //-----------------------------------------------------------------------------
        // 16bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T exchange16( T volatile * pDest, T v, memory_order /*order*/ ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal operand size" );
            assert( cds::details::is_aligned( pDest, 2 ));

#       if _MSC_VER >= 1600
            return (T) _InterlockedExchange16( (short volatile *) pDest, (short) v );
#       else
            __asm {
                mov ax, v;
                mov ecx, pDest;
                lock xchg word ptr [ecx], ax;
            }
#       endif
        }

        template <typename T>
        static inline void store16( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );
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
            static_assert( sizeof(T) == 2, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );
            assert( cds::details::is_aligned( pSrc, 2 ));

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }

        template <typename T>
        static inline bool cas16_strong( T volatile * pDest, T& expected, T desired, memory_order /*mo_success*/, memory_order /*mo_fail*/ ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal operand size" );
            assert( cds::details::is_aligned( pDest, 2 ));

            // _InterlockedCompareExchange behave as read-write memory barriers
            T prev = expected;
            expected = (T) _InterlockedCompareExchange16( (short *) pDest, (short) desired, (short) expected );
            return expected == prev;
        }

        template <typename T>
        static inline bool cas16_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas16_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        //-----------------------------------------------------------------------------
        // 32bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T exchange32( T volatile * pDest, T v, memory_order /*order*/ ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal operand size" );
            assert( cds::details::is_aligned( pDest, 4 ));

            return (T) _InterlockedExchange( (long *) pDest, (long) v );
        }

        template <typename T>
        static inline void store32( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );
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
            static_assert( sizeof(T) == 4, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );
            assert( cds::details::is_aligned( pSrc, 4 ));

            T v( *pSrc );
            fence_after_load( order );
            return v;
        }

        template <typename T>
        static inline bool cas32_strong( T volatile * pDest, T& expected, T desired, memory_order /*mo_success*/, memory_order /*mo_fail*/ ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal operand size" );
            assert( cds::details::is_aligned( pDest, 4 ));

            // _InterlockedCompareExchange behave as read-write memory barriers
            T prev = expected;
            expected = (T) _InterlockedCompareExchange( (long *) pDest, (long) desired, (long) expected );
            return expected == prev;
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
        static inline T fetch32_add( T volatile * pDest, T v, memory_order /*order*/) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal operand size" );
            assert( cds::details::is_aligned( pDest, 4 ));

            // _InterlockedExchangeAdd behave as read-write memory barriers
            return (T) _InterlockedExchangeAdd( (long *) pDest, (long) v );
        }

        //-----------------------------------------------------------------------------
        // 64bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline bool cas64_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal operand size" );
            assert( cds::details::is_aligned( pDest, 8 ));

            // _InterlockedCompareExchange behave as read-write memory barriers
            T prev = expected;
            expected = (T) _InterlockedCompareExchange64( (__int64 *) pDest, (__int64) desired, (__int64) expected );
            return expected == prev;
        }

        template <typename T>
        static inline bool cas64_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas64_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        template <typename T>
        static inline T load64( T volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );
            assert( cds::details::is_aligned( pSrc, 8 ));

            // Atomically loads 64bit value by SSE intrinsics
            __m128i volatile v = _mm_loadl_epi64( (__m128i const *) pSrc );
            fence_after_load( order );
            return (T) v.m128i_i64[0];
        }


        template <typename T>
        static inline T exchange64( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal operand size" );

            T cur = load64( pDest, memory_order_relaxed );
            do {
            } while (!cas64_weak( pDest, cur, v, order, memory_order_relaxed ));
            return cur;
        }

        template <typename T>
        static inline void store64( T volatile * pDest, T val, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );
            assert( cds::details::is_aligned( pDest, 8 ));

            if ( order != memory_order_seq_cst ) {
                __m128i v;
                v.m128i_i64[0] = val;
                fence_before( order );
                _mm_storel_epi64( (__m128i *) pDest, v );
            }
            else {
                exchange64( pDest, val, order );
            }
        }


        //-----------------------------------------------------------------------------
        // pointer primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T * exchange_ptr( T * volatile * pDest, T * v, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal operand size" );
            return (T *) _InterlockedExchange( (long volatile *) pDest, (uintptr_t) v );
            //return (T *) _InterlockedExchangePointer( (void * volatile *) pDest, reinterpret_cast<void *>(v));
        }

        template <typename T>
        static inline void store_ptr( T * volatile * pDest, T * src, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_release
                || order == memory_order_seq_cst
                );
            assert( pDest );

            if ( order != memory_order_seq_cst ) {
                fence_before( order );
                *pDest = src;
            }
            else {
                exchange_ptr( pDest, src, order );
            }
        }

        template <typename T>
        static inline T * load_ptr( T * volatile const * pSrc, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal operand size" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );

            T * v = *pSrc;
            fence_after_load( order );
            return v;
        }

        template <typename T>
        static inline bool cas_ptr_strong( T * volatile * pDest, T *& expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal operand size" );

            // _InterlockedCompareExchangePointer behave as read-write memory barriers
            T * prev = expected;
            expected = (T *) _InterlockedCompareExchange( (long volatile *) pDest, (uintptr_t) desired, (uintptr_t) prev );
            return expected == prev;
        }

        template <typename T>
        static inline bool cas_ptr_weak( T * volatile * pDest, T *& expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas_ptr_strong( pDest, expected, desired, mo_success, mo_fail );
        }
    }} // namespace vc::x86

    } // namespace platform
}}  // namespace cds::cxx11_atomic
//@endcond

#endif // #ifndef CDSLIB_COMPILER_VC_X86_CXX11_ATOMIC_H
