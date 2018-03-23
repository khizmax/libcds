// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_CXX11_ATOMIC_H
#define CDSLIB_COMPILER_CXX11_ATOMIC_H
//@cond

#include <type_traits>  // make_unsigned
#include <cds/details/defs.h>
#include <cds/details/aligned_type.h>

namespace cds { namespace cxx11_atomic {
    typedef enum memory_order {
        memory_order_relaxed,
        memory_order_consume,
        memory_order_acquire,
        memory_order_release,
        memory_order_acq_rel,
        memory_order_seq_cst
    } memory_order;

}}  // namespace cds::cxx11_atomic


#if CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)
#   if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#       include <cds/compiler/vc/x86/cxx11_atomic.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_AMD64
#       include <cds/compiler/vc/amd64/cxx11_atomic.h>
#   else
#       error "MS VC++ compiler: unsupported processor architecture"
#   endif
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG || CDS_COMPILER == CDS_COMPILER_INTEL
#   if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#       include <cds/compiler/gcc/x86/cxx11_atomic.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_AMD64
#       include <cds/compiler/gcc/amd64/cxx11_atomic.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_IA64
#       include <cds/compiler/gcc/ia64/cxx11_atomic.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_SPARC
#       include <cds/compiler/gcc/sparc/cxx11_atomic.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_PPC64
#       include <cds/compiler/gcc/ppc64/cxx11_atomic.h>
//#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_ARM7
//#       include <cds/compiler/gcc/arm7/cxx11_atomic.h>
#   else
#       error "GCC compiler: unsupported processor architecture. Try to use native C++11 atomic or boost.atomic"
#   endif
#else
#   error "Undefined compiler"
#endif

namespace cds { namespace cxx11_atomic {

    // forward declarations
    template <class T>
    struct atomic;

    namespace details {

        template <typename T, size_t Size, typename Primary = T >
        struct atomic_generic_ops;

        template <typename T, size_t Size>
        struct atomic_integral_ops;

        template <size_t TypeSize>
        struct primary_type;

        template <>
        struct primary_type<1>
        {
            typedef std::uint8_t type;
        };
        template <>
        struct primary_type<2>
        {
            typedef std::uint16_t type;
        };
        template <>
        struct primary_type<4>
        {
            typedef std::uint32_t type;
        };
        template <>
        struct primary_type<8>
        {
            typedef std::uint64_t type;
        };
#if CDS_BUILD_BITS == 64 && CDS_DCAS_SUPPORT
        template <>
        struct primary_type<16>
        {
            typedef unsigned __int128_t type;
        };
#endif

        template <typename T, typename Primary>
        struct make_atomic_primary
        {
            typedef T       source_type;
            typedef Primary primary_type;

            static primary_type volatile * ptr( source_type volatile * p ) noexcept
            {
                return reinterpret_cast<primary_type volatile *>(p);
            }
            static primary_type const volatile * ptr( source_type const volatile * p ) noexcept
            {
                return reinterpret_cast<primary_type const volatile *>(p);
            }

            static primary_type val( source_type v ) noexcept
            {
                return *reinterpret_cast<primary_type*>(&v);
            }

            static primary_type& ref( source_type& v ) noexcept
            {
                return reinterpret_cast<primary_type&>(v);
            }

            static primary_type const& ref( source_type const& v ) noexcept
            {
                return reinterpret_cast<primary_type const&>(v);
            }

            static source_type ret( primary_type r ) noexcept
            {
                return *reinterpret_cast<source_type *>(&r);
            }
        };

        template <typename T>
        struct make_atomic_primary<T, T>
        {
            typedef T source_type;
            typedef T primary_type;

            static primary_type volatile * ptr( source_type volatile * p ) noexcept
            {
                return p;
            }
            static primary_type const volatile * ptr( source_type const volatile * p ) noexcept
            {
                return p;
            }

            static primary_type val( source_type v ) noexcept
            {
                return v;
            }

            static primary_type& ref( source_type& v ) noexcept
            {
                return v;
            }

            static source_type ret( primary_type r ) noexcept
            {
                return r;
            }
        };

        template <typename T>
        struct atomic_integral_bitwise_ops
        {
        public:
            typedef typename std::make_unsigned<T>::type unsigned_type;
            typedef atomic_generic_ops<unsigned_type, sizeof(unsigned_type)> atomic_ops;

            static T fetch_and(T volatile * pDest, T val, memory_order order) noexcept
            {
                unsigned_type cur = atomic_ops::atomic_load_explicit( reinterpret_cast<unsigned_type volatile *>(pDest), memory_order_relaxed );
                do {} while ( !atomic_ops::atomic_compare_exchange_weak_explicit(
                    reinterpret_cast<unsigned_type volatile *>(pDest), &cur, cur & unsigned_type(val), order, memory_order_relaxed ));
                return T(cur);
            }

            static T fetch_or(T volatile * pDest, T val, memory_order order) noexcept
            {
                unsigned_type cur = atomic_ops::atomic_load_explicit( reinterpret_cast<unsigned_type volatile *>(pDest), memory_order_relaxed );
                do {} while ( !atomic_ops::atomic_compare_exchange_weak_explicit(
                    reinterpret_cast<unsigned_type volatile *>(pDest), &cur, cur | unsigned_type(val), order, memory_order_relaxed ));
                return T(cur);
            }

            static T fetch_xor(T volatile * pDest, T val, memory_order order) noexcept
            {
                unsigned_type cur = atomic_ops::atomic_load_explicit( reinterpret_cast<unsigned_type volatile *>(pDest), memory_order_relaxed );
                do {} while ( !atomic_ops::atomic_compare_exchange_weak_explicit(
                    reinterpret_cast<unsigned_type volatile *>(pDest), &cur, cur ^ unsigned_type(val), order, memory_order_relaxed ));
                return T(cur);
            }
        };


        // 8-bit atomic operations

        template <typename T, typename Primary>
        struct atomic_generic_ops< T, 1, Primary >
        {
            typedef make_atomic_primary<T, Primary> primary;

