//$$CDS-header$$

#ifndef __CDS_OS_POSIX_THREAD_H
#define __CDS_OS_POSIX_THREAD_H

#include <pthread.h>
#include <signal.h>

namespace cds { namespace OS {
    /// posix-related wrappers
    CDS_CXX11_INLINE_NAMESPACE namespace posix {

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

    constexpr const posix::ThreadId c_NullThreadId = 0;

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using posix::ThreadId;

    using posix::getCurrentThreadId;
    using posix::isThreadAlive;
#endif

}} // namespace cds::OS


#endif // #ifndef __CDS_OS_POSIX_THREAD_H
