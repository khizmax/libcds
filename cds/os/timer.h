// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_TIMER_H
#define CDSLIB_OS_TIMER_H

#include <cds/details/defs.h>

#if CDS_OS_TYPE == CDS_OS_WIN32 || CDS_OS_TYPE == CDS_OS_WIN64 || CDS_OS_TYPE == CDS_OS_MINGW
#    include <cds/os/win/timer.h>
#elif CDS_OS_TYPE == CDS_OS_LINUX
#    include <cds/os/linux/timer.h>
#elif CDS_OS_TYPE == CDS_OS_SUN_SOLARIS
#    include <cds/os/sunos/timer.h>
#elif CDS_OS_TYPE == CDS_OS_HPUX
#    include <cds/os/hpux/timer.h>
#elif CDS_OS_TYPE == CDS_OS_AIX
#    include <cds/os/aix/timer.h>
#elif CDS_OS_TYPE == CDS_OS_FREE_BSD
#   include <cds/os/free_bsd/timer.h>
#elif CDS_OS_TYPE == CDS_OS_OSX
#   include <cds/os/osx/timer.h>
#elif CDS_OS_TYPE == CDS_OS_PTHREAD || CDS_OS_INTERFACE == CDS_OSI_UNIX
#   include <cds/os/posix/timer.h>
#else
//************************************************************************
// Other OSes
//************************************************************************
#    error Unknown operating system. Compilation aborted.
#endif

#endif    // #ifndef CDSLIB_OS_TIMER_H