            // store
            static void atomic_store_explicit( T volatile * pDest, T v, memory_order order ) noexcept
            {
                platform::store8( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store_explicit( T * pDest, T v, memory_order order ) noexcept
            {
                platform::store8( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store( T volatile * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }
            static void atomic_store( T * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }

            // load
            static T atomic_load_explicit( T volatile const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load8( primary::ptr(pSrc), order ));
            }
            static T atomic_load_explicit( T const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load8( primary::ptr(pSrc), order ));
            }
            static T atomic_load( T volatile const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }
            static T atomic_load( T const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }

            // exchange
            static T atomic_exchange_explicit( T volatile * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange8( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange_explicit( T * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange8( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange( T volatile * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_exchange( T * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }

            // cas
            static bool atomic_compare_exchange_weak_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas8_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas8_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_weak( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas8_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas8_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
        };

        template <typename T>
        struct atomic_integral_ops< T, 1 >
            : atomic_generic_ops<T, 1, T >
            , atomic_integral_bitwise_ops<T>
        {
            typedef atomic_integral_bitwise_ops<T> bitwise_ops;

            // fetch_add
            static T atomic_fetch_add_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch8_add_defined
                return platform::fetch8_add( pDest, val, order );
#           else
                T cur = atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !atomic_compare_exchange_weak_explicit( pDest, &cur, cur + val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_add_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_add_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_add( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_add( T * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_sub
            static T atomic_fetch_sub_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch8_sub_defined
                return platform::fetch8_sub( pDest, val, order );
#           else
                T cur = atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !atomic_compare_exchange_weak_explicit( pDest, &cur, cur - val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_sub_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_sub_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_sub( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_sub( T * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_and
            static T atomic_fetch_and_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch8_and_defined
                return platform::fetch8_and( pDest, val, order );
#           else
                return bitwise_ops::fetch_and( pDest, val, order );
#           endif
            }
            static T atomic_fetch_and_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_and_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_and( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_and( T * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_or
            static T atomic_fetch_or_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch8_or_defined
                return platform::fetch8_or( pDest, val, order );
#           else
                return bitwise_ops::fetch_or( pDest, val, order );
#           endif
            }
            static T atomic_fetch_or_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_or_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_or( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_or( T * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_xor
            static T atomic_fetch_xor_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch8_xor_defined
                return platform::fetch8_xor( pDest, val, order );
#           else
                return bitwise_ops::fetch_xor( pDest, val, order );
#           endif
            }
            static T atomic_fetch_xor_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_xor_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_xor( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_xor( T * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
        };

        // 16-bit atomic operations

        template <typename T, typename Primary>
        struct atomic_generic_ops< T, 2, Primary >
        {
            typedef make_atomic_primary<T, Primary> primary;

            // store
            static void atomic_store_explicit( T volatile * pDest, T v, memory_order order ) noexcept
            {
                platform::store16( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store_explicit( T * pDest, T v, memory_order order ) noexcept
            {
                platform::store16( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store( T volatile * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }
            static void atomic_store( T * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }

            // load
            static T atomic_load_explicit( T volatile const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load16( primary::ptr(pSrc), order ));
            }
            static T atomic_load_explicit( T const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load16( primary::ptr(pSrc), order ));
            }
            static T atomic_load( T volatile const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }
            static T atomic_load( T const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }

            // exchange
            static T atomic_exchange_explicit( T volatile * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange16( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange_explicit( T * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange16( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange( T volatile * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_exchange( T * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }

            // cas
            static bool atomic_compare_exchange_weak_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas16_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas16_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, primary::val(desired), memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_weak( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas16_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas16_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
        };

        template <typename T>
        struct atomic_integral_ops< T, 2 >
            : atomic_generic_ops< T, 2, T >
            , atomic_integral_bitwise_ops<T>
        {
            typedef atomic_integral_bitwise_ops<T> bitwise_ops;

            // fetch_add
            static T atomic_fetch_add_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch16_add_defined
                return platform::fetch16_add( pDest, val, order );
#           else
                T cur = atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !atomic_compare_exchange_weak_explicit( pDest, &cur, cur + val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_add_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_add_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_add( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_add( T * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_sub
            static T atomic_fetch_sub_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch16_sub_defined
                return platform::fetch16_sub( pDest, val, order );
#           else
                T cur = atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !atomic_compare_exchange_weak_explicit( pDest, &cur, cur - val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_sub_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_sub_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_sub( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_sub( T * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_and
            static T atomic_fetch_and_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch16_and_defined
                return platform::fetch16_and( pDest, val, order );
#           else
                return bitwise_ops::fetch_and( pDest, val, order );
#           endif
            }
            static T atomic_fetch_and_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_and_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_and( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_and( T * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_or
            static T atomic_fetch_or_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch16_or_defined
                return platform::fetch16_or( pDest, val, order );
#           else
                return bitwise_ops::fetch_or( pDest, val, order );
#           endif
            }
            static T atomic_fetch_or_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_or_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_or( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_or( T * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_xor
            static T atomic_fetch_xor_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch16_xor_defined
                return platform::fetch16_xor( pDest, val, order );
#           else
                return bitwise_ops::fetch_xor( pDest, val, order );
#           endif
            }
            static T atomic_fetch_xor_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_xor_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_xor( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_xor( T * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
        };

        // 32-bit atomic operations

        template <typename T, typename Primary>
        struct atomic_generic_ops< T, 4, Primary >
        {
            typedef make_atomic_primary<T, Primary> primary;

            // store
            static void atomic_store_explicit( T volatile * pDest, T v, memory_order order ) noexcept
            {
                platform::store32( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store_explicit( T * pDest, T v, memory_order order ) noexcept
            {
                platform::store32( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store( T volatile * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }
            static void atomic_store( T * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }

            // load
            static T atomic_load_explicit( T volatile const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load32( primary::ptr(pSrc), order ));
            }
            static T atomic_load_explicit( T const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load32( primary::ptr(pSrc), order ));
            }
            static T atomic_load( T volatile const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }
            static T atomic_load( T const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }

            // exchange
            static T atomic_exchange_explicit( T volatile * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange32( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange_explicit( T * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange32( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange( T volatile * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_exchange( T * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }

            // cas
            static bool atomic_compare_exchange_weak_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas32_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas32_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_weak( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas32_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas32_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
        };

        template <typename T>
        struct atomic_integral_ops< T, 4 >
            : atomic_generic_ops< T, 4, T >
            , atomic_integral_bitwise_ops<T>
        {
            typedef atomic_integral_bitwise_ops<T> bitwise_ops;
            // fetch_add
            static T atomic_fetch_add_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch32_add_defined
                return platform::fetch32_add( pDest, val, order );
#           else
                T cur = atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !atomic_compare_exchange_weak_explicit( pDest, &cur, cur + val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_add_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_add_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_add( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_add( T * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_sub
            static T atomic_fetch_sub_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch32_sub_defined
                return platform::fetch32_sub( pDest, val, order );
#           else
                T cur = atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !atomic_compare_exchange_weak_explicit( pDest, &cur, cur - val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_sub_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_sub_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_sub( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_sub( T * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_and
            static T atomic_fetch_and_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch32_and_defined
                return platform::fetch32_and( pDest, val, order );
#           else
                return bitwise_ops::fetch_and( pDest, val, order );
#           endif
            }
            static T atomic_fetch_and_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_and_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_and( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_and( T * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_or
            static T atomic_fetch_or_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch32_or_defined
                return platform::fetch32_or( pDest, val, order );
#           else
                return bitwise_ops::fetch_or( pDest, val, order );
#           endif
            }
            static T atomic_fetch_or_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_or_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_or( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_or( T * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_xor
            static T atomic_fetch_xor_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch32_xor_defined
                return platform::fetch32_xor( pDest, val, order );
#           else
                return bitwise_ops::fetch_xor( pDest, val, order );
#           endif
            }
            static T atomic_fetch_xor_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_xor_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_xor( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_xor( T * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
        };


        // 64-bit atomic operations

        template <typename T, typename Primary>
        struct atomic_generic_ops< T, 8, Primary >
        {
            typedef make_atomic_primary<T, Primary> primary;

            // store
            static void atomic_store_explicit( T volatile * pDest, T v, memory_order order ) noexcept
            {
                platform::store64( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store_explicit( T * pDest, T v, memory_order order ) noexcept
            {
                platform::store64( primary::ptr(pDest), primary::val(v), order );
            }
            static void atomic_store( T volatile * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }
            static void atomic_store( T * pDest, T v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }

            // load
            static T atomic_load_explicit( T volatile const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load64( primary::ptr(pSrc), order ));
            }
            static T atomic_load_explicit( T const * pSrc, memory_order order ) noexcept
            {
                return primary::ret( platform::load64( primary::ptr(pSrc), order ));
            }
            static T atomic_load( T volatile const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }
            static T atomic_load( T const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }

            // exchange
            static T atomic_exchange_explicit( T volatile * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange64( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange_explicit( T * pDest, T val, memory_order order ) noexcept
            {
                return primary::ret( platform::exchange64( primary::ptr(pDest), primary::val(val), order ));
            }
            static T atomic_exchange( T volatile * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_exchange( T * pDest, T val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }

            // cas
            static bool atomic_compare_exchange_weak_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas64_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas64_weak( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_weak( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong_explicit( T volatile * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas64_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong_explicit( T * pDest, T * expected, T desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas64_strong( primary::ptr(pDest), primary::ref(*expected), primary::val(desired), mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong( T volatile * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong( T * pDest, T * expected, T desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
        };


        template <typename T>
        struct atomic_integral_ops< T, 8 >
            : atomic_generic_ops< T, 8, T >
            , atomic_integral_bitwise_ops<T>
        {
            typedef atomic_integral_bitwise_ops<T>  bitwise_ops;
            typedef atomic_generic_ops<T, 8, T>     general_ops;

            // fetch_add
            static T atomic_fetch_add_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch64_add_defined
                return platform::fetch64_add( pDest, val, order );
#           else
                T cur = general_ops::atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !general_ops::atomic_compare_exchange_weak_explicit( pDest, &cur, cur + val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_add_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_add_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_add( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_add( T * pDest, T val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_sub
            static T atomic_fetch_sub_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch64_sub_defined
                return platform::fetch64_sub( pDest, val, order );
#           else
                T cur = general_ops::atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !general_ops::atomic_compare_exchange_weak_explicit( pDest, &cur, cur - val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T atomic_fetch_sub_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_sub_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_sub( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_sub( T * pDest, T val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_and
            static T atomic_fetch_and_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch64_and_defined
                return platform::fetch64_and( pDest, val, order );
#           else
                return bitwise_ops::fetch_and( pDest, val, order );
#           endif
            }
            static T atomic_fetch_and_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_and_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_and( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_and( T * pDest, T val ) noexcept
            {
                return atomic_fetch_and_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_or
            static T atomic_fetch_or_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch64_or_defined
                return platform::fetch64_or( pDest, val, order );
#           else
                return bitwise_ops::fetch_or( pDest, val, order );
#           endif
            }
            static T atomic_fetch_or_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_or_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_or( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_or( T * pDest, T val ) noexcept
            {
                return atomic_fetch_or_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_xor
            static T atomic_fetch_xor_explicit(T volatile * pDest, T val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch64_xor_defined
                return platform::fetch64_xor( pDest, val, order );
#           else
                return bitwise_ops::fetch_xor( pDest, val, order );
#           endif
            }
            static T atomic_fetch_xor_explicit(T * pDest, T val , memory_order order) noexcept
            {
                return atomic_fetch_xor_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T atomic_fetch_xor( T volatile * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
            static T atomic_fetch_xor( T * pDest, T val ) noexcept
            {
                return atomic_fetch_xor_explicit( pDest, val, memory_order_seq_cst );
            }
        };


        // atomic pointer operations
        template <typename T>
        struct atomic_pointer_base
        {
            // store
            static void atomic_store_explicit( T * volatile * pDest, T * v, memory_order order ) noexcept
            {
                platform::store_ptr( pDest, v, order );
            }
            static void atomic_store_explicit( T * * pDest, T * v, memory_order order ) noexcept
            {
                platform::store_ptr( pDest, v, order );
            }
            static void atomic_store( T * volatile * pDest, T * v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }
            static void atomic_store( T * * pDest, T * v ) noexcept
            {
                atomic_store_explicit( pDest, v, memory_order_seq_cst );
            }

            // load
            static T * atomic_load_explicit( T * volatile const * pSrc, memory_order order ) noexcept
            {
                return platform::load_ptr( pSrc, order );
            }
            static T * atomic_load_explicit( T * const * pSrc, memory_order order ) noexcept
            {
                return platform::load_ptr( pSrc, order );
            }
            static T * atomic_load( T * volatile const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }
            static T * atomic_load( T * const * pSrc ) noexcept
            {
                return atomic_load_explicit( pSrc, memory_order_seq_cst );
            }

            // exchange
            static T * atomic_exchange_explicit( T * volatile * pDest, T * val, memory_order order ) noexcept
            {
                return platform::exchange_ptr( pDest, val, order );
            }
            static T * atomic_exchange_explicit( T * * pDest, T * val, memory_order order ) noexcept
            {
                return platform::exchange_ptr( pDest, val, order );
            }
            static T * atomic_exchange( T * volatile * pDest, T * val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }
            static T * atomic_exchange( T * * pDest, T * val ) noexcept
            {
                return atomic_exchange_explicit( pDest, val, memory_order_seq_cst );
            }

            // cas
            static bool atomic_compare_exchange_weak_explicit( T * volatile * pDest, T * * expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas_ptr_weak( pDest, *expected, desired, mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak_explicit( T * * pDest, T * * expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas_ptr_weak( pDest, *expected, desired, mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_weak( T * volatile * pDest, T ** expected, T * desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_weak( T ** pDest, T ** expected, T * desired ) noexcept
            {
                return atomic_compare_exchange_weak_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong_explicit( T * volatile * pDest, T ** expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas_ptr_strong( pDest, *expected, desired, mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong_explicit( T ** pDest, T ** expected, T * desired, memory_order mo_success, memory_order mo_fail ) noexcept
            {
                assert( expected );
                return platform::cas_ptr_strong( pDest, *expected, desired, mo_success, mo_fail );
            }
            static bool atomic_compare_exchange_strong( T * volatile * pDest, T ** expected, T * desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
            static bool atomic_compare_exchange_strong( T ** pDest, T ** expected, T * desired ) noexcept
            {
                return atomic_compare_exchange_strong_explicit( pDest, expected, desired, memory_order_seq_cst, memory_order_relaxed );
            }
        };

        template <typename T>
        struct atomic_pointer: public atomic_pointer_base<T>
        {
            typedef atomic_pointer_base<T> base_class;
            // fetch_add
            static T * atomic_fetch_add_explicit(T * volatile * pDest, ptrdiff_t val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch_ptr_add_defined
                platform::fetch_ptr_add( pDest, val, order );
#           else
                T * cur = base_class::atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !base_class::atomic_compare_exchange_weak_explicit( pDest, &cur, cur + val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T * atomic_fetch_add_explicit(T * * pDest, ptrdiff_t val , memory_order order) noexcept
            {
                return atomic_fetch_add_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T * atomic_fetch_add( T * volatile * pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }
            static T * atomic_fetch_add( T ** pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_sub
            static T * atomic_fetch_sub_explicit(T * volatile * pDest, ptrdiff_t val, memory_order order) noexcept
            {
#           ifdef CDS_ATOMIC_fetch_ptr_sub_defined
                platform::fetch_ptr_sub( pDest, val, order );
#           else
                T * cur = base_class::atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !base_class::atomic_compare_exchange_weak_explicit( pDest, &cur, cur - val, order, memory_order_relaxed ));
                return cur;
#           endif
            }
            static T * atomic_fetch_sub_explicit(T ** pDest, ptrdiff_t val , memory_order order) noexcept
            {
                return atomic_fetch_sub_explicit( reinterpret_cast<T volatile *>( pDest ), val, order );
            }
            static T * atomic_fetch_sub( T volatile * pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
            static T * atomic_fetch_sub( T * pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
        };

        template <>
        struct atomic_pointer<void>: public atomic_pointer_base<void>
        {
            typedef atomic_pointer_base<void>   base_class;

            // fetch_add
            static void * atomic_fetch_add_explicit(void * volatile * pDest, ptrdiff_t val, memory_order order) noexcept
            {
                void * cur = base_class::atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !base_class::atomic_compare_exchange_weak_explicit( pDest, &cur, reinterpret_cast<char *>(cur) + val, order, memory_order_relaxed ));
                return cur;
            }
            static void * atomic_fetch_add_explicit(void * * pDest, ptrdiff_t val , memory_order order) noexcept
            {
                return atomic_fetch_add_explicit( reinterpret_cast<void * volatile *>( pDest ), val, order );
            }
            static void * atomic_fetch_add( void * volatile * pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }
            static void * atomic_fetch_add( void ** pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_add_explicit( pDest, val, memory_order_seq_cst );
            }

            // fetch_sub
            static void * atomic_fetch_sub_explicit(void * volatile * pDest, ptrdiff_t val, memory_order order) noexcept
            {
                void * cur = base_class::atomic_load_explicit( pDest, memory_order_relaxed );
                do {} while ( !base_class::atomic_compare_exchange_weak_explicit( pDest, &cur, reinterpret_cast<char *>(cur) - val, order, memory_order_relaxed ));
                return cur;
            }
            static void * atomic_fetch_sub_explicit(void ** pDest, ptrdiff_t val , memory_order order) noexcept
            {
                return atomic_fetch_sub_explicit( reinterpret_cast<void * volatile *>( pDest ), val, order );
            }
            static void * atomic_fetch_sub( void * volatile * pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
            static void * atomic_fetch_sub( void ** pDest, ptrdiff_t val ) noexcept
            {
                return atomic_fetch_sub_explicit( pDest, val, memory_order_seq_cst );
            }
        };

        template <typename T>
        struct atomic_integral
        {
        private:
            typename cds::details::aligned_type<T, sizeof(T)>::type volatile m_val;
            //T volatile  m_val;
            typedef atomic_integral_ops<T, sizeof(T)>   atomic_ops;
        public:
            typedef T   atomic_type;
        public:
            bool is_lock_free() const volatile noexcept
            {
                return true;
            }
            bool is_lock_free() const noexcept
            {
                return true;
            }
            void store(T val, memory_order order = memory_order_seq_cst) volatile noexcept
            {
                atomic_ops::atomic_store_explicit( &m_val, val, order );
            }
            void store(T val, memory_order order = memory_order_seq_cst) noexcept
            {
                atomic_ops::atomic_store_explicit( &m_val, val, order );
            }

            T load(memory_order order = memory_order_seq_cst) const volatile noexcept
            {
                return atomic_ops::atomic_load_explicit( &m_val, order );
            }
            T load(memory_order order  = memory_order_seq_cst) const noexcept
            {
                return atomic_ops::atomic_load_explicit( &m_val, order );
            }

            operator T() const volatile noexcept
            {
                return load();
            }
            operator T() const noexcept
            {
                return load();
            }

            T exchange(T val, memory_order order = memory_order_seq_cst) volatile noexcept
            {
                return atomic_ops::atomic_exchange_explicit( &m_val, val, order );
            }
            T exchange(T val, memory_order order = memory_order_seq_cst) noexcept
            {
                return atomic_ops::atomic_exchange_explicit( &m_val, val, order );
            }

            bool compare_exchange_weak(T& expected, T desired , memory_order success_order, memory_order failure_order) volatile noexcept
            {
                return atomic_ops::atomic_compare_exchange_weak_explicit( &m_val, &expected, desired, success_order, failure_order );
            }
            bool compare_exchange_weak(T& expected, T desired , memory_order success_order, memory_order failure_order) noexcept
            {
                return atomic_ops::atomic_compare_exchange_weak_explicit( &m_val, &expected, desired, success_order, failure_order );
            }
            bool compare_exchange_strong(T& expected, T desired , memory_order success_order, memory_order failure_order) volatile noexcept
            {
                return atomic_ops::atomic_compare_exchange_strong_explicit( &m_val, &expected, desired, success_order, failure_order );
            }
            bool compare_exchange_strong(T& expected, T desired , memory_order success_order, memory_order failure_order) noexcept
            {
                return atomic_ops::atomic_compare_exchange_strong_explicit( &m_val, &expected, desired, success_order, failure_order );
            }
            bool compare_exchange_weak(T& expected, T desired , memory_order success_order = memory_order_seq_cst) volatile noexcept
            {
                return compare_exchange_weak( expected, desired, success_order, memory_order_relaxed );
            }
            bool compare_exchange_weak(T& expected, T desired , memory_order success_order = memory_order_seq_cst) noexcept
            {
                return compare_exchange_weak( expected, desired, success_order, memory_order_relaxed );
            }
            bool compare_exchange_strong(T& expected, T desired , memory_order success_order = memory_order_seq_cst) volatile noexcept
            {
                return compare_exchange_strong( expected, desired, success_order, memory_order_relaxed );
            }
            bool compare_exchange_strong(T& expected, T desired , memory_order success_order = memory_order_seq_cst) noexcept
            {
                return compare_exchange_strong( expected, desired, success_order, memory_order_relaxed );
            }

            T fetch_add(T val, memory_order order = memory_order_seq_cst) volatile noexcept
            {
                return atomic_ops::atomic_fetch_add_explicit( &m_val, val, order );
            }
            T fetch_add(T val, memory_order order = memory_order_seq_cst) noexcept
            {
                return atomic_ops::atomic_fetch_add_explicit( &m_val, val, order );
            }
            T fetch_sub(T val, memory_order order = memory_order_seq_cst) volatile noexcept
            {
                return atomic_ops::atomic_fetch_sub_explicit( &m_val, val, order );
            }
            T fetch_sub(T val, memory_order order = memory_order_seq_cst) noexcept
            {
                return atomic_ops::atomic_fetch_sub_explicit( &m_val, val, order );
            }
            T fetch_and(T val, memory_order order = memory_order_seq_cst) volatile noexcept
            {
                return atomic_ops::atomic_fetch_and_explicit( &m_val, val, order );
            }
            T fetch_and(T val, memory_order order = memory_order_seq_cst) noexcept
            {
                return atomic_ops::atomic_fetch_and_explicit( &m_val, val, order );
            }

            T fetch_or(T val, memory_order order = memory_order_seq_cst) volatile noexcept
            {
                return atomic_ops::atomic_fetch_or_explicit( &m_val, val, order );
            }
            T fetch_or(T val, memory_order order = memory_order_seq_cst) noexcept
            {
                return atomic_ops::atomic_fetch_or_explicit( &m_val, val, order );
            }
            T fetch_xor(T val, memory_order order = memory_order_seq_cst) volatile noexcept
            {
                return atomic_ops::atomic_fetch_xor_explicit( &m_val, val, order );
            }
            T fetch_xor(T val, memory_order order = memory_order_seq_cst) noexcept
            {
                return atomic_ops::atomic_fetch_xor_explicit( &m_val, val, order );
            }

            atomic_integral() = default;
            constexpr atomic_integral(T val) noexcept
                : m_val(val)
                {}

            atomic_integral(const atomic_integral&) = delete;
            atomic_integral& operator=(const atomic_integral&) = delete;
            atomic_integral& operator=(const atomic_integral&) volatile = delete;

            T operator=(T val) volatile noexcept
            {
                store(val);
                return val;
            }
            T operator=(T val) noexcept
            {
                store(val);
                return val;
            }

            // Post inc/dec
            T operator++(int) volatile noexcept
            {
                return fetch_add( 1 );
            }
            T operator++(int) noexcept
            {
                return fetch_add( 1 );
            }
            T operator--(int) volatile noexcept
            {
                return fetch_sub( 1 );
            }
            T operator--(int) noexcept
            {
                return fetch_sub( 1 );
            }

            // Pre inc/dec
            T operator++() volatile noexcept
            {
                return fetch_add( 1 ) + 1;
            }
            T operator++() noexcept
            {
                return fetch_add( 1 ) + 1;
            }
            T operator--() volatile noexcept
            {
                return fetch_sub( 1 ) - 1;
            }
            T operator--() noexcept
            {
                return fetch_sub( 1 ) - 1;
            }

            // op=
            T operator+=(T val) volatile noexcept
            {
                return fetch_add( val ) + val;
            }
            T operator+=(T val) noexcept
            {
                return fetch_add( val ) + val;
            }
            T operator-=(T val) volatile noexcept
            {
                return fetch_sub( val ) - val;
            }
            T operator-=(T val) noexcept
            {
                return fetch_sub( val ) - val;
            }
            T operator&=(T val) volatile noexcept
            {
                return fetch_and( val ) & val;
            }
            T operator&=(T val) noexcept
            {
                return fetch_and( val ) & val;
            }
            T operator|=(T val) volatile noexcept
            {
                return fetch_or( val ) | val;
            }
            T operator|=(T val) noexcept
            {
                return fetch_or( val ) | val;
            }
            T operator^=(T val) volatile noexcept
            {
                return fetch_xor( val ) ^ val;
            }
            T operator^=(T val) noexcept
            {
                return fetch_xor( val ) ^ val;
            }
        };

        template <typename Type>
        struct select_primary_type {
            typedef typename details::primary_type<sizeof(Type)>::type type;
        };
        template <>
        struct select_primary_type<bool> {
            typedef bool type;
        };

    }   // namespace details

    template <class T>
    struct atomic
    {
    private:
        typedef details::atomic_generic_ops<T, sizeof(T), typename details::select_primary_type<T>::type >  atomic_ops;

        T volatile m_data;
    public:
        bool is_lock_free() const volatile noexcept
        {
            return true;
        }
        bool is_lock_free() const noexcept
        {
            return true;
        }

        void store(T val, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            atomic_ops::atomic_store_explicit( &m_data, val, order );
        }
        void store(T val, memory_order order = memory_order_seq_cst) noexcept
        {
            atomic_ops::atomic_store_explicit( &m_data, val, order );
        }

        T load(memory_order order = memory_order_seq_cst) const volatile noexcept
        {
            return atomic_ops::atomic_load_explicit( &m_data, order );
        }
        T load(memory_order order = memory_order_seq_cst) const noexcept
        {
           return atomic_ops::atomic_load_explicit( &m_data, order );
        }

        operator T() const volatile noexcept
        {
            return load();
        }
        operator T() const noexcept
        {
            return load();
        }

        T exchange(T val, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return atomic_ops::atomic_exchange_explicit( &m_data, val, order );
        }
        T exchange(T val, memory_order order = memory_order_seq_cst) noexcept
        {
            return atomic_ops::atomic_exchange_explicit( &m_data, val, order );
        }

        bool compare_exchange_weak(T& expected, T desired, memory_order success_order, memory_order failure_order) volatile noexcept
        {
            return atomic_ops::atomic_compare_exchange_weak_explicit( &m_data, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_weak(T& expected, T desired, memory_order success_order, memory_order failure_order) noexcept
        {
            return atomic_ops::atomic_compare_exchange_weak_explicit( &m_data, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_strong(T& expected, T desired, memory_order success_order, memory_order failure_order) volatile noexcept
        {
            return atomic_ops::atomic_compare_exchange_strong_explicit( &m_data, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_strong(T& expected, T desired, memory_order success_order, memory_order failure_order) noexcept
        {
            return atomic_ops::atomic_compare_exchange_strong_explicit( &m_data, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_weak(T& expected, T desired, memory_order success_order = memory_order_seq_cst) volatile noexcept
        {
            return compare_exchange_weak( expected, desired, success_order, memory_order_relaxed );
        }
        bool compare_exchange_weak(T& expected, T desired, memory_order success_order = memory_order_seq_cst) noexcept
        {
            return compare_exchange_weak( expected, desired, success_order, memory_order_relaxed );
        }
        bool compare_exchange_strong(T& expected, T desired, memory_order success_order = memory_order_seq_cst) volatile noexcept
        {
            return compare_exchange_strong( expected, desired, success_order, memory_order_relaxed );
        }
        bool compare_exchange_strong(T& expected, T desired, memory_order success_order = memory_order_seq_cst) noexcept
        {
            return compare_exchange_strong( expected, desired, success_order, memory_order_relaxed );
        }

        atomic() = default;
        constexpr atomic(T val)
            : m_data( val )
            {}

        atomic(const atomic&) = delete;
        atomic& operator=(const atomic&) = delete;
        atomic& operator=(const atomic&) volatile = delete;

        T operator=(T val) volatile noexcept
        {
            store( val );
            return val;
        }
        T operator=(T val) noexcept
        {
            store( val );
            return val;
        }
    };

#   define CDS_DECLARE_ATOMIC_INTEGRAL( _type ) \
    template <> \
    struct atomic<_type>: public details::atomic_integral<_type> \
    { \
    private: \
        typedef details::atomic_integral<_type>   base_class  ; \
    public: \
        atomic() = default; \
        atomic(_type val) noexcept : base_class(val) {} \
        atomic(const atomic&) = delete; \
        atomic& operator=(const atomic&) = delete; \
        atomic& operator=(const atomic&) volatile = delete; \
        _type operator=(_type val) volatile noexcept { return base_class::operator=(val); } \
        _type operator=(_type val) noexcept { return base_class::operator=(val); } \
    };

    CDS_DECLARE_ATOMIC_INTEGRAL(char)
    CDS_DECLARE_ATOMIC_INTEGRAL(signed char)
    CDS_DECLARE_ATOMIC_INTEGRAL(unsigned char)
    CDS_DECLARE_ATOMIC_INTEGRAL(short)
    CDS_DECLARE_ATOMIC_INTEGRAL(unsigned short)
    CDS_DECLARE_ATOMIC_INTEGRAL(int)
    CDS_DECLARE_ATOMIC_INTEGRAL(unsigned int)
    CDS_DECLARE_ATOMIC_INTEGRAL(long)
    CDS_DECLARE_ATOMIC_INTEGRAL(unsigned long)
    CDS_DECLARE_ATOMIC_INTEGRAL(long long)
    CDS_DECLARE_ATOMIC_INTEGRAL(unsigned long long)
//#if CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION >= 40400
//    CDS_DECLARE_ATOMIC_INTEGRAL(char16_t)
//    CDS_DECLARE_ATOMIC_INTEGRAL(char32_t)
//#endif
//    CDS_DECLARE_ATOMIC_INTEGRAL(wchar_t)

#   undef CDS_DECLARE_ATOMIC_INTEGRAL


    template <typename T>
    class atomic<T *>
    {
    private:
        T * volatile m_ptr;
        typedef details::atomic_pointer<T>  atomic_ops;
    public:
        bool is_lock_free() const volatile noexcept
        {
            return true;
        }
        bool is_lock_free() const noexcept
        {
            return true;
        }

        void store(T * val, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            atomic_ops::atomic_store_explicit( &m_ptr, val, order );
        }
        void store(T * val, memory_order order = memory_order_seq_cst) noexcept
        {
            atomic_ops::atomic_store_explicit( &m_ptr, val, order );
        }

        T * load(memory_order order = memory_order_seq_cst) const volatile noexcept
        {
            return atomic_ops::atomic_load_explicit( &m_ptr, order );
        }
        T * load(memory_order order = memory_order_seq_cst) const noexcept
        {
            return atomic_ops::atomic_load_explicit( &m_ptr, order );
        }

        operator T *() const volatile noexcept
        {
            return load();
        }
        operator T *() const noexcept
        {
            return load();
        }

        T * exchange(T * val, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return atomic_ops::atomic_exchange_explicit( &m_ptr, val, order );
        }
        T * exchange(T * val, memory_order order = memory_order_seq_cst) noexcept
        {
            return atomic_ops::atomic_exchange_explicit( &m_ptr, val, order );
        }

        bool compare_exchange_weak(T *& expected, T * desired, memory_order success_order, memory_order failure_order) volatile noexcept
        {
            return atomic_ops::atomic_compare_exchange_weak_explicit( &m_ptr, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_weak(T *& expected, T * desired, memory_order success_order, memory_order failure_order) noexcept
        {
            return atomic_ops::atomic_compare_exchange_weak_explicit( &m_ptr, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_strong(T *& expected, T * desired, memory_order success_order, memory_order failure_order) volatile noexcept
        {
            return atomic_ops::atomic_compare_exchange_strong_explicit( &m_ptr, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_strong(T *& expected, T * desired, memory_order success_order, memory_order failure_order) noexcept
        {
            return atomic_ops::atomic_compare_exchange_strong_explicit( &m_ptr, &expected, desired, success_order, failure_order );
        }
        bool compare_exchange_weak(T *& expected, T * desired, memory_order success_order = memory_order_seq_cst) volatile noexcept
        {
            return compare_exchange_weak( expected, desired, success_order, memory_order_relaxed );
        }
        bool compare_exchange_weak(T *& expected, T * desired, memory_order success_order = memory_order_seq_cst) noexcept
        {
            return compare_exchange_weak( expected, desired, success_order, memory_order_relaxed );
        }
        bool compare_exchange_strong(T *& expected, T * desired, memory_order success_order = memory_order_seq_cst) volatile noexcept
        {
            return compare_exchange_strong( expected, desired, success_order, memory_order_relaxed );
        }
        bool compare_exchange_strong(T *& expected, T * desired, memory_order success_order = memory_order_seq_cst) noexcept
        {
            return compare_exchange_strong( expected, desired, success_order, memory_order_relaxed );
        }

        T * fetch_add(ptrdiff_t offset, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return atomic_ops::atomic_fetch_add_explicit( &m_ptr, offset, order );
        }
        T * fetch_add(ptrdiff_t offset, memory_order order = memory_order_seq_cst) noexcept
        {
            return atomic_ops::atomic_fetch_add_explicit( &m_ptr, offset, order );
        }

        T * fetch_sub(ptrdiff_t offset, memory_order order = memory_order_seq_cst) volatile noexcept
        {
            return atomic_ops::atomic_fetch_sub_explicit( &m_ptr, offset, order );
        }
        T * fetch_sub(ptrdiff_t offset, memory_order order = memory_order_seq_cst) noexcept
        {
            return atomic_ops::atomic_fetch_sub_explicit( &m_ptr, offset, order );
        }

        atomic() = default;
        constexpr atomic(T * val) noexcept
            : m_ptr( val )
        {}

        atomic(const atomic&) = delete;
        atomic& operator=(const atomic&) = delete;
        atomic& operator=(const atomic&) volatile = delete;

        T * operator=(T * val) volatile noexcept
        {
            store( val );
            return val;
        }
        T * operator=(T * val) noexcept
        {
            store( val );
            return val;
        }
    };

    // Atomic typedefs
    typedef atomic<bool>            atomic_bool;
    typedef atomic<char>            atomic_char;
    typedef atomic<signed char>     atomic_schar;
    typedef atomic<unsigned char>   atomic_uchar;
    typedef atomic<short>           atomic_short;
    typedef atomic<unsigned short>  atomic_ushort;
    typedef atomic<int>             atomic_int;
    typedef atomic<unsigned int>    atomic_uint;
    typedef atomic<long>            atomic_long;
    typedef atomic<unsigned long>   atomic_ulong;
    typedef atomic<long long>       atomic_llong;
    typedef atomic<unsigned long long> atomic_ullong;
#if ( CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION >= 40400 ) || CDS_COMPILER == CDS_COMPILER_CLANG
    typedef atomic<char16_t>        atomic_char16_t;
    typedef atomic<char32_t>        atomic_char32_t;
#endif
    typedef atomic<wchar_t>         atomic_wchar_t;


    typedef atomic<std::int_least8_t>    atomic_int_least8_t;
    typedef atomic<std::uint_least8_t>   atomic_uint_least8_t;
    typedef atomic<std::int_least16_t>   atomic_int_least16_t;
    typedef atomic<std::uint_least16_t>  atomic_uint_least16_t;
    typedef atomic<std::int_least32_t>   atomic_int_least32_t;
    typedef atomic<std::uint_least32_t>  atomic_uint_least32_t;
    typedef atomic<std::int_least64_t>   atomic_int_least64_t;
    typedef atomic<std::uint_least64_t>  atomic_uint_least64_t;
    typedef atomic<std::int_fast8_t>     atomic_int_fast8_t;
    typedef atomic<std::uint_fast8_t>    atomic_uint_fast8_t;
    typedef atomic<std::int_fast16_t>    atomic_int_fast16_t;
    typedef atomic<std::uint_fast16_t>   atomic_uint_fast16_t;
    typedef atomic<std::int_fast32_t>    atomic_int_fast32_t;
    typedef atomic<std::uint_fast32_t>   atomic_uint_fast32_t;
    typedef atomic<std::int_fast64_t>    atomic_int_fast64_t;
    typedef atomic<std::uint_fast64_t>   atomic_uint_fast64_t;
    typedef atomic<intptr_t>             atomic_intptr_t;
    typedef atomic<uintptr_t>            atomic_uintptr_t;
    typedef atomic<size_t>               atomic_size_t;
    typedef atomic<ptrdiff_t>            atomic_ptrdiff_t;
    typedef atomic<std::intmax_t>        atomic_intmax_t;
    typedef atomic<std::uintmax_t>       atomic_uintmax_t;

    template <class T>
    static inline bool atomic_is_lock_free(const volatile atomic<T> * p) noexcept
    {
        return p->is_lock_free();
    }

    template <class T>
    static inline bool atomic_is_lock_free(const atomic<T> * p ) noexcept
    {
        return p->is_lock_free();
    }

    /*
    template <class T>
    static inline void atomic_init(volatile atomic<T> * p, T val) noexcept
    {
        p->init( val );
    }

    template <class T>
    static inline void atomic_init( atomic<T> * p, T val) noexcept
    {
        p->init( val );
    }
    */

    template <class T>
    static inline void atomic_store(volatile atomic<T>* p, T val) noexcept
    {
        p->store(val);
    }
    template <class T>
    static inline void atomic_store(atomic<T>* p, T val) noexcept
    {
        p->store( val );
    }

    template <class T>
    static inline void atomic_store_explicit(volatile atomic<T>* p, T val, memory_order order) noexcept
    {
        p->store( val, order );
    }
    template <class T>
    static inline void atomic_store_explicit(atomic<T>* p, T val, memory_order order) noexcept
    {
        p->store( val, order );
    }

    template <class T>
    static inline T atomic_load(const volatile atomic<T>* p) noexcept
    {
        return p->load();
    }
    template <class T>
    static inline T atomic_load(const atomic<T>* p) noexcept
    {
        return p->load();
    }

    template <class T>
    static inline T atomic_load_explicit(const volatile atomic<T>* p, memory_order order) noexcept
    {
        return p->load( order );
    }
    template <class T>
    static inline T atomic_load_explicit(const atomic<T>* p, memory_order order) noexcept
    {
        return p->load( order );
    }

    template <class T>
    static inline T atomic_exchange(volatile atomic<T>* p, T val) noexcept
    {
        return p->exchange( val );
    }
    template <class T>
    static inline T atomic_exchange(atomic<T>* p, T val ) noexcept
    {
        return p->exchange( val );
    }

    template <class T>
    static inline T atomic_exchange_explicit(volatile atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->exchange( val, order );
    }
    template <class T>
    static inline T atomic_exchange_explicit(atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->exchange( val, order );
    }

    template <class T>
    static inline bool atomic_compare_exchange_weak(volatile atomic<T>* p, T* expected, T desired) noexcept
    {
        return p->compare_exchange_weak( *expected, desired );
    }
    template <class T>
    static inline bool atomic_compare_exchange_weak(atomic<T>* p, T* expected, T desired) noexcept
    {
        return p->compare_exchange_weak( *expected, desired );
    }

    template <class T>
    static inline bool atomic_compare_exchange_strong(volatile atomic<T>* p, T* expected, T desired) noexcept
    {
        return p->compare_exchange_strong( *expected, desired );
    }
    template <class T>
    static inline bool atomic_compare_exchange_strong(atomic<T>* p, T* expected, T desired) noexcept
    {
        return p->compare_exchange_strong( *expected, desired );
    }

    template <class T>
    static inline bool atomic_compare_exchange_weak_explicit(volatile atomic<T>* p, T* expected, T desired, memory_order success_order, memory_order failure_order) noexcept
    {
        return p->compare_exchange_weak( *expected, desired, success_order, failure_order );
    }
    template <class T>
    static inline bool atomic_compare_exchange_weak_explicit(atomic<T>* p, T* expected, T desired, memory_order success_order, memory_order failure_order) noexcept
    {
        return p->compare_exchange_weak( *expected, desired, success_order, failure_order );
    }

    template <class T>
    static inline bool atomic_compare_exchange_strong_explicit(volatile atomic<T>* p, T* expected, T desired, memory_order success_order, memory_order failure_order) noexcept
    {
        return p->compare_exchange_strong( *expected, desired, success_order, failure_order );
    }
    template <class T>
    static inline bool atomic_compare_exchange_strong_explicit(atomic<T>* p, T* expected, T desired, memory_order success_order, memory_order failure_order) noexcept
    {
        return p->compare_exchange_strong( *expected, desired, success_order, failure_order );
    }

    template <class T>
    static inline T atomic_fetch_add(volatile atomic<T>* p, T val) noexcept
    {
        return p->fetch_add( val );
    }
    template <class T>
    static inline T atomic_fetch_add(atomic<T>* p, T val) noexcept
    {
        return p->fetch_add( val );
    }
    template <class T>
    static inline T * atomic_fetch_add(volatile atomic<T *>* p, ptrdiff_t offset) noexcept
    {
        return p->fetch_add( offset );
    }
    template <class T>
    static inline T * atomic_fetch_add(atomic<T *>* p, ptrdiff_t offset) noexcept
    {
        return p->fetch_add( offset );
    }

    template <class T>
    static inline T atomic_fetch_add_explicit(volatile atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_add( val, order );
    }
    template <class T>
    static inline T atomic_fetch_add_explicit(atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_add( val, order );
    }
    template <class T>
    static inline T * atomic_fetch_add_explicit(volatile atomic<T *>* p, ptrdiff_t offset, memory_order order) noexcept
    {
        return p->fetch_add( offset, order );
    }
    template <class T>
    static inline T * atomic_fetch_add_explicit(atomic<T *>* p, ptrdiff_t offset, memory_order order) noexcept
    {
        return p->fetch_add( offset, order );
    }

    template <class T>
    static inline T atomic_fetch_sub(volatile atomic<T>* p, T val) noexcept
    {
        return p->fetch_sub( val );
    }
    template <class T>
    static inline T atomic_fetch_sub(atomic<T>* p, T val) noexcept
    {
        return p->fetch_sub( val );
    }
    template <class T>
    static inline T * atomic_fetch_sub(volatile atomic<T *>* p, ptrdiff_t offset) noexcept
    {
        return p->fetch_sub( offset );
    }
    template <class T>
    static inline T * atomic_fetch_sub(atomic<T *>* p, ptrdiff_t offset) noexcept
    {
        return p->fetch_sub( offset );
    }

    template <class T>
    static inline T atomic_fetch_sub_explicit(volatile atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_sub( val, order );
    }
    template <class T>
    static inline T atomic_fetch_sub_explicit(atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_sub( val, order );
    }
    template <class T>
    static inline T * atomic_fetch_sub_explicit(volatile atomic<T *>* p, ptrdiff_t offset, memory_order order) noexcept
    {
        return p->fetch_sub( offset, order );
    }
    template <class T>
    static inline T * atomic_fetch_sub_explicit(atomic<T *>* p, ptrdiff_t offset, memory_order order) noexcept
    {
        return p->fetch_sub( offset, order );
    }

    template <class T>
    static inline T atomic_fetch_and(volatile atomic<T>* p, T val) noexcept
    {
        return p->fetch_and( val );
    }
    template <class T>
    static inline T atomic_fetch_and(atomic<T>* p, T val) noexcept
    {
        return p->fetch_and( val );
    }

    template <class T>
    static inline T atomic_fetch_and_explicit(volatile atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_and( val, order );
    }
    template <class T>
    static inline T atomic_fetch_and_explicit(atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_and( val, order );
    }

    template <class T>
    static inline T atomic_fetch_or(volatile atomic<T>* p, T val) noexcept
    {
        return p->fetch_or( val );
    }
    template <class T>
    static inline T atomic_fetch_or(atomic<T>* p, T val) noexcept
    {
        return p->fetch_or( val );
    }

    template <class T>
    static inline T atomic_fetch_or_explicit(volatile atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_or( val, order );
    }
    template <class T>
    static inline T atomic_fetch_or_explicit(atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_or( val, order );
    }

    template <class T>
    static inline T atomic_fetch_xor(volatile atomic<T>* p, T val) noexcept
    {
        return p->fetch_xor( val );
    }
    template <class T>
    static inline T atomic_fetch_xor(atomic<T>* p, T val) noexcept
    {
        return p->fetch_xor( val );
    }

    template <class T>
    static inline T atomic_fetch_xor_explicit(volatile atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_xor( val, order );
    }
    template <class T>
    static inline T atomic_fetch_xor_explicit(atomic<T>* p, T val, memory_order order) noexcept
    {
        return p->fetch_xor( val, order );
    }

    // Atomic flag type
    typedef struct atomic_flag
    {
        void clear( memory_order order = memory_order_seq_cst ) volatile noexcept
        {
            assert( order != memory_order_acquire
                && order != memory_order_acq_rel
                && order != memory_order_consume
                );
            platform::atomic_flag_clear( &m_Flag, order );
        }
        void clear( memory_order order = memory_order_seq_cst ) noexcept
        {
            assert( order != memory_order_acquire
                && order != memory_order_acq_rel
                && order != memory_order_consume
                );
            platform::atomic_flag_clear( &m_Flag, order );
        }

        bool test_and_set( memory_order order = memory_order_seq_cst ) volatile noexcept
        {
            return platform::atomic_flag_tas( &m_Flag, order );
        }
        bool test_and_set( memory_order order = memory_order_seq_cst ) noexcept
        {
            return platform::atomic_flag_tas( &m_Flag, order );
        }

        atomic_flag() = default;

        atomic_flag(const atomic_flag&) = delete;
        atomic_flag& operator=(const atomic_flag&) = delete;
        atomic_flag& operator=(const atomic_flag&) volatile = delete;

        platform::atomic_flag_type volatile m_Flag;
    } atomic_flag;

    static inline bool atomic_flag_test_and_set(volatile atomic_flag* p) noexcept
    {
        return p->test_and_set();
    }
    static inline bool atomic_flag_test_and_set(atomic_flag * p) noexcept
    {
        return p->test_and_set();
    }
    static inline bool atomic_flag_test_and_set_explicit(volatile atomic_flag* p, memory_order order) noexcept
    {
        return p->test_and_set( order );
    }
    static inline bool atomic_flag_test_and_set_explicit(atomic_flag* p, memory_order order) noexcept
    {
        return p->test_and_set( order );
    }
    static inline void atomic_flag_clear(volatile atomic_flag* p) noexcept
    {
        return p->clear();
    }
    static inline void atomic_flag_clear(atomic_flag* p) noexcept
    {
        return p->clear();
    }
    static inline void atomic_flag_clear_explicit(volatile atomic_flag* p, memory_order order) noexcept
    {
        return p->clear( order );
    }
    static inline void atomic_flag_clear_explicit(atomic_flag* p, memory_order order) noexcept
    {
        return p->clear( order );
    }

    // Fences
    static inline void atomic_thread_fence(memory_order order) noexcept
    {
        platform::thread_fence( order );
        CDS_COMPILER_RW_BARRIER;
    }
    static inline void atomic_signal_fence(memory_order order) noexcept
    {
        platform::signal_fence( order );
    }

}}  // namespace cds::cxx11_atomic

//@endcond
#endif // #ifndef CDSLIB_COMPILER_CXX11_ATOMIC_H
