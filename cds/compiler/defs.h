/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSLIB_COMPILER_DEFS_H
#define CDSLIB_COMPILER_DEFS_H

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

#ifndef CDS_EXPORT_API
#   define CDS_EXPORT_API
#endif

#ifndef cds_likely
#   define cds_likely( expr )   expr
#   define cds_unlikely( expr ) expr
#endif

#ifndef static_if
#   define static_if  if
#endif

// Features
#include <cds/compiler/feature_tsan.h>

#endif  // #ifndef CDSLIB_COMPILER_DEFS_H
