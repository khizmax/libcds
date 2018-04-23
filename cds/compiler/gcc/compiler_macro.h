// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_COMPILER_MACRO_H
#define CDSLIB_COMPILER_GCC_COMPILER_MACRO_H

// OS interface && OS name
#ifndef CDS_OS_TYPE
#   if defined( __linux__ )
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_LINUX
#       define CDS_OS__NAME         "linux"
#       define CDS_OS__NICK         "linux"
#   elif defined( __sun__ )
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_SUN_SOLARIS
#       define CDS_OS__NAME         "Sun Solaris"
#       define CDS_OS__NICK         "sun"
#   elif defined( __hpux__ )
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_HPUX
#       define CDS_OS__NAME         "HP-UX"
#       define CDS_OS__NICK         "hpux"
#   elif defined( _AIX )
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_AIX
#       define CDS_OS__NAME         "AIX"
#       define CDS_OS__NICK         "aix"
#   elif defined( __FreeBSD__ )
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_FREE_BSD
#       define CDS_OS__NAME         "FreeBSD"
#       define CDS_OS__NICK         "freebsd"
#   elif defined( __OpenBSD__ )
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_OPEN_BSD
#       define CDS_OS__NAME         "OpenBSD"
#       define CDS_OS__NICK         "openbsd"
#   elif defined( __NetBSD__ )
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_NET_BSD
#       define CDS_OS__NAME         "NetBSD"
#       define CDS_OS__NICK         "netbsd"
#   elif defined(__MINGW32__) || defined( __MINGW64__)
#       define CDS_OS_INTERFACE     CDS_OSI_WINDOWS
#       define CDS_OS_TYPE          CDS_OS_MINGW
#       define CDS_OS__NAME         "MinGW"
#       define CDS_OS__NICK         "mingw"
#   elif defined(__MACH__)
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_OSX
#       define CDS_OS__NAME         "OS X"
#       define CDS_OS__NICK         "osx"
#   else
#       define CDS_OS_INTERFACE     CDS_OSI_UNIX
#       define CDS_OS_TYPE          CDS_OS_PTHREAD
#       define CDS_OS__NAME         "pthread"
#       define CDS_OS__NICK         "pthread"
#   endif
#endif // #ifndef CDS_OS_TYPE

// Processor architecture

#if defined(__arm__) && !defined(__ARM_ARCH)
// GCC 4.6 does not defined __ARM_ARCH
#   if defined(__ARM_ARCH_8A__) || defined(__ARM_ARCH_8S__) || defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64)
#       define __ARM_ARCH   8
#   elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7S__)
#       define __ARM_ARCH   7
#   else
#       define __ARM_ARCH   5
#   endif
#endif

#if defined(__x86_64__) || defined(__amd64__) || defined(__amd64)
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_AMD64
#   define CDS_BUILD_BITS        64
#   define CDS_PROCESSOR__NAME   "Intel x86-64"
#   define CDS_PROCESSOR__NICK   "amd64"
#elif defined(__i386__)
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_X86
#   define CDS_BUILD_BITS        32
#   define CDS_PROCESSOR__NAME   "Intel x86"
#   define CDS_PROCESSOR__NICK   "x86"
#elif defined(sparc) || defined (__sparc__)
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_SPARC
#   define CDS_PROCESSOR__NAME   "Sparc"
#   define CDS_PROCESSOR__NICK   "sparc"
#   ifdef __arch64__
#       define CDS_BUILD_BITS   64
#    else
#       error Sparc 32bit is not supported
#    endif
#elif defined( __ia64__)
#    define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_IA64
#    define CDS_BUILD_BITS        64
#    define CDS_PROCESSOR__NAME   "Intel IA64"
#    define CDS_PROCESSOR__NICK   "ia64"
#elif defined(_ARCH_PPC64)
#    define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_PPC64
#    define CDS_BUILD_BITS        64
#    define CDS_PROCESSOR__NAME   "IBM PowerPC64"
#    define CDS_PROCESSOR__NICK   "ppc64"
#elif defined(__arm__) && __SIZEOF_POINTER__ == 4 && __ARM_ARCH >= 7 && __ARM_ARCH < 8
#    define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_ARM7
#    define CDS_BUILD_BITS        32
#    define CDS_PROCESSOR__NAME   "ARM v7"
#    define CDS_PROCESSOR__NICK   "arm7"
#elif ( defined(__arm__)  || defined(__aarch64__)) && __ARM_ARCH >= 8
#    define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_ARM8
#    define CDS_BUILD_BITS        64
#    define CDS_PROCESSOR__NAME   "ARM v8"
#    define CDS_PROCESSOR__NICK   "arm8"
#elif defined(__arm__)  || defined(__aarch64__)
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_ARM8
#   define CDS_PROCESSOR__NAME   "ARM"
#   define CDS_PROCESSOR__NICK   "arm"
#   if __SIZEOF_POINTER__ == 8
#       define CDS_BUILD_BITS        64
#   else
#       define CDS_BUILD_BITS        32
#   endif
#else
#   if defined(CDS_USE_LIBCDS_ATOMIC)
#       error "Libcds does not support atomic implementation for the processor architecture. Try to use C++11-compatible compiler and remove CDS_USE_LIBCDS_ATOMIC flag from compiler command line"
#   else
#       define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_UNKNOWN
#       define CDS_BUILD_BITS        32
#       define CDS_PROCESSOR__NAME   "unknown"
#       define CDS_PROCESSOR__NICK   "unknown"
#   endif
#endif

#if CDS_OS_TYPE == CDS_OS_MINGW
#   ifdef CDS_BUILD_LIB
#       define CDS_EXPORT_API          __declspec(dllexport)
#   elif !defined(CDS_BUILD_STATIC_LIB)
#       define CDS_EXPORT_API          __declspec(dllimport)
#   endif
#else
#   ifndef __declspec
#       define __declspec(_x)
#   endif
#endif

// Byte order
#if !defined(CDS_ARCH_LITTLE_ENDIAN) && !defined(CDS_ARCH_BIG_ENDIAN)
#   ifdef __BYTE_ORDER__
#       if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#           define CDS_ARCH_LITTLE_ENDIAN
#       elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#           define CDS_ARCH_BIG_ENDIAN
#       endif
#   else
#       warning "Undefined byte order for current architecture (no __BYTE_ORDER__ preprocessor definition)"
#   endif
#endif

// Sanitizer attributes
// Example: CDS_DISABLE_SANITIZE( "function" )
#ifdef CDS_ADDRESS_SANITIZER_ENABLED
#   define CDS_SUPPRESS_SANITIZE( ... ) __attribute__(( no_sanitize( __VA_ARGS__ )))
#else
#   define CDS_SUPPRESS_SANITIZE( ... )
#endif


#endif // #ifndef CDSLIB_COMPILER_GCC_COMPILER_MACRO_H
