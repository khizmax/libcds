//$$CDS-header$$

#ifndef CDSLIB_OS_THREAD_H
#define CDSLIB_OS_THREAD_H

#include <thread>
#include <cds/details/defs.h>

#if CDS_OS_TYPE == CDS_OS_WIN32 || CDS_OS_TYPE == CDS_OS_WIN64 || CDS_OS_TYPE == CDS_OS_MINGW
#    include <cds/os/win/thread.h>
#else
#    include <cds/os/posix/thread.h>
#endif

namespace cds { namespace OS {

    /// Default backoff::yield implementation
    static inline void    backoff()
    {
        std::this_thread::yield();
    }
}} // namespace cds::OS

#endif    // #ifndef CDSLIB_OS_THREAD_H
