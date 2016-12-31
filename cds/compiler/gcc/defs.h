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

#include <cds/compiler/gcc/compiler_macro.h>

#define alignof __alignof__

// ***************************************
// C++11 features

// C++11 inline namespace
#define CDS_CXX11_INLINE_NAMESPACE_SUPPORT

// constexpr
#define CDS_CONSTEXPR    constexpr

// noexcept
#define CDS_NOEXCEPT_SUPPORT        noexcept
#define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)

// C++11 thread_local keyword
#define CDS_CXX11_THREAD_LOCAL_SUPPORT

// Full SFINAE support
#define CDS_CXX11_SFINAE

// Inheriting constructors
#define CDS_CXX11_INHERITING_CTOR

// *************************************************
// Features
// If you run under Thread Sanitizer, pass -DCDS_THREAD_SANITIZER_ENABLED in compiler command line
//#define CDS_THREAD_SANITIZER_ENABLED

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))

// Attributes
#if CDS_COMPILER_VERSION >= 40900
#   if __cplusplus < 201103
#       define CDS_DEPRECATED( reason ) __attribute__((deprecated( reason )))
#   elif __cplusplus == 201103 // C++11
#       define CDS_DEPRECATED( reason ) [[gnu::deprecated(reason)]]
#   else  // C++14
#       define CDS_DEPRECATED( reason ) [[deprecated(reason)]]
#   endif
#else
#   define CDS_DEPRECATED( reason ) __attribute__((deprecated( reason )))
#endif

// likely/unlikely

#define cds_likely( expr )   __builtin_expect( !!( expr ), 1 )
#define cds_unlikely( expr ) __builtin_expect( !!( expr ), 0 )

// double-width CAS support
// note: gcc-4.8 does not support double-word atomics
//       gcc-4.9: a lot of crashes when use DCAS
#if CDS_COMPILER_VERSION >= 50000
#   if CDS_BUILD_BITS == 64
#       ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_16
#           define CDS_DCAS_SUPPORT
#       endif
#   else
#       ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8
#           define CDS_DCAS_SUPPORT
#       endif
#   endif
#endif

#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef CDSLIB_COMPILER_GCC_DEFS_H
