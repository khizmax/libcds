//$$CDS-header$$

#ifndef __CDS_OS_POSIX_THREAD_H
#define __CDS_OS_POSIX_THREAD_H

#include <pthread.h>
#include <signal.h>

namespace cds { namespace OS {
    /// posix-related wrappers
    namespace posix {

        /// Posix thread id type
        typedef std::thread::native_handle_type ThreadId;

        /// Get current thread id
        static inline ThreadId getCurrentThreadId()
        {
            return pthread_self();
        }

        /// Checks if thread \p id is alive
        static inline bool isThreadAlive( ThreadId id )
        {
            // if sig is zero, error checking is performed but no signal is actually sent.
            // ESRCH - No thread could be found corresponding to that specified by the given thread ID
            // Unresolved problem: Linux may crash on dead thread_id. Workaround unknown (except signal handler...)
            return pthread_kill( id, 0 ) != ESRCH;
        }
    }    // namespace posix

    using posix::ThreadId;
    constexpr const ThreadId c_NullThreadId = 0;

    using posix::getCurrentThreadId;
    using posix::isThreadAlive;

}} // namespace cds::OS


#endif // #ifndef __CDS_OS_POSIX_THREAD_H
