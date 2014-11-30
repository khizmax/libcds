//$$CDS-header$$

#ifndef __CDS_DEFS_H
#define __CDS_DEFS_H

#include <stddef.h>
#include <assert.h>
#include <cstdint>
#include <exception>
#include <string>
#include <memory>

#include <cds/version.h>

/** \mainpage CDS: Concurrent Data Structures library

   This library is a collection of lock-free and lock-based fine-grained algorithms of data structures
   like maps, queues, list etc. The library contains implementation of well-known data structures
   and memory reclamation schemas for modern processor architectures.

   Supported processor architectures and operating systems (OS) are:
      - x86 [32bit] Linux, Windows, FreeBSD, MinGW
      - amd64 (x86-64) [64bit] Linux, Windows, FreeBSD, MinGW
      - ia64 (itanium) [64bit] Linux, HP-UX 11.23, HP-UX 11.31
      - sparc [64bit] Sun Solaris
      - Mac OS X amd64

   Supported compilers:
      - GCC 4.3+ - for the UNIX-like OSes
      - Clang 3.0+ - for Linux
      - MS Visual C++ 2008 and above - for MS Windows

   For each lock-free data structure the \p CDS library presents several implementation based on published papers. For
   example, there are several implementations of queue, each of them is divided by memory reclamation
   schema used. However, any implementation supports common interface for the type of data structure.

   To implement any lock-free data structure, two things are needed:
   - atomic operation library conforming with C++11 memory model. The <b>libcds</b> has such feature, see cds::cxx11_atomic namespace for
     details and compiler-specific information.
   - safe memory reclamation (SMR) or garbage collecting (GC) algorithm. The <b>libcds</b> has an implementation of several
     well-known SMR algos, see below.

   The main part of lock-free data structs is garbage collecting. The garbage collector (GC) solves the problem of safe
   memory reclamation that is one of the main problems for lock-free programming.
   The library contains the implementations of several light-weight \ref cds_garbage_collector "memory reclamation schemes":
   - M.Michael's Hazard Pointer - \p see cds::gc::HP, \p cds::gc::DHP for more explanation
   - User-space Read-Copy Update (RCU) - see cds::urcu namespace
   - there is cds::gc::nogc "GC" for containers that do not support item reclamation.

   Many GC requires a support from the thread. The library does not define the threading model you must use,
   it is developed to support various ones; about incorporating <b>cds</b> library to your threading model see cds::threading.

   The main namespace for the library is \ref cds.
   To see the full list of container's class go to <a href="modules.html">modules</a> tab.

   \par How to build

   The <b>cds</b> is mostly header-only library. Only small part of library related to GC core functionality
   must be compiled. The test projects depends on the following static library from \p boost:
   - <tt>boost_thread</tt>
   - <tt>boost_date_time</tt>

   \par Windows build

   Prerequisites: for building <b>cds</b> library and test suite you need:
    - <a href="http://www.activestate.com/activeperl/downloads">perl</a> installed; \p PATH environment variable
        should contain full path to Perl binary. Perl is used to generate large dictionary for testing purpose;
    - <a href="http://www.boost.org/">boost library</a> 1.51 and above. You should create environment variable
        \p BOOST_PATH containing full path to \p boost root directory (for example, <tt>C:\\libs\\boost_1_47_0</tt>).

   Open solution file <tt>cds\projects\vcX\cds.sln</tt> where vcX - version of
   Microsoft Visual C++ you use: vc9 for MS VC 2008, vc10 for MS VC 2010 and so on. The solution
   contains <tt>cds</tt> project and several test projects. Just build the library using solution.

   <b>Warning</b>: the solution depends on \p BOOST_PATH environment variable that specifies full path
   to \p boost library root directory. The test projects search \p boost libraries in:
   - for 32bit: \$(BOOST_PATH)/stage/lib, \$(BOOST_PATH)/stage32/lib, and \$(BOOST_PATH)/bin.
   - for 64bit: \$(BOOST_PATH)/stage64/lib and \$(BOOST_PATH)/bin.

   \par *NIX build

   For Unix-like systems GCC and Clang compilers are supported.
   Use GCC 4.3 (or above) compiler or Clang 3.0 or above to build <b>cds</b> library. The distributive contains
   makefile and <tt>build.sh</tt> script in <tt>build</tt> directory.
   The <tt>build/sample</tt> directory contains sample scripts for different operating systems and
   processor architectures.
   The <tt>build.sh</tt> script supports the following options:
   - <tt>-c toolset</tt> - Toolset name, possible values: <tt>gcc</tt> (default), <tt>clang</tt>
   - <tt>-x compiler</tt> - C++ compiler name (e.g. g++, g++-4.5 and so on)
   - <tt>-p arch</tt> - processor architecture; possible values for arch are: x86, amd64 (x86_64), sparc, ia64
   - <tt>-o OStype</tt> - OS family; possible values for OStype are: linux, sunos (solaris), hpux
   - <tt>-D define</tt> additional defines
   - <tt>-b bits</tt> - bits to build, accepts '64', '32'
   - <tt>-l "options"</tt> - extra linker options (in quotes)
   - <tt>-z "options"</tt> - extra compiler options (in quotes)
   - <tt>--with-boost path</tt> - path to boost include
   - <tt>--debug-cxx-options "options"</tt> - extra compiler options for debug target
   - <tt>--debug-ld-options "options"</tt> - extra linker options for debug target
   - <tt>--release-cxx-options "options"</tt> - extra compiler options for release target
   - <tt>--release-ld-options "optons"</tt> - extra linker options for release target
   - <tt>--clean</tt> - clean all before building
   - <tt>--debug-test</tt> - make unit test in debug mode; by defalt release unit test generated
   - <tt>--amd64-use-128bit</tt> - compile with supporting 128bit (16byte) CAS on amd64 (for am64 only)

    <b>Important for GCC compiler</b>: all your projects that use <b>libcds</b> must be compiled with <b>-fno-strict-aliasing</b>
    compiler flag. Also, the compiler option <tt>-std=c++0x</tt> is very useful.

   \anchor cds_how_to_use
   \par How to use

   To use \p cds lock-free containers based on garbage collectors (GC) provided by library
   your application must be linked with \p libcds.

   The main part of lock-free programming is garbage collecting for safe memory reclamation.
   The library provides several types of GC schemes. One of widely used and well-tested one is Hazard Pointer
   memory reclamation schema discovered by M. Micheal and implemented in the library as cds::gc::HP class.
   Usually, the application is based on only one type of GC.

   In the next example we mean that your application uses Hazard Pointer (cds::gc::HP) - based containers.

    First, in your code you should initialize \p cds library and a garbage collector in \p main function:
    \code
    #include <cds/init.h>       // for cds::Initialize and cds::Terminate
    #include <cds/gc/hp.h>      // for cds::HP (Hazard Pointer) garbage collector

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
        if ( !cds::threading::Manager::isThreadAttached() )
            cds::threading::Manager::attachThread();

        // Do some work with HP-related containers
        ...
    }
    \endcode

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
#   define CDS_VERIFY( _expr )    assert( _expr )
#   define CDS_DEBUG_ONLY( _expr )        _expr
#else
#   define CDS_VERIFY( _expr )    _expr
#   define CDS_DEBUG_ONLY( _expr )
#endif

#ifdef CDS_STRICT
#   define CDS_STRICT_DO(_expr)         _expr
#else
#   define CDS_STRICT_DO( _expr )
#endif


// Compiler-specific defines
#include <cds/compiler/defs.h>

#define CDS_NOEXCEPT            CDS_NOEXCEPT_SUPPORT
#define CDS_NOEXCEPT_( expr )   CDS_NOEXCEPT_SUPPORT_( expr )

#ifdef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
#   define CDS_CXX11_INLINE_NAMESPACE   inline
#else
#   define CDS_CXX11_INLINE_NAMESPACE
#endif

//@cond
// typedefs for back compatibility
namespace cds {
    /// Atomic pointer
    typedef void *            pointer_t;

    /// 64bit unaligned int
    typedef int64_t     atomic64_unaligned;

    /// 64bit unaligned unsigned int
    typedef uint64_t  atomic64u_unaligned;

    /// 64bit aligned int
    typedef atomic64_unaligned CDS_TYPE_ALIGNMENT(8)    atomic64_aligned;

    /// 64bit aligned unsigned int
    typedef atomic64u_unaligned CDS_TYPE_ALIGNMENT(8)   atomic64u_aligned;

    /// 64bit atomic int (aligned)
    typedef atomic64_aligned    atomic64_t;

    /// 64bit atomic unsigned int (aligned)
    typedef atomic64u_aligned   atomic64u_t;

    /// 32bit atomic int
    typedef int32_t     atomic32_t;

    /// 32bit atomic unsigned int
    typedef uint32_t    atomic32u_t;

    /// atomic int
    typedef atomic32_t          atomic_t;

    /// atomic unsigned int
    typedef atomic32u_t         unsigned_atomic_t;

    /// atomic int sized as pointer
    typedef intptr_t ptr_atomic_t;

    /// atomic unsigned int sized as pointer
    typedef uintptr_t uptr_atomic_t;
} // namespace cds
//@endcond

/*************************************************************************
 Common things
**************************************************************************/

