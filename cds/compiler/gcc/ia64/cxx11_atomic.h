// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_IA64_CXX11_ATOMIC_H
#define CDSLIB_COMPILER_GCC_IA64_CXX11_ATOMIC_H

/*
    Source:
        1. load/store: http://www.decadent.org.uk/pipermail/cpp-threads/2008-December/001932.html
        2. Mapping to C++ Memory Model: http://www.cl.cam.ac.uk/~pes20/cpp/cpp0xmappings.html
*/

#include <cstdint>

//@cond
namespace cds { namespace cxx11_atomic {
    namespace platform { inline namespace gcc { inline namespace ia64 {

        static inline void itanium_full_fence() noexcept
        {
            __asm__ __volatile__ ( "mf \n\t" ::: "memory" );
        }

        static inline void fence_before( memory_order order ) noexcept
        {
            switch(order) {
            case memory_order_relaxed:
            case memory_order_consume:
            case memory_order_acquire:
                break;
            case memory_order_release:
            case memory_order_acq_rel:
                CDS_COMPILER_RW_BARRIER;
                break;
            case memory_order_seq_cst:
                itanium_full_fence();
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
                itanium_full_fence();
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
                case memory_order_release:
                case memory_order_acquire:
                case memory_order_acq_rel:
                    CDS_COMPILER_RW_BARRIER;
                    break;
                case memory_order_seq_cst:
                    itanium_full_fence();
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

#define CDS_ITANIUM_ATOMIC_LOAD( n_bytes, n_bits )   \
        template <typename T>   \
        static inline T load##n_bits( T volatile const * pSrc, memory_order order ) noexcept \
        { \
            static_assert( sizeof(T) == n_bytes, "Illegal size of operand" )   ; \
            assert( order ==  memory_order_relaxed \
                || order ==  memory_order_consume  \
                || order ==  memory_order_acquire  \
                || order == memory_order_seq_cst   \
                ) ; \
            assert( pSrc )  ; \
            T val    ; \
            __asm__ __volatile__ ( \
                "ld" #n_bytes ".acq %[val] = [%[pSrc]]  \n\t" \
                : [val] "=r" (val) \
                : [pSrc] "r" (pSrc) \
                : "memory" \
                ) ; \
            return val ; \
        }

#define CDS_ITANIUM_ATOMIC_STORE( n_bytes, n_bits ) \
        template <typename T> \
        static inline void store##n_bits( T volatile * pDest, T val, memory_order order ) noexcept \
        { \
            static_assert( sizeof(T) == n_bytes, "Illegal size of operand" )   ; \
            assert( order ==  memory_order_relaxed \
                || order ==  memory_order_release  \
                || order == memory_order_seq_cst   \
                ) ; \
            assert( pDest )  ; \
            if ( order == memory_order_seq_cst ) { \
                __asm__ __volatile__ ( \
                    "st" #n_bytes ".rel [%[pDest]] = %[val] \n\t" \
                    "mf     \n\t" \
                    :: [pDest] "r" (pDest), [val] "r" (val) \
                    : "memory" \
                    ) ; \
            } \
            else { \
                __asm__ __volatile__ ( \
                    "st" #n_bytes ".rel [%[pDest]] = %[val] \n\t" \
                    :: [pDest] "r" (pDest), [val] "r" (val) \
                    : "memory" \
                    ) ; \
                fence_after(order) ; \
            } \
        }

#define CDS_ITANIUM_ATOMIC_CAS( n_bytes, n_bits ) \
        template <typename T> \
        static inline bool cas##n_bits##_strong( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order /*mo_fail*/ ) noexcept \
        { \
            static_assert( sizeof(T) == n_bytes, "Illegal size of operand" )   ; \
            T current ; \
            switch(mo_success) { \
            case memory_order_relaxed: \
            case memory_order_consume: \
            case memory_order_acquire: \
                __asm__ __volatile__ ( \
                    "mov ar.ccv = %[expected] ;;\n\t" \
                    "cmpxchg" #n_bytes ".acq %[current] = [%[pDest]], %[desired], ar.ccv\n\t" \
                    : [current] "=r" (current) \
                    : [pDest] "r" (pDest), [expected] "r" (expected), [desired] "r" (desired) \
                    : "ar.ccv", "memory" \
                    ); \
                break ; \
            case memory_order_release: \
                __asm__ __volatile__ ( \
                    "mov ar.ccv = %[expected] ;;\n\t" \
                    "cmpxchg" #n_bytes ".rel %[current] = [%[pDest]], %[desired], ar.ccv\n\t" \
                    : [current] "=r" (current) \
                    : [pDest] "r" (pDest), [expected] "r" (expected), [desired] "r" (desired) \
                    : "ar.ccv", "memory" \
                    ); \
                break ; \
            case memory_order_acq_rel: \
            case memory_order_seq_cst: \
                __asm__ __volatile__ ( \
                    "mov ar.ccv = %[expected] ;;\n\t" \
                    "cmpxchg" #n_bytes ".rel %[current] = [%[pDest]], %[desired], ar.ccv\n\t" \
                    "mf \n\t" \
                    : [current] "=r" (current) \
                    : [pDest] "r" (pDest), [expected] "r" (expected), [desired] "r" (desired) \
                    : "ar.ccv", "memory" \
                    ); \
                break; \
            default: \
                assert(false); \
            } \
            bool bSuccess = expected == current ; \
            expected = current ; \
            return bSuccess ; \
        } \
        template <typename T> \
        static inline bool cas##n_bits##_weak( T volatile * pDest, T& expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept \
        { return cas##n_bits##_strong( pDest, expected, desired, mo_success, mo_fail ); }

        // xchg is performed with acquire semantics
#define CDS_ITANIUM_ATOMIC_EXCHANGE( n_bytes, n_bits ) \
        template <typename T> \
        static inline T exchange##n_bits( T volatile * pDest, T val, memory_order order ) noexcept \
        { \
            static_assert( sizeof(T) == n_bytes, "Illegal size of operand" )   ; \
            assert( pDest ) ; \
            T current ; \
            switch(order) \
            { \
            case memory_order_relaxed: \
            case memory_order_consume: \
            case memory_order_acquire: \
                __asm__ __volatile__ ( \
                    "xchg" #n_bytes " %[current] = [%[pDest]], %[val]\n\t" \
                    : [current] "=r" (current) \
                    : [pDest] "r" (pDest), [val] "r" (val) \
                    : "memory" \
                    ); \
                break; \
            case memory_order_acq_rel: \
            case memory_order_release: \
            case memory_order_seq_cst: \
                __asm__ __volatile__ ( \
                    "mf \n\t" \
                    "xchg" #n_bytes " %[current] = [%[pDest]], %[val]\n\t" \
                    : [current] "=r" (current) \
                    : [pDest] "r" (pDest), [val] "r" (val) \
                    : "memory" \
                    ); \
                break; \
            default: assert(false); \
            } \
            return current ; \
        }

#define CDS_ITANIUM_ATOMIC_FETCH_ADD( n_bytes, n_add )  \
        switch (order) { \
            case memory_order_relaxed: \
            case memory_order_consume: \
            case memory_order_acquire: \
                __asm__ __volatile__ ( \
                    "fetchadd" #n_bytes ".acq %[cur] = [%[pDest]], " #n_add " \n\t" \
                    : [cur] "=r" (cur) \
                    : [pDest] "r" (pDest) \
                    : "memory" \
                    ); \
                break ; \
            case memory_order_release: \
                __asm__ __volatile__ ( \
                    "fetchadd" #n_bytes ".rel %[cur] = [%[pDest]], " #n_add " \n\t" \
                    : [cur] "=r" (cur) \
                    : [pDest] "r" (pDest) \
                    : "memory" \
                    ); \
                break ; \
            case memory_order_acq_rel: \
            case memory_order_seq_cst: \
                __asm__ __volatile__ ( \
                    "fetchadd" #n_bytes ".rel %[cur] = [%[pDest]], " #n_add " \n\t" \
                    "mf \n\t" \
                    : [cur] "=r" (cur) \
                    : [pDest] "r" (pDest) \
                    : "memory" \
                    ); \
                break ; \
            default: \
                assert(false); \
        }

        //-----------------------------------------------------------------------------
        // 8bit primitives
        //-----------------------------------------------------------------------------

        CDS_ITANIUM_ATOMIC_LOAD( 1, 8 )
        CDS_ITANIUM_ATOMIC_STORE( 1, 8 )
        CDS_ITANIUM_ATOMIC_CAS( 1, 8 )
        CDS_ITANIUM_ATOMIC_EXCHANGE( 1, 8 )

        //-----------------------------------------------------------------------------
        // 16bit primitives
        //-----------------------------------------------------------------------------

        CDS_ITANIUM_ATOMIC_LOAD( 2, 16 )
        CDS_ITANIUM_ATOMIC_STORE( 2, 16 )
        CDS_ITANIUM_ATOMIC_CAS( 2, 16 )
        CDS_ITANIUM_ATOMIC_EXCHANGE( 2, 16 )

        //-----------------------------------------------------------------------------
        // 32bit primitives
        //-----------------------------------------------------------------------------

        CDS_ITANIUM_ATOMIC_LOAD( 4, 32 )
        CDS_ITANIUM_ATOMIC_STORE( 4, 32 )
        CDS_ITANIUM_ATOMIC_CAS( 4, 32 )
        CDS_ITANIUM_ATOMIC_EXCHANGE( 4, 32 )

#       define CDS_ATOMIC_fetch32_add_defined
        template <typename T>
        static inline T fetch32_add( T volatile * pDest, T val, memory_order order) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( pDest );

            T cur;
            switch ( val ) {
                case 1:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, 1 );
                    break;
                case 4:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, 4 );
                    break;
                case 8:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, 8 );
                    break;
                case 16:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, 16 );
                    break;
                default:
                    cur = load32( pDest, memory_order_relaxed );
                    do {} while ( !cas32_strong( pDest, cur, cur + val, order, memory_order_relaxed ));
                    break;
            }
            return cur;
        }

#       define CDS_ATOMIC_fetch32_sub_defined
        template <typename T>
        static inline T fetch32_sub( T volatile * pDest, T val, memory_order order) noexcept
        {
            static_assert( sizeof(T) == 4, "Illegal size of operand" );
            assert( pDest );
            T cur;
            switch ( val ) {
                case 1:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, -1 );
                    break;
                case 4:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, -4 );
                    break;
                case 8:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, -8 );
                    break;
                case 16:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 4, -16 );
                    break;
                default:
                    cur = load32( pDest, memory_order_relaxed );
                    do {} while ( !cas32_strong( pDest, cur, cur - val, order, memory_order_relaxed ));
                    break;
            }
            return cur;
        }

        //-----------------------------------------------------------------------------
        // 64bit primitives
        //-----------------------------------------------------------------------------

        CDS_ITANIUM_ATOMIC_LOAD( 8, 64 )
        CDS_ITANIUM_ATOMIC_STORE( 8, 64 )
        CDS_ITANIUM_ATOMIC_CAS( 8, 64 )
        CDS_ITANIUM_ATOMIC_EXCHANGE( 8, 64 )

