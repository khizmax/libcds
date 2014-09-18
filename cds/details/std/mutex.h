//$$CDS-header$$

#ifndef __CDS_DETAILS_STD_MUTEX_H
#define __CDS_DETAILS_STD_MUTEX_H

//@cond

#include <cds/details/defs.h>

#ifdef CDS_CXX11_STDLIB_MUTEX
#   include <mutex>
    namespace cds_std {
        using std::mutex;
        using std::recursive_mutex;
        using std::unique_lock;
        using std::lock_guard;
        using std::adopt_lock_t;
    }
#else
#   include <boost/thread/mutex.hpp>
#   include <boost/thread/recursive_mutex.hpp>
#   if BOOST_VERSION >= 105300
#       include <boost/thread/locks.hpp>
#       include <boost/thread/lock_guard.hpp>
#   else
#       include <boost/thread.hpp>
#   endif
    namespace cds_std {
        using boost::mutex;
        using boost::recursive_mutex;
        using boost::unique_lock;
        using boost::lock_guard;
        using boost::adopt_lock_t;
    }
#endif

//@endcond

#endif // #ifndef __CDS_DETAILS_STD_MUTEX_H
