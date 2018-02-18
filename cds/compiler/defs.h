// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_DEFS_H
#define CDSLIB_COMPILER_DEFS_H

// __cplusplus values
#define CDS_CPLUSPLUS_11     201103L
#define CDS_CPLUSPLUS_14     201402L
#define CDS_CPLUSPLUS_17     201703L

// VC 2017 is not full C++11-compatible yet
//#if __cplusplus < CDS_CPLUSPLUS_11
//#   error C++11 and above is required
//#endif


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

//if constexpr support (C++17)
#ifndef constexpr_if
#   define constexpr_if if
#endif

// Features
#include <cds/compiler/feature_tsan.h>

#endif  // #ifndef CDSLIB_COMPILER_DEFS_H
