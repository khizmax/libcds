//$$CDS-header$$

#ifndef __CDS_DETAILS_STD_THREAD_H
#define __CDS_DETAILS_STD_THREAD_H

//@cond

#include <cds/details/defs.h>

#ifdef CDS_CXX11_STDLIB_THREAD
#   include <thread>
    namespace cds_std {
        using std::thread;
        namespace this_thread = std::this_thread;
    }
#else
#   include <boost/thread/thread.hpp>
    namespace cds_std {
        using boost::thread;
        namespace this_thread = boost::this_thread;
    }
#endif

//@endcond

#endif // #ifndef __CDS_DETAILS_STD_THREAD_H
