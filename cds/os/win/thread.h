// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_WIN_THREAD_H
#define CDSLIB_OS_WIN_THREAD_H

#ifndef NOMINMAX
#   define NOMINMAX
#endif
#include <windows.h>

namespace cds { namespace OS {
    /// Windows-specific functions
    inline namespace Win32 {

        /// OS-specific type of thread identifier
        typedef DWORD           ThreadId;

        /// Get current thread id
        static inline ThreadId get_current_thread_id()
        {
            return ::GetCurrentThreadId();
        }
    }    // namespace Win32

    //@cond
    constexpr const Win32::ThreadId c_NullThreadId = 0;
    //@endcond

}} // namespace cds::OS

#endif // #ifndef CDSLIB_OS_WIN_THREAD_H
