//$$CDS-header$$

#ifndef __CDS_COMPILER_GCC_DEFS_H
#define __CDS_COMPILER_GCC_DEFS_H

// Compiler version
#define CDS_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if CDS_COMPILER_VERSION < 40300
#   error "Compiler version error. GCC version 4.3.0 and above is supported"
#endif

// Compiler name
#ifdef __VERSION__
#   define  CDS_COMPILER__NAME    ("GNU C++ " __VERSION__)
#else
#   define  CDS_COMPILER__NAME    "GNU C++"
#endif
#define  CDS_COMPILER__NICK        "gcc"

// C++11 atomic support
#if CDS_COMPILER_VERSION >= 40700
#   define CDS_CXX11_ATOMIC_SUPPORT     1
#elif defined(CDS_CXX11_ATOMIC_GCC)
#   define CDS_CXX11_ATOMIC_SUPPORT     1
#endif


#include <cds/compiler/gcc/compiler_macro.h>


#define alignof __alignof__

// ***************************************
// C++11 features

// Variadic template support (only if -std=c++0x compile-line option provided)
#if CDS_COMPILER_VERSION >= 40300
#   define CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
#endif

// RValue
#if CDS_COMPILER_VERSION >= 40300
#   define CDS_RVALUE_SUPPORT
#   define CDS_MOVE_SEMANTICS_SUPPORT
#endif

// Default template arguments for function templates
#if CDS_COMPILER_VERSION >= 40300
#   define CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT
#endif

// C++11 inline namespace
#if CDS_COMPILER_VERSION >= 40400
#   define CDS_CXX11_INLINE_NAMESPACE_SUPPORT
#endif

// Delete definition and explicitly-defaulted function
#if CDS_COMPILER_VERSION >= 40400
// C++11 delete definition ( function declaration = delete)
#   define CDS_CXX11_DELETE_DEFINITION_SUPPORT

// C++11 explicitly-defaulted function (= default) [std 8.4.2 [dcl.fct.def.default]]
#   define CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
#endif

// Lambda
#if CDS_COMPILER_VERSION >= 40500
#   define CDS_CXX11_LAMBDA_SUPPORT
#   if CDS_COMPILER_VERSION < 40800
#       define CDS_BUG_STATIC_MEMBER_IN_LAMBDA
#   endif
#endif

// Explicit conversion operator
#if CDS_COMPILER_VERSION >= 40500
#   define CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT
#endif

#if CDS_COMPILER_VERSION >= 40600
#   define CDS_CONSTEXPR    constexpr
#   define CDS_NOEXCEPT_SUPPORT        noexcept
#   define CDS_NOEXCEPT_SUPPORT_(expr) noexcept(expr)
#   if CDS_COMPILER_VERSION >= 40600 && CDS_COMPILER_VERSION < 40700
    // GCC 4.6.x does not allow noexcept specification in defaulted function
    // void foo() noexcept = default
    // error: function ‘foo’ defaulted on its first declaration must not have an exception-specification
#       define CDS_NOEXCEPT_DEFAULTED
#       define CDS_NOEXCEPT_DEFAULTED_(expr)
    // GCC 4.6.x: constexpr and const are incompatible in variable declaration
#       define CDS_CONSTEXPR_CONST const
#   else
#       define CDS_CONSTEXPR_CONST constexpr const
#   endif
#else
#   define CDS_CONSTEXPR
#   define CDS_CONSTEXPR_CONST const
#   define CDS_NOEXCEPT_SUPPORT
#   define CDS_NOEXCEPT_SUPPORT_(expr)
#endif

// C++11 template alias
#if CDS_COMPILER_VERSION >= 40700
#   define CDS_CXX11_TEMPLATE_ALIAS_SUPPORT
#endif

// C++11 thread_local keyword
#if CDS_COMPILER_VERSION >= 40800
#   define CDS_CXX11_THREAD_LOCAL_SUPPORT
#endif


#if defined( CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT ) && defined(CDS_MOVE_SEMANTICS_SUPPORT) && CDS_COMPILER_VERSION >= 40400 && CDS_COMPILER_VERSION < 40600
    // GCC 4.4 - 4.5 bug: move ctor & assignment operator cannot be defaulted
#   define CDS_DISABLE_DEFAULT_MOVE_CTOR
#endif

// Thread support library (thread, mutex, condition variable, chrono)
#if CDS_COMPILER_VERSION >= 40800
#   define CDS_CXX11_STDLIB_THREAD
#   define CDS_CXX11_STDLIB_MUTEX
#   define CDS_CXX11_STDLIB_CONDITION_VARIABLE
#   define CDS_CXX11_STDLIB_CHRONO
#endif

// Full SFINAE support
#if CDS_COMPILER_VERSION >= 40700
#   define CDS_CXX11_SFINAE
#endif

// *************************************************
// Alignment macro

#define CDS_TYPE_ALIGNMENT(n)   __attribute__ ((aligned (n)))
#define CDS_CLASS_ALIGNMENT(n)  __attribute__ ((aligned (n)))
#define CDS_DATA_ALIGNMENT(n)   __attribute__ ((aligned (n)))


#include <cds/compiler/gcc/compiler_barriers.h>

#endif // #ifndef __CDS_COMPILER_GCC_DEFS_H
