// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_THREAD_H
#define CDSLIB_OS_THREAD_H

#include <thread>
#include <cds/details/defs.h>

#if CDS_OS_TYPE == CDS_OS_WIN32 || CDS_OS_TYPE == CDS_OS_WIN64 || CDS_OS_TYPE == CDS_OS_MINGW
#    include <cds/os/win/thread.h>
#else
#    include <cds/os/posix/thread.h>
#endif

#if CDS_THREADING_HPX
#include <hpx/modules/threading.hpp>
#endif

namespace cds { namespace OS {

    /// Default backoff::yield implementation
    static inline void    backoff()
    {
#if CDS_THREADING_HPX
        hpx::this_thread::yield();
#else
        std::this_thread::yield();
#endif
    }
}} // namespace cds::OS

#endif    // #ifndef CDSLIB_OS_THREAD_H
