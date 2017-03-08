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

#ifndef CDSLIB_COMPILER_VC_DEFS_H
#define CDSLIB_COMPILER_VC_DEFS_H
//@cond

// Compiler version
#define CDS_COMPILER_VERSION    _MSC_VER

// Compiler name
// Supported compilers: MS VC 2013 +
// C++ compiler versions:
#define CDS_COMPILER_MSVC12     1800    // 2013 vc12
#define CDS_COMPILER_MSVC14     1900    // 2015 vc14
#define CDS_COMPILER_MSVC14_1   1910    // 2017 vc14.1

#if CDS_COMPILER_VERSION < CDS_COMPILER_MSVC12
#   error "Only MS Visual C++ 12 (2013) Update 4 and above is supported"
#endif

#if _MSC_VER == 1800
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2013"
#   define  CDS_COMPILER__NICK  "vc12"
#   define  CDS_COMPILER_LIBCDS_SUFFIX "vc12"
#elif _MSC_VER == 1900
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2015"
#   define  CDS_COMPILER__NICK  "vc14"
#   define  CDS_COMPILER_LIBCDS_SUFFIX "vcv140"
#elif _MSC_VER == 1910
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

#ifdef CDS_BUILD_LIB
#   define CDS_EXPORT_API          __declspec(dllexport)
#else
#   define CDS_EXPORT_API          __declspec(dllimport)
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

// constexpr is not yet supported
#define CDS_CONSTEXPR

// noexcept - vc14 +
#if CDS_COMPILER_VERSION > CDS_COMPILER_MSVC12
#   define CDS_NOEXCEPT_SUPPORT        noexcept
#   define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)
#else
#   define CDS_NOEXCEPT_SUPPORT
#   define CDS_NOEXCEPT_SUPPORT_(expr)
#endif

// C++11 inline namespace
#if CDS_COMPILER_VERSION > CDS_COMPILER_MSVC12
#   define CDS_CXX11_INLINE_NAMESPACE_SUPPORT
#endif

#if CDS_COMPILER_VERSION == CDS_COMPILER_MSVC12
    // VC12: move ctor cannot be defaulted
    // Error: C2610 [move ctor] is not a special member function which can be defaulted
#   define CDS_DISABLE_DEFAULT_MOVE_CTOR
#endif

// Full SFINAE support
#if CDS_COMPILER_VERSION > CDS_COMPILER_MSVC12
#   define CDS_CXX11_SFINAE
#endif

// Inheriting constructors
#if CDS_COMPILER_VERSION > CDS_COMPILER_MSVC12
#   define CDS_CXX11_INHERITING_CTOR
#endif

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)     __declspec( align(n))
#define CDS_DATA_ALIGNMENT(n)     __declspec( align(n))
#define CDS_CLASS_ALIGNMENT(n)    __declspec( align(n))

// Attributes
#if CDS_COMPILER_VERSION >= CDS_COMPILER_MSVC14
#   define CDS_DEPRECATED( reason ) [[deprecated( reason )]]
#else
#   define CDS_DEPRECATED( reason ) __declspec(deprecated( reason ))
#endif

#define CDS_NORETURN __declspec(noreturn)

// Exceptions

#if defined( _CPPUNWIND )
#   define CDS_EXCEPTION_ENABLED
#endif


// double-width CAS support
//#define CDS_DCAS_SUPPORT

// Byte order
//  It seems, MSVC works only on little-endian architecture?..
#if !defined(CDS_ARCH_LITTLE_ENDIAN) && !defined(CDS_ARCH_BIG_ENDIAN)
#   define CDS_ARCH_LITTLE_ENDIAN
#endif

// Sanitizer attributes (not supported)
#define CDS_SUPPRESS_SANITIZE( ... )

#include <cds/compiler/vc/compiler_barriers.h>

//@endcond
#endif // #ifndef CDSLIB_COMPILER_VC_DEFS_H
