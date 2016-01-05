/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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
#   if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__)
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
#elif defined(__arm__) && __SIZEOF_POINTER__ == 4 && __ARM_ARCH >= 7
#    define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_ARM7
#    define CDS_BUILD_BITS        32
#    define CDS_PROCESSOR__NAME   "ARM v7"
#    define CDS_PROCESSOR__NICK   "arm7"
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
#   else
#       define CDS_EXPORT_API          __declspec(dllimport)
#   endif
#else
#   ifndef __declspec
#       define __declspec( _x )
#   endif
#endif

#if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#   define CDS_STDCALL __attribute__((stdcall))
#else
#   define CDS_STDCALL
#endif


#endif // #ifndef CDSLIB_COMPILER_GCC_COMPILER_MACRO_H
