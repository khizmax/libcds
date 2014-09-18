//$$CDS-header$$

#ifndef __CDS_COMPILER_VC_DEFS_H
#define __CDS_COMPILER_VC_DEFS_H
//@cond

// Compiler version
#define CDS_COMPILER_VERSION    _MSC_VER

// Compiler name
// Supported compilers: MS VC 2008, 2010, 2012, 2013
// C++ compiler versions:
#define CDS_COMPILER_MSVC9  1500    // 2008 vc9
#define CDS_COMPILER_MSVC10 1600    // 2010 vc10
#define CDS_COMPILER_MSVC11 1700    // 2012 vc11
#define CDS_COMPILER_MSVC12 1800    // 2013 vc12

#if _MSC_VER == 1500
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2008"
#   define  CDS_COMPILER__NICK  "vc9"
#elif _MSC_VER == 1600
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2010"
#   define  CDS_COMPILER__NICK  "vc10"
#elif _MSC_VER == 1700
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2012"
#   define  CDS_COMPILER__NICK  "vc11"
#elif _MSC_VER == 1800
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2013"
#   define  CDS_COMPILER__NICK  "vc12"
#else
#   define  CDS_COMPILER__NAME  "MS Visual C++"
#   define  CDS_COMPILER__NICK  "msvc"
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

#define  CDS_STDCALL    __stdcall

#ifdef CDS_BUILD_LIB
#   define CDS_EXPORT_API          __declspec(dllexport)
#else
#   define CDS_EXPORT_API          __declspec(dllimport)
#endif

#define alignof     __alignof

#if CDS_COMPILER_VERSION < 1600
#   include <boost/static_assert.hpp>
#   define static_assert(_expr, _msg)     BOOST_STATIC_ASSERT((_expr))
#endif

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
#define CDS_CONSTEXPR_CONST const

// noexcept is not yet supported
//#define CDS_NOEXCEPT_SUPPORT        noexcept
//#define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)
#define CDS_NOEXCEPT_SUPPORT
#define CDS_NOEXCEPT_SUPPORT_(expr)

// C++11 atomic support
// MSVC 2012 has <atomic> implementation but all load/store is based on CAS
// that is quite inefficient.
// So for VC 2012 we use internal implementation for atomics
#if CDS_COMPILER_VERSION >= 1800
#   define CDS_CXX11_ATOMIC_SUPPORT     1
#endif


// Lambda (VC 10 +)
#if CDS_COMPILER_VERSION >= 1600
#   define CDS_CXX11_LAMBDA_SUPPORT
#   if CDS_COMPILER_VERSION < 1700
#       define CDS_BUG_STATIC_MEMBER_IN_LAMBDA
#   endif
#endif

// RValue (VC 10+)
#if CDS_COMPILER_VERSION >= 1600
#   define CDS_RVALUE_SUPPORT
#   define CDS_MOVE_SEMANTICS_SUPPORT
#endif

// Default template arguments for function templates (VC12+)
#if CDS_COMPILER_VERSION >= 1800
#   define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT
#endif

// C++11 delete definition ( function declaration = delete)
#if CDS_COMPILER_VERSION >= 1800
#   define CDS_CXX11_DELETE_DEFINITION_SUPPORT
#endif

// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#if CDS_COMPILER_VERSION >= 1800
#   define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#endif

// Variadic template support (VC12+)
#if CDS_COMPILER_VERSION >= 1800
#   define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT     1
#endif

// C++11 template alias
#if CDS_COMPILER_VERSION >= 1800
#   define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT
#endif

// C++11 inline namespace
//#define CDS_CXX11_INLINE_NAMESPACE_SUPPORT

// Explicit conversion operator (VC12+)
#if CDS_COMPILER_VERSION >= 1800
#   define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT
#endif


// <cstdint>
#if CDS_COMPILER_VERSION == 1500
    // MS VC 2008 has no <cstdint>
#   include <cds/compiler/cstdint_boost.h>
#else
#   include <cds/compiler/cstdint_std.h>
#endif

// Thread support library (thread, mutex, condition variable)
#if CDS_COMPILER_VERSION >= 1700
    // MS VC 11+
#   define CDS_CXX11_STDLIB_THREAD
#   define CDS_CXX11_STDLIB_MUTEX
#   define CDS_CXX11_STDLIB_CONDITION_VARIABLE
#   define CDS_CXX11_STDLIB_CHRONO
#endif

// Full SFINAE support
//#if CDS_COMPILER_VERSION >= ????
//#   define CDS_CXX11_SFINAE
//#endif


// *************************************************
// Alignment macro

// VC 2005 generates error C2719 "formal parameter with __declspec(align('#')) won't be aligned"
// for function's formal parameter with align declspec
#define CDS_TYPE_ALIGNMENT(n)     __declspec( align(n) )
#define CDS_DATA_ALIGNMENT(n)     __declspec( align(n) )
#define CDS_CLASS_ALIGNMENT(n)    __declspec( align(n) )

#include <cds/compiler/vc/compiler_barriers.h>

//@endcond
#endif // #ifndef __CDS_COMPILER_VC_DEFS_H
