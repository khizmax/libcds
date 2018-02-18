// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_VC_AMD64_CXX11_ATOMIC_H
#define CDSLIB_COMPILER_VC_AMD64_CXX11_ATOMIC_H

#include <intrin.h>
#include <emmintrin.h>  // for 128bit atomic load/store
#include <cds/details/is_aligned.h>

#pragma intrinsic( _InterlockedIncrement )
#pragma intrinsic( _InterlockedDecrement )
#pragma intrinsic( _InterlockedCompareExchange )
#pragma intrinsic( _InterlockedCompareExchangePointer )
#pragma intrinsic( _InterlockedCompareExchange16 )
#pragma intrinsic( _InterlockedCompareExchange64 )
#pragma intrinsic( _InterlockedExchange )
#pragma intrinsic( _InterlockedExchange64 )
#pragma intrinsic( _InterlockedExchangePointer )
#pragma intrinsic( _InterlockedExchangeAdd )
#pragma intrinsic( _InterlockedExchangeAdd64 )
//#pragma intrinsic( _InterlockedAnd )
//#pragma intrinsic( _InterlockedOr )
//#pragma intrinsic( _InterlockedXor )
//#pragma intrinsic( _InterlockedAnd64 )
//#pragma intrinsic( _InterlockedOr64 )
//#pragma intrinsic( _InterlockedXor64 )
#pragma intrinsic( _interlockedbittestandset )
#if _MSC_VER >= 1600
#   pragma intrinsic( _InterlockedCompareExchange8 )
#   pragma intrinsic( _InterlockedExchange8 )
#   pragma intrinsic( _InterlockedExchange16 )
#endif

//@cond
namespace cds { namespace cxx11_atomic {
    namespace platform { inline namespace vc { inline namespace amd64 {

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

        static inline void full_fence()
        {
            // MS VC does not support inline assembler in C code.
            // So, we use InterlockedExchange for full fence instead of mfence inst
            long t;
            _InterlockedExchange( &t, 0 );
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
                full_fence();
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
                    full_fence();
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
        static inline bool cas8_strong( T volatile * pDest, T& expected, T desired, memory_order /*mo_success*/, memory_order /*mo_fail*/ ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );

#       if _MSC_VER >= 1600
            // VC 2010 +
            T prev = expected;
            expected = (T) _InterlockedCompareExchange8( (char volatile*) pDest, (char) desired, (char) expected );
            return expected == prev;
#       else
            // VC 2008
            unsigned int *  pnDest = (unsigned int *)( ((unsigned __int64) pDest) & ~(unsigned __int64(3)));
            unsigned int    nOffset = ((unsigned __int64) pDest) & 3;
            unsigned int    nExpected;
            unsigned int    nDesired;

            for (;;) {
                nExpected =
                    nDesired = *pnDest;
                memcpy( reinterpret_cast<T *>(&nExpected) + nOffset, &expected, sizeof(T));
                memcpy( reinterpret_cast<T *>(&nDesired) + nOffset, &desired, sizeof(T));

                unsigned int nPrev = (unsigned int) _InterlockedCompareExchange( (long *) pnDest, (long) nDesired, (long) nExpected );
                if ( nPrev == nExpected )
                    return true;
                T nByte;
                memcpy( &nByte, reinterpret_cast<T *>(&nPrev) + nOffset, sizeof(T));
                if ( nByte != expected ) {
                    expected = nByte;
                    return false;
                }
            }
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
            static_assert( sizeof(T) == 1, "Illegal size of operand" );

#       if _MSC_VER >= 1600
            CDS_UNUSED(order);
            return (T) _InterlockedExchange8( (char volatile *) pDest, (char) v );
#       else
            T expected = *pDest;
            do {} while ( !cas8_strong( pDest, expected, v, order, memory_order_relaxed ));
            return expected;
#       endif
        }
#if _MSC_VER >= 1600
#   pragma warning(pop)
#endif

