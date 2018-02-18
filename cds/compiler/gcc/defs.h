// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_DEFS_H
#define CDSLIB_COMPILER_GCC_DEFS_H

// Compiler version
#define CDS_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if CDS_COMPILER_VERSION < 40800
#   error "Compiler version error. GCC version 4.8.0 and above is supported"
#endif

// Compiler name
#ifdef __VERSION__
#   define  CDS_COMPILER__NAME    ("GNU C++ " __VERSION__)
#else
#   define  CDS_COMPILER__NAME    "GNU C++"
#endif
#define  CDS_COMPILER__NICK        "gcc"

#if __cplusplus < CDS_CPLUSPLUS_11
#   error C++11 and above is required
#endif


#include <cds/compiler/gcc/compiler_macro.h>

#define alignof __alignof__

// ***************************************
// C++11 features

// C++11 thread_local keyword
#define CDS_CXX11_THREAD_LOCAL_SUPPORT

// *************************************************
// Features
// If you run under Thread Sanitizer, pass -DCDS_THREAD_SANITIZER_ENABLED in compiler command line
// UPD: Seems, GCC 5+ has predefined macro __SANITIZE_THREAD__, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64354
#if defined(__SANITIZE_THREAD__) && !defined(CDS_THREAD_SANITIZER_ENABLED)
#   define CDS_THREAD_SANITIZER_ENABLED
#endif

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))

// Attributes
#if CDS_COMPILER_VERSION >= 40900
#   if __cplusplus == CDS_CPLUSPLUS_11 // C++11
#       define CDS_DEPRECATED( reason ) [[gnu::deprecated(reason)]]
#   else  // C++14
#       define CDS_DEPRECATED( reason ) [[deprecated(reason)]]
#   endif
#else
    // GCC 4.8
#   define CDS_DEPRECATED( reason ) __attribute__((deprecated( reason )))
#endif

#define CDS_NORETURN __attribute__((__noreturn__))

// likely/unlikely

#define cds_likely( expr )   __builtin_expect( !!( expr ), 1 )
#define cds_unlikely( expr ) __builtin_expect( !!( expr ), 0 )

// Exceptions
#if defined( __EXCEPTIONS ) && __EXCEPTIONS == 1
#   define CDS_EXCEPTION_ENABLED
#endif

// double-width CAS support
// note: gcc-4.8 does not support double-word atomics
//       gcc-4.9: a lot of crashes when use DCAS
//       gcc-7: 128-bit atomic is not lock-free, see https://gcc.gnu.org/ml/gcc/2017-01/msg00167.html
// You can manually suppress wide-atomic support by defining in compiler command line:
//  for 64bit platform: -DCDS_DISABLE_128BIT_ATOMIC
//  for 32bit platform: -DCDS_DISABLE_64BIT_ATOMIC
#if CDS_COMPILER_VERSION >= 50000
#   if CDS_BUILD_BITS == 64
#       if !defined( CDS_DISABLE_128BIT_ATOMIC ) && defined( __GCC_HAVE_SYNC_COMPARE_AND_SWAP_16 ) && CDS_COMPILER_VERSION < 70000
#           define CDS_DCAS_SUPPORT
#       endif
#   else
#       if !defined( CDS_DISABLE_64BIT_ATOMIC ) && defined( __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8 )
#           define CDS_DCAS_SUPPORT
#       endif
#   endif
#endif

//if constexpr support (C++17)
#ifndef constexpr_if
#   if defined( __cpp_if_constexpr ) && __cpp_if_constexpr >= 201606
#       define constexpr_if if constexpr
#   endif
#endif


#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef CDSLIB_COMPILER_GCC_DEFS_H
