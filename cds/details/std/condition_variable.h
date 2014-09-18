//$$CDS-header$$

#ifndef __CDS_DETAILS_STD_CONDITION_VARIABLE_H
#define __CDS_DETAILS_STD_CONDITION_VARIABLE_H

//@cond

#include <cds/details/defs.h>

#ifdef CDS_CXX11_STDLIB_CONDITION_VARIABLE
#   include <condition_variable>
    namespace cds_std {
        using std::condition_variable;
        using std::condition_variable_any;
    }
#else
#   include <boost/thread/condition_variable.hpp>
    namespace cds_std {
        using boost::condition_variable;
        using boost::condition_variable_any;
    }
#endif

//@endcond

#endif // #ifndef __CDS_DETAILS_STD_CONDITION_VARIABLE_H