        template <typename T>
        static inline void store8( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
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
            static_assert( sizeof(T) == 1, "Illegal size of operand" );
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
        static inline bool cas16_strong( T volatile * pDest, T& expected, T desired, memory_order /*mo_success*/, memory_order /*mo_fail*/ ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
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

        template <typename T>
        static inline T exchange16( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 2, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 2 ));

#       if _MSC_VER >= 1600
            order;
            return (T) _InterlockedExchange16( (short volatile *) pDest, (short) v );
#       else
            T expected = *pDest;
            do {} while ( !cas16_strong( pDest, expected, v, order, memory_order_relaxed ));
            return expected;
#       endif
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
            assert( pSrc );
            assert( cds::details::is_aligned( pSrc, 2 ));

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }

        //-----------------------------------------------------------------------------
        // 32bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T exchange32( T volatile * pDest, T v, memory_order /*order*/ ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 4 ));

            return (T) _InterlockedExchange( (long *) pDest, (long) v );
        }

        template <typename T>
        static inline void store32( T volatile * pDest, T src, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
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
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );
            assert( cds::details::is_aligned( pSrc, 4 ));

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }

        template <typename T>
        static inline bool cas32_strong( T volatile * pDest, T& expected, T desired, memory_order /*mo_success*/, memory_order /*mo_fail*/ ) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
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
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 4 ));

            // _InterlockedExchangeAdd behave as read-write memory barriers
            return (T) _InterlockedExchangeAdd( (long *) pDest, (long) v );
        }

        //-----------------------------------------------------------------------------
        // 64bit primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline bool cas64_strong( T volatile * pDest, T& expected, T desired, memory_order /*mo_success*/, memory_order /*mo_fail*/ ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
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
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( order ==  memory_order_relaxed
                || order ==  memory_order_consume
                || order ==  memory_order_acquire
                || order ==  memory_order_seq_cst
                );
            assert( pSrc );
            assert( cds::details::is_aligned( pSrc, 8 ));

            T v = *pSrc;
            fence_after_load( order );
            return v;
        }


        template <typename T>
        static inline T exchange64( T volatile * pDest, T v, memory_order order ) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );

            T cur = load64( pDest, memory_order_relaxed );
            do {
            } while (!cas64_weak( pDest, cur, v, order, memory_order_relaxed ));
            return cur;
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
            assert( cds::details::is_aligned( pDest, 8 ));

            if ( order != memory_order_seq_cst ) {
                fence_before( order );
                *pDest = val;
            }
            else {
                exchange64( pDest, val, order );
            }
        }

#       define CDS_ATOMIC_fetch64_add_defined
        template <typename T>
        static inline T fetch64_add( T volatile * pDest, T v, memory_order /*order*/) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( cds::details::is_aligned( pDest, 8 ));

            // _InterlockedExchangeAdd64 behave as read-write memory barriers
            return (T) _InterlockedExchangeAdd64( (__int64 *) pDest, (__int64) v );
        }

        //-----------------------------------------------------------------------------
        // pointer primitives
        //-----------------------------------------------------------------------------

        template <typename T>
        static inline T * exchange_ptr( T * volatile * pDest, T * v, memory_order /*order*/ ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );
            return (T *) _InterlockedExchangePointer( (void * volatile *) pDest, reinterpret_cast<void *>(v));
        }

        template <typename T>
        static inline void store_ptr( T * volatile * pDest, T * src, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );
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
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );
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
        static inline bool cas_ptr_strong( T * volatile * pDest, T *& expected, T * desired, memory_order /*mo_success*/, memory_order /*mo_fail*/ ) noexcept
        {
            static_assert( sizeof(T *) == sizeof(void *), "Illegal size of operand" );

            // _InterlockedCompareExchangePointer behave as read-write memory barriers
            T * prev = expected;
            expected = (T *) _InterlockedCompareExchangePointer( (void * volatile *) pDest, (void *) desired, (void *) expected );
            return expected == prev;
        }

        template <typename T>
        static inline bool cas_ptr_weak( T * volatile * pDest, T *& expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas_ptr_strong( pDest, expected, desired, mo_success, mo_fail );
        }

    }} // namespace vc::amd64

    } // namespace platform
}}  // namespace cds::cxx11_atomic
//@endcond

#endif // #ifndef CDSLIB_COMPILER_VC_AMD64_CXX11_ATOMIC_H
