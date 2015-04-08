//$$CDS-header$$

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
