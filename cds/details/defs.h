// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DEFS_H
#define CDSLIB_DEFS_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <memory>

#include <cds/version.h>

/** \mainpage CDS: Concurrent Data Structures library

   This library is a collection of lock-free and lock-based fine-grained algorithms of data structures
   like maps, queues, list etc. The library contains implementation of well-known data structures
   and memory reclamation schemas for modern processor architectures. The library is written on C++11.

   The main namespace for the library is \ref cds.
   To see the full list of container's class go to <a href="modules.html">modules</a> tab.

   Supported processor architectures and operating systems (OS) are:
      - x86 [32bit] Linux, Windows, FreeBSD, MinGW
      - amd64 (x86-64) [64bit] Linux, Windows, FreeBSD, MinGW
      - ia64 (itanium) [64bit] Linux, HP-UX 11.23, HP-UX 11.31
      - sparc [64bit] Sun Solaris
      - Mac OS X amd64
      - ppc64 Linux

   Supported compilers:
      - GCC 4.8+
      - Clang 3.6+
      - MS Visual C++ 2015 and above
      - Intel C++ Compiler 15

   For each lock-free data structure the \p CDS library presents several implementation based on published papers. For
   example, there are several implementations of queue, each of them is divided by memory reclamation
   schema used. However, any implementation supports common interface for the type of data structure.

   To use any lock-free data structure, the following are needed:
   - atomic operation library conforming with C++11 memory model.
      The <b>libcds</b> can be built with \p std::atomic, \p boost::atomic or its own
      @ref cds_cxx11_atomic "atomic implementation"
   - safe memory reclamation (SMR) or garbage collecting (GC) algorithm.

   SMR is the main part of lock-free data structs. The SMR solves the problem of safe
   memory reclamation that is one of the main problem for lock-free programming.
   The library contains the implementations of several light-weight \ref cds_garbage_collector "memory reclamation schemes":
   - M.Michael's Hazard Pointer - see \p cds::gc::HP, \p cds::gc::DHP for more explanation
   - User-space Read-Copy Update (RCU) - see \p cds::urcu namespace
   - there is an empty \p cds::gc::nogc "GC" for append-only containers that do not support item reclamation.

   Many GC requires a support from the thread. The library does not define the threading model you must use,
   it is developed to support various ones; about incorporating <b>cds</b> library to your threading model see \p cds::threading.

   \anchor cds_how_to_use
   \par How to use

   The main part of lock-free programming is SMR, so-called garbage collector,  for safe memory reclamation.
   The library provides several types of SMR schemes. One of widely used and well-tested is Hazard Pointer
   memory reclamation schema discovered by M. Micheal and implemented in the library as \p cds::gc::HP class.
   Usually, the application is based on only one type of GC.

   In the next example we mean that you use Hazard Pointer \p cds::gc::HP - based containers.

    First, in your code you should initialize \p cds library and Hazard Pointer in \p main() function:
    \code
    #include <cds/init.h>       // for cds::Initialize and cds::Terminate
    #include <cds/gc/hp.h>      // for cds::HP (Hazard Pointer) SMR

    int main(int argc, char** argv)
    {
        // Initialize libcds
        cds::Initialize();

        {
            // Initialize Hazard Pointer singleton
            cds::gc::HP hpGC;

            // If main thread uses lock-free containers
            // the main thread should be attached to libcds infrastructure
            cds::threading::Manager::attachThread();

            // Now you can use HP-based containers in the main thread
            //...
        }

        // Terminate libcds
        cds::Terminate();
    }
    \endcode

    Second, any of your thread should be attached to \p cds infrastructure.
    \code
    #include <cds/gc/hp.h>

    int myThreadEntryPoint(void *)
    {
        // Attach the thread to libcds infrastructure
        cds::threading::Manager::attachThread();

        // Now you can use HP-based containers in the thread
        //...

        // Detach thread when terminating
        cds::threading::Manager::detachThread();
    }
    \endcode

    After that, you can use \p cds lock-free containers safely without any external synchronization.

    In some cases, you should work in an external thread. For example, your application
    is a plug-in for a server that calls your code in a thread that has been created by the server.
    In this case, you should use persistent mode of garbage collecting. In this mode, the thread attaches
    to the GC singleton only if it is not attached yet and never call detaching:
    \code
    #include <cds/gc/hp.h>

    int plugin_entry_point()
    {
        // Attach the thread if it is not attached yet
        if ( !cds::threading::Manager::isThreadAttached())
            cds::threading::Manager::attachThread();

        // Do some work with HP-related containers
        ...
    }
    \endcode


   \par How to build

   The <b>cds</b> is mostly header-only library. Only small part of library related to GC core functionality
   should be compiled. <b>cds</b> depends on C++ standard library only.

   Test suite depends on:
   - \p boost.thread (thread-loal storage support), boost.system
   - \p google-test

   Some parts of libcds may depend on DCAS (double-width compare-and-swap) atomic primitive if
   the target architecture supports it. For x86, cmake build script enables -mcx16 compiler flag that
   switches DCAS support on. You may manually disable DCAS support with the following  command line flags
   in GCC/clang (for MS VC++ compiler DCAS is not supported):
   - \p -DCDS_DISABLE_128BIT_ATOMIC - for 64bit build
   - \p -DCDS_DISABLE_64BIT_ATOMIC - for 32bit build

   @warning All your projects AND libcds MUST be compiled with the same flags - either with DCAS support or without it.

   \par Windows build

   Prerequisites: for building <b>cds</b> library and test suite you need:
    - <a href="http://www.activestate.com/activeperl/downloads">perl</a> installed; \p PATH environment variable
        should contain full path to Perl binary. Perl is used to generate large dictionary for testing purpose;
    - <a href="http://www.boost.org/">boost library</a> 1.51 and above. You should create environment variable
        \p BOOST_PATH containing full path to \p boost root directory (for example, <tt>C:\\libs\\boost_1_57_0</tt>).

   Open solution file <tt>cds\projects\vc141\cds.sln</tt> with Microsoft VisualStudio 2017.
   The solution contains \p cds project and a lot of test projects. Just build the library using solution.

   <b>Warning</b>: the solution depends on \p BOOST_PATH environment variable that specifies full path
   to \p boost library root directory. The test projects search \p boost libraries in:
   - for 32bit: <tt>\$(BOOST_PATH)/stage/lib</tt>, <tt>\$(BOOST_PATH)/stage32/lib</tt>, and <tt>\$(BOOST_PATH)/bin</tt>.
   - for 64bit: <tt>\$(BOOST_PATH)/stage64/lib</tt> and <tt>\$(BOOST_PATH)/bin</tt>.

   If you use static libcds, you should compile your projects with <tt>CDS_BUILD_STATIC_LIB</tt> preprocessor 
   definition.

   All tests are based on googletest framework. The following environment variables specify
   where to find gtest include and library directories:
   - \p GTEST_ROOT - gtest root directory. <tt>\$(GTEST_ROOT)/include</tt> specifies full path to
        gtest include files;
   - \p GTEST_LIB64 - the path to 64bit gtest library dir;
   - \p GTEST_LIB32 - the path to 32bit gtest library dir.

   \par *NIX build

   For Unix-like systems GCC and Clang compilers are supported.
   Use GCC 4.8+ compiler or Clang 3.6+ to build <b>cds</b> library with CMake.
   See accompanying file <tt>/build/cmake/readme.md</tt> for more info.
*/


