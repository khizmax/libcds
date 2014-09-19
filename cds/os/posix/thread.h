//$$CDS-header$$

#ifndef __CDS_OS_POSIX_THREAD_H
#define __CDS_OS_POSIX_THREAD_H

#include <stdlib.h>     // system
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <string.h>

namespace cds { namespace OS {
    /// posix-related wrappers
    namespace posix {
        /// Checks if thread \p id is alive
        static inline bool isThreadAlive( std::thread::id id )
        {
            // if sig is zero, error checking is performed but no signal is actually sent.
            // ESRCH - No thread could be found corresponding to that specified by the given thread ID
            // Unresolved problem: Linux may crash on dead thread_id.  Workaround unknown (except signal handler...)
            return pthread_kill( id, 0 ) != ESRCH;
        }

        /// Yield thread
        static inline void yield()      { sched_yield(); }

        /// Default back-off thread strategy (yield)
        static inline void backoff()    { sched_yield(); }

    }    // namespace posix

    using posix::isThreadAlive;
    using posix::yield;
    using posix::backoff;

}} // namespace cds::OS


#endif // #ifndef __CDS_OS_POSIX_THREAD_H
