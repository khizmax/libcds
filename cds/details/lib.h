// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_LIB_H
#define CDSLIB_LIB_H
//@cond

#include <cds/details/defs.h>

#ifndef CDS_BUILD_LIB

#ifdef _DEBUG
#   define  CDS_LIB_DEBUG_SUFFIX "-dbg"
#else
#   define  CDS_LIB_DEBUG_SUFFIX ""
#endif

#if CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL
#   pragma comment( lib, "libcds-" CDS_PROCESSOR__NICK CDS_LIB_DEBUG_SUFFIX )
#endif

#undef CDS_LIB_DEBUG_SUFFIX

#endif // #ifndef CDS_BUILD_LIB

//@endcond
#endif // #ifndef CDSLIB_LIB_H
