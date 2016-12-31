/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_OS_POSIX_THREAD_H
#define CDSLIB_OS_POSIX_THREAD_H

#include <pthread.h>
#include <signal.h>

namespace cds { namespace OS {
    /// posix-related wrappers
    inline namespace posix {

        /// Posix thread id type
        typedef std::thread::native_handle_type ThreadId;

        /// Get current thread id
        static inline ThreadId get_current_thread_id()
        {
            return pthread_self();
        }

        /// Checks if thread \p id is alive
        static inline bool is_thread_alive( ThreadId id )
        {
            // if sig is zero, error checking is performed but no signal is actually sent.
            // ESRCH - No thread could be found corresponding to that specified by the given thread ID
            // Unresolved problem: Linux may crash on dead thread_id. Workaround unknown (except signal handler...)
            return pthread_kill( id, 0 ) != ESRCH;
        }
    }    // namespace posix

    //@cond
    constexpr const posix::ThreadId c_NullThreadId = 0;
    //@endcond

}} // namespace cds::OS


#endif // #ifndef CDSLIB_OS_POSIX_THREAD_H
