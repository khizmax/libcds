// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_VC_DEFS_H
#define CDSLIB_COMPILER_VC_DEFS_H
//@cond

// Compiler version
#define CDS_COMPILER_VERSION    _MSC_VER

// Compiler name
// Supported compilers: MS VC 2015 +
// C++ compiler versions:
#define CDS_COMPILER_MSVC14         1900    // 2015 vc14
#define CDS_COMPILER_MSVC14_1       1910    // 2017 vc14.1
#define CDS_COMPILER_MSVC14_1_3     1911    // 2017 vc14.1 (VS 15.3)
#define CDS_COMPILER_MSVC14_1_5     1912    // 2017 vc14.1 (VS 15.5)
#define CDS_COMPILER_MSVC15         2000    // next Visual Studio

#if CDS_COMPILER_VERSION < CDS_COMPILER_MSVC14
#   error "Only MS Visual C++ 14 (2015) and above is supported"
#endif

#if _MSC_VER == 1900
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2015"
#   define  CDS_COMPILER__NICK  "vc14"
#   define  CDS_COMPILER_LIBCDS_SUFFIX "vcv140"
#elif _MSC_VER < 2000
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2017"
#   define  CDS_COMPILER__NICK  "vc141"
#   define  CDS_COMPILER_LIBCDS_SUFFIX "vcv141"
#else
#   define  CDS_COMPILER__NAME  "MS Visual C++"
#   define  CDS_COMPILER__NICK  "msvc"
#   define  CDS_COMPILER_LIBCDS_SUFFIX "vc"
#endif

// OS interface
#define CDS_OS_INTERFACE CDS_OSI_WINDOWS

// OS name
#if defined(_WIN64)
#   define CDS_OS_TYPE      CDS_OS_WIN64
#   define CDS_OS__NAME     "Win64"
#   define CDS_OS__NICK     "Win64"
#elif defined(_WIN32)
#   define CDS_OS_TYPE      CDS_OS_WIN32
#   define CDS_OS__NAME     "Win32"
#   define CDS_OS__NICK     "Win32"
#endif

// Processor architecture
#ifdef _M_IX86
#   define CDS_BUILD_BITS       32
#   define CDS_PROCESSOR_ARCH   CDS_PROCESSOR_X86
#   define CDS_PROCESSOR__NAME  "Intel x86"
#   define CDS_PROCESSOR__NICK  "x86"
#elif _M_X64
#   define CDS_BUILD_BITS       64
#   define CDS_PROCESSOR_ARCH   CDS_PROCESSOR_AMD64
#   define CDS_PROCESSOR__NAME  "AMD64"
#   define CDS_PROCESSOR__NICK  "amd64"
#else
#   define CDS_BUILD_BITS        -1
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_UNKNOWN
#   define CDS_PROCESSOR__NAME    "<<Undefined>>"
#   error Microsoft Visual C++ compiler is supported for x86 only
#endif

#define  __attribute__( _x )

#ifndef CDS_BUILD_STATIC_LIB
#   ifdef CDS_BUILD_LIB
#       define CDS_EXPORT_API          __declspec(dllexport)
#   else
#       define CDS_EXPORT_API          __declspec(dllimport)
#   endif
#else
#   define CDS_EXPORT_API
#endif

#define alignof     __alignof

// Memory leaks detection (debug build only)
#ifdef _DEBUG
#   define _CRTDBG_MAP_ALLOC
#   define _CRTDBG_MAPALLOC
#   include <stdlib.h>
#   include <crtdbg.h>
#   define CDS_MSVC_MEMORY_LEAKS_DETECTING_ENABLED
#endif

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)     __declspec( align(n))
#define CDS_DATA_ALIGNMENT(n)     __declspec( align(n))
#define CDS_CLASS_ALIGNMENT(n)    __declspec( align(n))

// Attributes
#define CDS_DEPRECATED( reason ) [[deprecated( reason )]]

#define CDS_NORETURN __declspec(noreturn)

// Exceptions
#if defined( _CPPUNWIND )
#   define CDS_EXCEPTION_ENABLED
#endif

    /*
        MSVC does not support inlining of thread_local static data members in class for DLL.
        Example:
        // .h
        class storage
        {
        public:
            record& get()
            {
                return tls_;
            }

        private:
            static thread_local record tls_;
        };

        // .cpp (DLL)
        thread_local record storage::tls_
    */
#define CDS_DISABLE_CLASS_TLS_INLINE

// double-width CAS support
//#define CDS_DCAS_SUPPORT

// Byte order
//  It seems, MSVC works only on little-endian architecture?..
#if !defined(CDS_ARCH_LITTLE_ENDIAN) && !defined(CDS_ARCH_BIG_ENDIAN)
#   define CDS_ARCH_LITTLE_ENDIAN
#endif

//if constexpr support (C++17)
#ifndef constexpr_if
    // Standard way to check if the compiler supports "if constexpr"
    // Of course, MS VC doesn't support any standard way
#   if defined __cpp_if_constexpr
#       if __cpp_if_constexpr >= 201606
#           define constexpr_if if constexpr
#       endif
#   elif CDS_COMPILER_VERSION >= CDS_COMPILER_MSVC14_1_3 && _MSVC_LANG > CDS_CPLUSPLUS_14
        // MS-specific WTF.
        // Don't work in /std:c++17 because /std:c++17 key defines _MSVC_LANG=201402 (c++14) in VC 15.3
#       define constexpr_if if constexpr
#   endif
#endif

// Sanitizer attributes (not supported)
#define CDS_SUPPRESS_SANITIZE( ... )

#include <cds/compiler/vc/compiler_barriers.h>

//@endcond
#endif // #ifndef CDSLIB_COMPILER_VC_DEFS_H
