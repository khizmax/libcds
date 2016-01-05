/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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
