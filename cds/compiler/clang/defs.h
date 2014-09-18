//$$CDS-header$$

#ifndef __CDS_COMPILER_CLANG_DEFS_H
#define __CDS_COMPILER_CLANG_DEFS_H

// Compiler version
#define CDS_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)

// Compiler name
#define  CDS_COMPILER__NAME    ("clang " __clang_version__)
#define  CDS_COMPILER__NICK    "clang"

// C++11 atomic support - only for libc++
// Note: Clang libc++ atomic leads to program crash.
// So, we use libcds atomic implementation
//#if __has_feature(cxx_atomic) && defined(_LIBCPP_VERSION)
//#   define CDS_CXX11_ATOMIC_SUPPORT     1
//#endif


#include <cds/compiler/gcc/compiler_macro.h>


#define alignof __alignof__

// Variadic template support (only if -std=c++0x compile-line option provided)
#if __has_feature(cxx_variadic_templates)
#   define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
#endif

// Default template arguments for function templates
#if __has_feature(cxx_default_function_template_args)
#   define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT
#endif

#if __has_feature(cxx_deleted_functions)
// C++11 delete definition ( function declaration = delete)
#   define CDS_CXX11_DELETE_DEFINITION_SUPPORT
#endif

#if __has_feature(cxx_defaulted_functions)
// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#   define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#endif

// Explicit conversion operators
#if __has_feature(cxx_explicit_conversions)
#   define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT
#endif

// C++11 template alias
#if __has_feature(cxx_alias_templates)
#   define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT
#endif

// C++11 inline namespace
#if __has_feature(cxx_inline_namespaces)
#   define CDS_CXX11_INLINE_NAMESPACE_SUPPORT
#endif

// Lambda
#if __has_feature(cxx_lambdas)
#   define CDS_CXX11_LAMBDA_SUPPORT
#endif

// RValue
#if __has_feature(cxx_rvalue_references)
#   define CDS_RVALUE_SUPPORT
#   define CDS_MOVE_SEMANTICS_SUPPORT
#endif

#if __has_feature(cxx_constexpr)
#   define CDS_CONSTEXPR    constexpr
#   define CDS_CONSTEXPR_CONST constexpr const
#else
#   define CDS_CONSTEXPR
#   define CDS_CONSTEXPR_CONST const
#endif

#if __has_feature(cxx_noexcept)
#   define CDS_NOEXCEPT_SUPPORT        noexcept
#   define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)
#else
#   define CDS_NOEXCEPT_SUPPORT
#   define CDS_NOEXCEPT_SUPPORT_(expr)
#endif

// C++11 thread_local keyword
#if __has_feature(cxx_thread_local)         // CLang 3.3
#   define CDS_CXX11_THREAD_LOCAL_SUPPORT
#endif

// Thread support library (thread, mutex, condition variable, chrono)
#if CDS_COMPILER_VERSION >= 30100
#   if __has_include(<thread>)
#       define CDS_CXX11_STDLIB_THREAD
#   endif

#   if __has_include(<chrono>)
#       define CDS_CXX11_STDLIB_CHRONO
#   endif

#   if __has_include(<mutex>)
#       define CDS_CXX11_STDLIB_MUTEX
#   endif

#   if __has_include(<condition_variable>)
#       define CDS_CXX11_STDLIB_CONDITION_VARIABLE
#   endif
#endif

// Full SFINAE support
#define CDS_CXX11_SFINAE

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))


#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef __CDS_COMPILER_GCC_DEFS_H