/// The main library namespace
namespace cds {}

/*
    \brief Basic typedefs and defines

    You do not need include this header directly. All library header files depends on defs.h and include it.

    Defines macros:

    CDS_COMPILER        Compiler:
                    - CDS_COMPILER_MSVC     Microsoft Visual C++
                    - CDS_COMPILER_GCC      GNU C++
                    - CDS_COMPILER_CLANG    clang
                    - CDS_COMPILER_UNKNOWN  unknown compiler

    CDS_COMPILER__NAME    Character compiler name

    CDS_COMPILER_VERSION    Compliler version (number)

    CDS_BUILD_BITS    Resulting binary code:
                    - 32        32bit
                    - 64        64bit
                    - -1        undefined

    CDS_POW2_BITS    CDS_BUILD_BITS == 2**CDS_POW2_BITS

    CDS_PROCESSOR_ARCH    The processor architecture:
                    - CDS_PROCESSOR_X86     Intel x86 (32bit)
                    - CDS_PROCESSOR_AMD64   Amd64, Intel x86-64 (64bit)
                    - CDS_PROCESSOR_IA64    Intel IA64 (Itanium)
                    - CDS_PROCESSOR_SPARC   Sparc
                    - CDS_PROCESSOR_PPC64   PowerPC64
                    - CDS_PROCESSOR_ARM7    ARM v7
                    - CDS_PROCESSOR_ARM8    ARM v8
                    - CDS_PROCESSOR_UNKNOWN undefined processor architecture

    CDS_PROCESSOR__NAME    The name (string) of processor architecture

    CDS_OS_TYPE        Operating system type:
                    - CDS_OS_UNKNOWN        unknown OS
                    - CDS_OS_PTHREAD        unknown OS with pthread
                    - CDS_OS_WIN32          Windows 32bit
                    - CDS_OS_WIN64          Windows 64bit
                    - CDS_OS_LINUX          Linux
                    - CDS_OS_SUN_SOLARIS    Sun Solaris
                    - CDS_OS_HPUX           HP-UX
                    - CDS_OS_AIX            IBM AIX
                    - CDS_OS_BSD            FreeBSD, OpenBSD, NetBSD - common flag
                    - CDS_OS_FREE_BSD       FreeBSD
                    - CDS_OS_OPEN_BSD       OpenBSD
                    - CSD_OS_NET_BSD        NetBSD
                    - CDS_OS_MINGW          MinGW
                    - CDS_OS_OSX            Apple OS X

    CDS_OS__NAME        The name (string) of operating system type

    CDS_OS_INTERFACE OS interface:
                    - CDS_OSI_UNIX             Unix (POSIX)
                    - CDS_OSI_WINDOWS          Windows


    CDS_BUILD_TYPE    Build type: 'RELEASE' or 'DEBUG' string

*/

