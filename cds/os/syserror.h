//$$CDS-header$$

#ifndef __CDS_OS_SYSERROR_H
#define __CDS_OS_SYSERROR_H

#include <cds/details/defs.h>

#if CDS_OS_TYPE == CDS_OS_WIN32 || CDS_OS_TYPE == CDS_OS_WIN64 || CDS_OS_TYPE == CDS_OS_MINGW
#    include <cds/os/win/syserror.h>
#else
#    include <cds/os/posix/syserror.h>
#endif

#endif    // #ifndef __CDS_OS_SYSERROR_H
