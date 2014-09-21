//$$CDS-header$$

#ifndef __CDS_COMPILER_CLANG_DEFS_H
#define __CDS_COMPILER_CLANG_DEFS_H

// Compiler version
#define CDS_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)

// Compiler name
#define  CDS_COMPILER__NAME    ("clang " __clang_version__)
#define  CDS_COMPILER__NICK    "clang"

#if CDS_COMPILER_VERSION < 30300
#   error "Compiler version error. Clang version 3.3.0 and above is supported"
#endif


#if defined(_LIBCPP_VERSION) && !defined(CDS_USE_BOOST_ATOMIC)
    // Note: Clang libc++ atomic leads to program crash.
    // So, we use libcds atomic implementation
#   define CDS_USE_LIBCDS_ATOMIC
#endif

#include <cds/compiler/gcc/compiler_macro.h>

#define alignof __alignof__

// Variadic template support (only if -std=c++0x compile-line option provided)
#define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT

// Default template arguments for function templates
#define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT

// C++11 delete definition ( function declaration = delete)
#define CDS_CXX11_DELETE_DEFINITION_SUPPORT

// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT

// Explicit conversion operators
#define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT

// C++11 template alias
#define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT

// C++11 inline namespace
#define CDS_CXX11_INLINE_NAMESPACE_SUPPORT

// Lambda
#define CDS_CXX11_LAMBDA_SUPPORT

// RValue
#define CDS_RVALUE_SUPPORT
#define CDS_MOVE_SEMANTICS_SUPPORT

#define CDS_CONSTEXPR    constexpr
#define CDS_CONSTEXPR_CONST constexpr const

#define CDS_NOEXCEPT_SUPPORT        noexcept
#define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)

// C++11 thread_local keyword
#define CDS_CXX11_THREAD_LOCAL_SUPPORT

// Full SFINAE support
#define CDS_CXX11_SFINAE

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))


#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef __CDS_COMPILER_GCC_DEFS_H
