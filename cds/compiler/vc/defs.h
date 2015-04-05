//$$CDS-header$$

#ifndef CDSLIB_COMPILER_VC_DEFS_H
#define CDSLIB_COMPILER_VC_DEFS_H
//@cond

// Compiler version
#define CDS_COMPILER_VERSION    _MSC_VER

// Compiler name
// Supported compilers: MS VC 2013
// C++ compiler versions:
#define CDS_COMPILER_MSVC12 1800    // 2013 vc12
#define CDS_COMPILER_MSVC14 1900    // 2015 vc14

#if CDS_COMPILER_VERSION < CDS_COMPILER_MSVC12
#   error "Only MS Visual C++ 12 (2013) Update 4 and above is supported"
#endif

#if _MSC_VER == 1800
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2013"
#   define  CDS_COMPILER__NICK  "vc12"
#elif _MSC_VER == 1900
#   define  CDS_COMPILER__NAME  "MS Visual C++ 2015"
#   define  CDS_COMPILER__NICK  "vc14"
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

#define CDS_TYPE_ALIGNMENT(n)     __declspec( align(n) )
#define CDS_DATA_ALIGNMENT(n)     __declspec( align(n) )
#define CDS_CLASS_ALIGNMENT(n)    __declspec( align(n) )

#include <cds/compiler/vc/compiler_barriers.h>

//@endcond
#endif // #ifndef CDSLIB_COMPILER_VC_DEFS_H
