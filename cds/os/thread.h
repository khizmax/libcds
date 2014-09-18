//$$CDS-header$$

#ifndef __CDS_OS_THREAD_H
#define __CDS_OS_THREAD_H

#include <cds/details/defs.h>

#if CDS_OS_TYPE == CDS_OS_WIN32 || CDS_OS_TYPE == CDS_OS_WIN64 || CDS_OS_TYPE == CDS_OS_MINGW
#    include <cds/os/win/thread.h>
#else
#    include <cds/os/posix/thread.h>
#endif

#endif    // #ifndef __CDS_OS_THREAD_H