#if defined(_DEBUG) || !defined(NDEBUG)
#    define    CDS_DEBUG
#    define    CDS_BUILD_TYPE    "DEBUG"
#else
#    define    CDS_BUILD_TYPE    "RELEASE"
#endif

/// Unused function argument
#define CDS_UNUSED(x)   (void)(x)

// Supported compilers:
#define CDS_COMPILER_MSVC        1
#define CDS_COMPILER_GCC         2
#define CDS_COMPILER_INTEL       3
#define CDS_COMPILER_CLANG       4
#define CDS_COMPILER_UNKNOWN    -1

// Supported processor architectures:
#define CDS_PROCESSOR_X86       1
#define CDS_PROCESSOR_IA64      2
#define CDS_PROCESSOR_SPARC     3
#define CDS_PROCESSOR_AMD64     4
#define CDS_PROCESSOR_PPC64     5   // PowerPC 64bit
#define CDS_PROCESSOR_ARM7      7
#define CDS_PROCESSOR_ARM8      8
#define CDS_PROCESSOR_UNKNOWN   -1

// Supported OS interfaces
#define CDS_OSI_UNKNOWN          0
#define CDS_OSI_UNIX             1
#define CDS_OSI_WINDOWS          2

// Supported operating systems (value of CDS_OS_TYPE):
#define CDS_OS_UNKNOWN          -1
#define CDS_OS_WIN32            1
#define CDS_OS_WIN64            5
#define CDS_OS_LINUX            10
#define CDS_OS_SUN_SOLARIS      20
#define CDS_OS_HPUX             30
#define CDS_OS_AIX              50  // IBM AIX
#define CDS_OS_FREE_BSD         61
#define CDS_OS_OPEN_BSD         62
#define CDS_OS_NET_BSD          63
#define CDS_OS_MINGW            70
#define CDS_OS_OSX              80
#define CDS_OS_PTHREAD          100

#if defined(_MSC_VER)
#   if defined(__ICL) || defined(__INTEL_COMPILER)
#       define CDS_COMPILER CDS_COMPILER_INTEL
#   elif defined(__clang__)
#       define CDS_COMPILER CDS_COMPILER_CLANG
#   else
#       define CDS_COMPILER CDS_COMPILER_MSVC
#   endif
#elif defined(__clang__)    // Clang checking must be before GCC since Clang defines __GCC__ too
#   define CDS_COMPILER CDS_COMPILER_CLANG
#elif defined( __GCC__ ) || defined(__GNUC__)
#   if defined(__ICL) || defined(__INTEL_COMPILER)
#       define CDS_COMPILER CDS_COMPILER_INTEL
#   else
#       define CDS_COMPILER CDS_COMPILER_GCC
#   endif
#else
#    define CDS_COMPILER CDS_COMPILER_UNKNOWN
#endif  // Compiler choice


// CDS_VERIFY: Debug - assert(_expr); Release - _expr
#ifdef CDS_DEBUG
#   define CDS_VERIFY( _expr )       assert( _expr )
#   define CDS_VERIFY_FALSE( _expr ) assert( !( _expr ))
#   define CDS_DEBUG_ONLY( _expr )        _expr
#   define CDS_VERIFY_EQ( _expr, val )   assert( (_expr) == (val) )
#else
#   define CDS_VERIFY( _expr )    _expr
#   define CDS_VERIFY_FALSE( _expr ) _expr
#   define CDS_DEBUG_ONLY( _expr )
#   define CDS_VERIFY_EQ( _expr, val )   _expr
#endif

#ifdef CDS_STRICT
#   define CDS_STRICT_DO(_expr)         _expr
#else
#   define CDS_STRICT_DO( _expr )
#endif

#ifdef CDS_DEBUG
#   define cds_assert( expr )       assert( expr )
#else
    static inline void cds_assert( bool expr ) {
        if ( !expr )
            abort();
    }
#endif

// Compiler-specific defines
#include <cds/compiler/defs.h>

/*************************************************************************
 Common things
**************************************************************************/

namespace cds {

    /// any_type is used as a placeholder for auto-calculated type (usually in \p rebind templates)
    struct any_type {};

} // namespace cds

#endif // #ifndef CDSLIB_DEFS_H
