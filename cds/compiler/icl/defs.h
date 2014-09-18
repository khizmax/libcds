//$$CDS-header$$

#ifndef __CDS_COMPILER_ICL_DEFS_H
#define __CDS_COMPILER_ICL_DEFS_H
//@cond

// Compiler version
#ifdef __ICL
#   define CDS_COMPILER_VERSION    __ICL
#else
#   define CDS_COMPILER_VERSION    __INTEL_COMPILER
#endif

// Compiler name
// Supported compilers: MS VC 2008, 2010, 2012
//
#   define  CDS_COMPILER__NAME  "Intel C++"
#   define  CDS_COMPILER__NICK  "icl"

// OS name
#if defined(_WIN64)
#   define CDS_OS_INTERFACE CDS_OSI_WINDOWS
#   define CDS_OS_TYPE      CDS_OS_WIN64
#   define CDS_OS__NAME     "Win64"
#   define CDS_OS__NICK     "Win64"
#elif defined(_WIN32)
#   define CDS_OS_INTERFACE CDS_OSI_WINDOWS
#   define CDS_OS_TYPE      CDS_OS_WIN32
#   define CDS_OS__NAME     "Win32"
#   define CDS_OS__NICK     "Win32"
#elif defined( __linux__ )
#   define CDS_OS_INTERFACE     CDS_OSI_UNIX
#   define CDS_OS_TYPE          CDS_OS_LINUX
#   define CDS_OS__NAME         "linux"
#   define CDS_OS__NICK         "linux"
#endif

// Processor architecture
#if defined(_M_X64) || defined(_M_AMD64)
#   define CDS_BUILD_BITS       64
#   define CDS_PROCESSOR_ARCH   CDS_PROCESSOR_AMD64
#   define CDS_PROCESSOR__NAME  "AMD64"
#   define CDS_PROCESSOR__NICK  "amd64"
#elif defined(_M_IX86)
#   define CDS_BUILD_BITS       32
#   define CDS_PROCESSOR_ARCH   CDS_PROCESSOR_X86
#   define CDS_PROCESSOR__NAME  "Intel x86"
#   define CDS_PROCESSOR__NICK  "x86"
#else
#   define CDS_BUILD_BITS        -1
#   define CDS_PROCESSOR_ARCH    CDS_PROCESSOR_UNKNOWN
#   define CDS_PROCESSOR__NAME    "<<Undefined>>"
#   error Intel C++ compiler is supported for x86 only
#endif

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#   define  __attribute__( _x )
#   define  CDS_STDCALL    __stdcall
#else
#   if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#       define CDS_STDCALL __attribute__((stdcall))
#   else
#       define CDS_STDCALL
#   endif
#endif

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#   ifdef CDS_BUILD_LIB
#       define CDS_EXPORT_API          __declspec(dllexport)
#   else
#       define CDS_EXPORT_API          __declspec(dllimport)
#   endif
#endif

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#   define alignof     __alignof
#else
#   define alignof __alignof__
#endif

//#if CDS_COMPILER_VERSION < 1600
//#   include <boost/static_assert.hpp>
//#   define static_assert(_expr, _msg)     BOOST_STATIC_ASSERT((_expr))
//#endif

// Memory leaks detection (debug build only)
//#ifdef _DEBUG
//#   define _CRTDBG_MAP_ALLOC
//#   define _CRTDBG_MAPALLOC
//#   include <stdlib.h>
//#   include <crtdbg.h>
//#   define CDS_MSVC_MEMORY_LEAKS_DETECTING_ENABLED
//#endif

#if CDS_COMPILER_VERSION < 1400
#   define CDS_CONSTEXPR
#   define CDS_CONSTEXPR_CONST const
#else
#   define CDS_CONSTEXPR    constexpr
#   define CDS_CONSTEXPR_CONST constexpr const
#endif

// noexcept is not yet supported
#if CDS_COMPILER_VERSION < 1400
#   define CDS_NOEXCEPT_SUPPORT
#   define CDS_NOEXCEPT_SUPPORT_(expr)
#else
#   define CDS_NOEXCEPT_SUPPORT        noexcept
#   define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)
#endif

// C++11 atomic support
#if CDS_COMPILER_VERSION >= 1300 && CDS_OS_INTERFACE == CDS_OSI_WINDOWS && _MSC_VER >= 1700
    // Intel C++ 13 supports C++11 atomic standard for VC++ 2012
#   define CDS_CXX11_ATOMIC_SUPPORT     1
#endif


// Lambda (ICL 12 +)
#if CDS_COMPILER_VERSION >= 1200
#   define CDS_CXX11_LAMBDA_SUPPORT
#endif

// RValue (ICL 10+)
#if CDS_COMPILER_VERSION >= 1200
#   define CDS_RVALUE_SUPPORT
#   define CDS_MOVE_SEMANTICS_SUPPORT
#endif

// Default template arguments for function templates (ICL 12.1+)
#if CDS_COMPILER_VERSION >= 1201
#   define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT
#endif

// C++11 delete definition ( function declaration = delete)
#if CDS_COMPILER_VERSION >= 1200
#   define CDS_CXX11_DELETE_DEFINITION_SUPPORT
#endif

// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#if CDS_COMPILER_VERSION >= 1200
#   define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#endif

// Variadic template support (ICL 12.1+)
#if CDS_COMPILER_VERSION >= 1201
#   define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT     1
#endif

// C++11 template alias
#if CDS_COMPILER_VERSION >= 1201
#   define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT
#endif

// C++11 inline namespace
#if CDS_COMPILER_VERSION >= 1400
#   define CDS_CXX11_INLINE_NAMESPACE_SUPPORT
#endif

// Explicit conversion operator
//#if CDS_COMPILER_VERSION >= 1800
//#   define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT
//#endif


// <cstdint>
// <cstdint>
#if _MSC_VER == 1500
// MS VC 2008 has no <cstdint>
#   include <cds/compiler/cstdint_boost.h>
#else
#   include <cds/compiler/cstdint_std.h>
#endif

// Thread support library (thread, mutex, condition variable)
#if _MSC_VER >= 1700
    // MS VC 11+
#   define CDS_CXX11_STDLIB_THREAD
#   define CDS_CXX11_STDLIB_MUTEX
#   define CDS_CXX11_STDLIB_CONDITION_VARIABLE
#   define CDS_CXX11_STDLIB_CHRONO
#endif

#if defined( CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT ) && defined(CDS_MOVE_SEMANTICS_SUPPORT) && CDS_COMPILER_VERSION < 1400
    // Intel C++ bug: move ctor & assignment operator cannot be defaulted
    // http://software.intel.com/en-us/forums/topic/394395
#   define CDS_DISABLE_DEFAULT_MOVE_CTOR
#endif


// *************************************************
// Alignment macro

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#   define CDS_TYPE_ALIGNMENT(n)     __declspec( align(n) )
#   define CDS_DATA_ALIGNMENT(n)     __declspec( align(n) )
#   define CDS_CLASS_ALIGNMENT(n)    __declspec( align(n) )
#else
#   define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#   define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#   define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#endif

#include <cds/compiler/icl/compiler_barriers.h>

//@endcond
#endif // #ifndef __CDS_COMPILER_VC_DEFS_H
