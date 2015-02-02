//$$CDS-header$$

#ifndef CDSLIB_COMPILER_GCC_DEFS_H
#define CDSLIB_COMPILER_GCC_DEFS_H

// Compiler version
#define CDS_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if CDS_COMPILER_VERSION < 40800
#   error "Compiler version error. GCC version 4.8.0 and above is supported"
#endif

// Compiler name
#ifdef __VERSION__
#   define  CDS_COMPILER__NAME    ("GNU C++ " __VERSION__)
#else
#   define  CDS_COMPILER__NAME    "GNU C++"
#endif
#define  CDS_COMPILER__NICK        "gcc"

#include <cds/compiler/gcc/compiler_macro.h>

#define alignof __alignof__

// ***************************************
// C++11 features

// C++11 inline namespace
#define CDS_CXX11_INLINE_NAMESPACE_SUPPORT

// constexpr
#define CDS_CONSTEXPR    constexpr

// noexcept
#define CDS_NOEXCEPT_SUPPORT        noexcept
#define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)

// C++11 thread_local keyword
#define CDS_CXX11_THREAD_LOCAL_SUPPORT

// Full SFINAE support
#define CDS_CXX11_SFINAE

// Inheriting constructors
#define CDS_CXX11_INHERITING_CTOR

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))


#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef CDSLIB_COMPILER_GCC_DEFS_H
