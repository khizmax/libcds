// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_WIN_THREAD_H
#define CDSLIB_OS_WIN_THREAD_H

#ifndef NOMINMAX
#   define NOMINMAX
#endif
#if CDS_THREADING_HPX
#include <hpx/modules/threading.hpp>
#else
#include <windows.h>
#endif

namespace cds { namespace OS {
    /// Windows-specific functions
    inline namespace Win32 {

        /// OS-specific type of thread identifier
#if CDS_THREADING_HPX
        typedef hpx::threads::thread_id ThreadId;
#else
        typedef DWORD           ThreadId;
#endif

        /// Get current thread id
        static inline ThreadId get_current_thread_id()
        {
#if CDS_THREADING_HPX
            return hpx::threads::get_self_id();
#else
            return ::GetCurrentThreadId();
#endif
        }
    }    // namespace Win32

    //@cond
#if CDS_THREADING_HPX
    constexpr const Win32::ThreadId c_NullThreadId = hpx::threads::invalid_thread_id;
#else
    constexpr const Win32::ThreadId c_NullThreadId = 0;
#endif
    //@endcond

}} // namespace cds::OS

#endif // #ifndef CDSLIB_OS_WIN_THREAD_H
