//$$CDS-header$$

#ifndef CDSLIB_ARH_COMPILER_DEFS_H
#define CDSLIB_ARH_COMPILER_DEFS_H

/*
    Required C++11 features:
    - move semantics [CDS_RVALUE_SUPPORT, CDS_MOVE_SEMANTICS_SUPPORT]
    - lambda function [CDS_CXX11_LAMBDA_SUPPORT]
    - variadic template [CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT]
    - template alias [CDS_CXX11_TEMPLATE_ALIAS_SUPPORT]
    - explicit conversion operator [CDS_CXX11_EXPLICIT_CONVERSION_OPERATOR_SUPPORT]
    - default template argument for function [CDS_CXX11_DEFAULT_FUNCTION_TEMPLATE_ARGS_SUPPORT]
    - explicit default functions (=default) [CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT]
    - =delete [CDS_CXX11_DELETE_DEFINITION_SUPPORT]
*/

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   include <cds/compiler/vc/defs.h>
#elif CDS_COMPILER == CDS_COMPILER_GCC
#   include <cds/compiler/gcc/defs.h>
#elif CDS_COMPILER == CDS_COMPILER_INTEL
#   include <cds/compiler/icl/defs.h>
#elif CDS_COMPILER == CDS_COMPILER_CLANG
#   include <cds/compiler/clang/defs.h>
#elif CDS_COMPILER == CDS_COMPILER_UNKNOWN
#   error Unknown compiler. Compilation aborted
#else
#   error Unknown value of CDS_COMPILER macro
#endif

#ifndef CDS_STDCALL
#   define CDS_STDCALL
#endif

#ifndef CDS_EXPORT_API
#   define CDS_EXPORT_API
#endif

#endif  // #ifndef CDSLIB_ARH_COMPILER_DEFS_H
