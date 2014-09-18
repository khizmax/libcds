//$$CDS-header$$

#ifndef __CDS_COMPILER_VC_DEFS_H
#define __CDS_COMPILER_VC_DEFS_H
//@cond

// Compiler version
#define CDS_COMPILER_VERSION    _MSC_VER

// Compiler name
// Supported compilers: MS VC 2013
// C++ compiler versions:
#define CDS_COMPILER_MSVC12 1800    // 2013 vc12

#if CDS_COMPILER_VERSION < CDS_COMPILER_MSVC12
#   error "Only MS Visual C++ 12 (2013) and above is supported"
#endif

#if _MSC_VER == 1800
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
#define CDS_CXX11_ATOMIC_SUPPORT     1

// Lambda (VC 10 +)
//#define CDS_CXX11_LAMBDA_SUPPORT

// RValue (VC 10+)
#define CDS_RVALUE_SUPPORT
#define CDS_MOVE_SEMANTICS_SUPPORT

// Default template arguments for function templates (VC12+)
#define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT

// C++11 delete definition ( function declaration = delete)
#define CDS_CXX11_DELETE_DEFINITION_SUPPORT

// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT

// Variadic template support (VC12+)
#define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT     1

// C++11 template alias
#define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT

// C++11 inline namespace
//#define CDS_CXX11_INLINE_NAMESPACE_SUPPORT

// Explicit conversion operator (VC12+)
#define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT

// <cstdint>
#include <cstdint>

// Thread support library (thread, mutex, condition variable)
#define CDS_CXX11_STDLIB_THREAD
#define CDS_CXX11_STDLIB_MUTEX
#define CDS_CXX11_STDLIB_CONDITION_VARIABLE
#define CDS_CXX11_STDLIB_CHRONO

// Full SFINAE support
//#if CDS_COMPILER_VERSION >= ????
//#   define CDS_CXX11_SFINAE
//#endif


// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)     __declspec( align(n) )
#define CDS_DATA_ALIGNMENT(n)     __declspec( align(n) )
#define CDS_CLASS_ALIGNMENT(n)    __declspec( align(n) )

#include <cds/compiler/vc/compiler_barriers.h>

//@endcond
#endif // #ifndef __CDS_COMPILER_VC_DEFS_H