namespace cds {

    /// Base of all exceptions in the library
    class Exception: public std::exception
    {
    protected:
        std::string    m_strMsg    ;    ///< Exception message
    public:
        /// Create empty exception
        Exception()
        {}
        /// Create exception with message
        explicit Exception( const char * pszMsg )
            : m_strMsg( pszMsg )
        {}
        /// Create exception with message
        explicit Exception( const std::string& strMsg )
            :m_strMsg( strMsg )
        {}

        /// Destructor
        virtual ~Exception() throw()
        {}

        /// Return exception message
        virtual const char * what( ) const throw()
        {
            return m_strMsg.c_str();
        }
    };

//@cond
#   define CDS_PURE_VIRTUAL_FUNCTION_CALLED    { assert(false); throw Exception("Pure virtual function called"); }
#   define CDS_PURE_VIRTUAL_FUNCTION_CALLED_(method_name)    { assert(false); throw Exception("Pure virtual function called " method_name ); }
//@endcond

    /// any_type is used as a placeholder for auto-calculated type (usually in \p rebind templates)
    struct any_type {};

} // namespace cds


//@cond
#ifdef _DEBUG
#   define cds_assert(X)    assert(X)
#else
#   include <stdio.h>   // snprintf
    static inline void cds_assert_( bool bCond, char const * pszMsg, char const * pszFile, int nLine )
    {
        if ( !bCond ) {
            char buf[4096];
#   if CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CS_OSI_WINDOWS)
            _snprintf_s( buf, sizeof(buf)/sizeof(buf[0]), _TRUNCATE, pszMsg, pszFile, nLine );
#   else
            snprintf( buf, sizeof(buf)/sizeof(buf[0]), pszMsg, pszFile, nLine );
#   endif
            throw cds::Exception( buf );
        }
    }
#   define cds_assert(X)    cds_assert_( X, "%s (%d): Assert failed: " #X, __FILE__, __LINE__ );
#endif
//@endcond

#endif // #ifndef __CDS_DEFS_H
