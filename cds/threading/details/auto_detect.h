// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_AUTO_DETECT_H
#define CDSLIB_THREADING_AUTO_DETECT_H

#if defined(CDS_THREADING_AUTODETECT)
    // Auto-detect appropriate threading model
#   if CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)
        // For MSVC, CDS_THREADING_MSVC and CDS_THREADING_WIN_TLS is supported.
        // CDS_THREADING_MSVC must be explicitly defined if needed
#       if !defined(CDS_THREADING_MSVC) && !defined(CDS_THREADING_WIN_TLS) && !defined(CDS_THREADING_CXX11)
#           define CDS_THREADING_WIN_TLS
#       endif
#   elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG || CDS_COMPILER == CDS_COMPILER_INTEL
        // For GCC, CDS_THREADING_GCC and CDS_THREADING_PTHREAD is supported
        // CDS_THREADING_GCC must be explicitly defined if needed
#       if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#           if !defined(CDS_THREADING_GCC) && !defined(CDS_THREADING_WIN_TLS) && !defined(CDS_THREADING_CXX11)
#               define CDS_THREADING_WIN_TLS
#           endif
#       elif !defined(CDS_THREADING_GCC) && !defined(CDS_THREADING_PTHREAD) && !defined(CDS_THREADING_CXX11)
#           define CDS_THREADING_PTHREAD
#       endif
#   else
#       error "Unknown compiler for cds::threading"
#   endif
#endif // #if defined(CDS_THREADING_AUTODETECT)



#if defined(CDS_THREADING_MSVC)
#   include <cds/threading/details/msvc.h>
#elif defined(CDS_THREADING_WIN_TLS)
#   include <cds/threading/details/wintls.h>
#elif defined(CDS_THREADING_PTHREAD)
#   include <cds/threading/details/pthread.h>
#elif defined(CDS_THREADING_GCC)
#   include <cds/threading/details/gcc.h>
#elif defined(CDS_THREADING_CXX11)
#   include <cds/threading/details/cxx11.h>
#elif !defined(CDS_THREADING_USER_DEFINED)
#   error "You must define one of CDS_THREADING_xxx macro before compiling the application"
#endif

#endif // #ifndef CDSLIB_THREADING_AUTO_DETECT_H
