// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_POSIX_THREAD_H
#define CDSLIB_OS_POSIX_THREAD_H

#include <signal.h>

#if CDS_THREADING_HPX
#include <hpx/modules/threading.hpp>
#else
#include <pthread.h>
#endif

namespace cds { namespace OS {
    /// posix-related wrappers
    inline namespace posix {

        /// Posix thread id type
#if CDS_THREADING_HPX
        typedef hpx::threads::thread_id ThreadId;
#else
        typedef std::thread::native_handle_type ThreadId;
#endif

        /// Get current thread id
        static inline ThreadId get_current_thread_id()
        {
#if CDS_THREADING_HPX
            return hpx::threads::get_self_id();
#else
            return pthread_self();
#endif
        }
    }    // namespace posix

    //@cond
#if CDS_THREADING_HPX
    constexpr const posix::ThreadId c_NullThreadId = hpx::threads::invalid_thread_id;
#else
    constexpr const posix::ThreadId c_NullThreadId = 0;
#endif
    //@endcond

}} // namespace cds::OS


#endif // #ifndef CDSLIB_OS_POSIX_THREAD_H