#       define CDS_ATOMIC_fetch64_add_defined
        template <typename T>
        static inline T fetch64_add( T volatile * pDest, T val, memory_order order) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( pDest );

            T cur;
            switch ( val ) {
                case 1:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 1 );
                    break;
                case 4:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 4 );
                    break;
                case 8:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 8 );
                    break;
                case 16:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 16 );
                    break;
                default:
                    cur = load64( pDest, memory_order_relaxed );
                    do {} while ( !cas64_strong( pDest, cur, cur + val, order, memory_order_relaxed ));
                    break;
            }
            return cur;
        }

#       define CDS_ATOMIC_fetch64_sub_defined
        template <typename T>
        static inline T fetch64_sub( T volatile * pDest, T val, memory_order order) noexcept
        {
            static_assert( sizeof(T) == 8, "Illegal size of operand" );
            assert( pDest );
            T cur;
            switch ( val ) {
                case 1:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -1 );
                    break;
                case 4:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -4 );
                    break;
                case 8:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -8 );
                    break;
                case 16:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -16 );
                    break;
                default:
                    cur = load64( pDest, memory_order_relaxed );
                    do {} while ( !cas64_strong( pDest, cur, cur - val, order, memory_order_relaxed ));
                    break;
            }
            return cur;
        }

        //-----------------------------------------------------------------------------
        // pointer primitives
        //-----------------------------------------------------------------------------
        template <typename T>
        static inline T * load_ptr( T * volatile const * pSrc, memory_order order ) noexcept
        {
            assert( order ==  memory_order_relaxed
                 || order ==  memory_order_consume
                 || order ==  memory_order_acquire
                 || order == memory_order_seq_cst
            );
            assert( pSrc );
            T * val;
            __asm__ __volatile__ (
                "ld8.acq %[val] = [%[pSrc]]  \n\t"
                : [val] "=r" (val)
                : [pSrc] "r" (pSrc)
                : "memory"
            );
            return val;
        }

        template <typename T>
        static inline void store_ptr( T * volatile * pDest, T * val, memory_order order ) noexcept
        {
            assert( order ==  memory_order_relaxed
                 || order ==  memory_order_release
                 || order == memory_order_seq_cst
            );
            assert( pDest );

            if ( order == memory_order_seq_cst ) {
                __asm__ __volatile__ (
                    "st8.rel [%[pDest]] = %[val] \n\t"
                    "mf     \n\t"
                    :: [pDest] "r" (pDest), [val] "r" (val)
                    : "memory"
                );
            }
            else {
                __asm__ __volatile__ (
                    "st8.rel [%[pDest]] = %[val] \n\t"
                    :: [pDest] "r" (pDest), [val] "r" (val)
                    : "memory"
                );
                fence_after(order);
            }
        }

        template <typename T>
        static inline bool cas_ptr_strong( T * volatile * pDest, T *& expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            static_assert( sizeof(T *) == 8, "Illegal size of operand" );
            assert( pDest );

            T * current;

            switch(mo_success) {
            case memory_order_relaxed:
            case memory_order_consume:
            case memory_order_acquire:
                __asm__ __volatile__ (
                    "mov ar.ccv = %[expected] ;;\n\t"
                    "cmpxchg8.acq %[current] = [%[pDest]], %[desired], ar.ccv\n\t"
                    : [current] "=r" (current)
                    : [pDest] "r" (pDest), [expected] "r" (expected), [desired] "r" (desired)
                    : "ar.ccv", "memory"
                );
                break;
            case memory_order_release:
               __asm__ __volatile__ (
                   "mov ar.ccv = %[expected] ;;\n\t"
                   "cmpxchg8.rel %[current] = [%[pDest]], %[desired], ar.ccv\n\t"
                   : [current] "=r" (current)
                   : [pDest] "r" (pDest), [expected] "r" (expected), [desired] "r" (desired)
                   : "ar.ccv", "memory"
               );
               break;
            case memory_order_acq_rel:
            case memory_order_seq_cst:
               __asm__ __volatile__ (
                   "mov ar.ccv = %[expected] ;;\n\t"
                   "cmpxchg8.rel %[current] = [%[pDest]], %[desired], ar.ccv\n\t"
                   "mf \n\t"
                   : [current] "=r" (current)
                   : [pDest] "r" (pDest), [expected] "r" (expected), [desired] "r" (desired)
                   : "ar.ccv", "memory"
               );
               break;
            default:
                assert(false);
            }

            bool bSuccess = expected == current;
            expected = current;
            if ( !bSuccess )
                fence_after( mo_fail );
            return bSuccess;
        }

        template <typename T>
        static inline bool cas_ptr_weak( T * volatile * pDest, T *& expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
        {
            return cas_ptr_strong( pDest, expected, desired, mo_success, mo_fail );
        }

        template <typename T>
        static inline T * exchange_ptr( T * volatile * pDest, T * val, memory_order order ) noexcept
        {
            static_assert( sizeof(T *) == 8, "Illegal size of operand" );
            assert( pDest );

            T * current;
            switch(order) {
                case memory_order_relaxed:
                case memory_order_consume:
                case memory_order_acquire:
                    __asm__ __volatile__ (
                        "xchg8 %[current] = [%[pDest]], %[val]\n\t"
                        : [current] "=r" (current)
                        : [pDest] "r" (pDest), [val] "r" (val)
                        : "memory"
                    );
                    break;
                case memory_order_acq_rel:
                case memory_order_release:
                case memory_order_seq_cst:
                    __asm__ __volatile__ (
                        "mf \n\t"
                        "xchg8 %[current] = [%[pDest]], %[val]\n\t"
                        : [current] "=r" (current)
                        : [pDest] "r" (pDest), [val] "r" (val)
                        : "memory"
                    );
                    break;
                default: assert(false);
            }
            return current;
        }


        template <typename T> struct atomic_pointer_sizeof { enum { value = sizeof(T) }; };
        template <> struct atomic_pointer_sizeof<void> { enum { value = 1 }; };

        // It does not work properly
        // atomic.fetch_add( ... ) returns nullptr, why?..
//#       define CDS_ATOMIC_fetch_ptr_add_defined
        template <typename T>
        static inline T * fetch_ptr_add( T * volatile * pDest, ptrdiff_t val, memory_order order) noexcept
        {
            static_assert( sizeof(T *) == 8, "Illegal size of operand" );
            assert( pDest );

            T * cur;
            val *= atomic_pointer_sizeof<T>::value;
            switch ( val ) {
                case 1:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 1 );
                    break;
                case 4:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 4 );
                    break;
                case 8:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 8 );
                    break;
                case 16:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, 16 );
                    break;
                default:
                    cur = load_ptr( pDest, memory_order_relaxed );
                    do {} while ( !cas_ptr_strong( pDest, cur, reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(cur) + val), order, memory_order_relaxed ));
                    break;
            }
            return cur;
        }

        // It does not work properly
        // atomic.fetch_sub( ... ) returns nullptr, why?..
