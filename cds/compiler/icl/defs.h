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

#define CDS_CONSTEXPR    constexpr
#define CDS_CONSTEXPR_CONST constexpr const

#define CDS_NOEXCEPT_SUPPORT        noexcept
#define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)

// Lambda (ICL 12 +)
#define CDS_CXX11_LAMBDA_SUPPORT

// RValue (ICL 10+)
#define CDS_RVALUE_SUPPORT
#define CDS_MOVE_SEMANTICS_SUPPORT

// Default template arguments for function templates (ICL 12.1+)
#define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT

// C++11 delete definition ( function declaration = delete)
#define CDS_CXX11_DELETE_DEFINITION_SUPPORT

// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT

// C++11 inline namespace
#define CDS_CXX11_INLINE_NAMESPACE_SUPPORT

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