//#       define CDS_ATOMIC_fetch_ptr_sub_defined
        template <typename T>
        static inline T * fetch_ptr_sub( T * volatile * pDest, ptrdiff_t val, memory_order order) noexcept
        {
            static_assert( sizeof(T *) == 8, "Illegal size of operand" );
            assert( pDest );
            T * cur;
            val *= atomic_pointer_sizeof<T>::value;
            switch ( val ) {
                case 1:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -1 );
                    break;
                case 4:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -4 );
                    break;
                case 8:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -8 );
                    break;
                case 16:
                    CDS_ITANIUM_ATOMIC_FETCH_ADD( 8, -16 );
                    break;
                default:
                    cur = load_ptr( pDest, memory_order_relaxed );
                    do {} while ( !cas_ptr_strong( pDest, cur, reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(cur) - val), order, memory_order_relaxed ));
                    break;
            }
            return cur;
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

#undef CDS_ITANIUM_ATOMIC_LOAD
#undef CDS_ITANIUM_ATOMIC_STORE
#undef CDS_ITANIUM_ATOMIC_CAS
#undef CDS_ITANIUM_ATOMIC_EXCHANGE
#undef CDS_ITANIUM_ATOMIC_FETCH_ADD

    }} // namespace gcc::ia64

    }   // namespace platform
}}  // namespace cds::cxx11_atomic
//@endcond

#endif // #ifndef CDSLIB_COMPILER_GCC_IA64_CXX11_ATOMIC_H
