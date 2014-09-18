//$$CDS-header$$

#ifndef __CDS_DETAILS_STD_CHRONO_H
#define __CDS_DETAILS_STD_CHRONO_H

//@cond

#include <cds/details/defs.h>

#ifdef CDS_CXX11_STDLIB_CHRONO
#   include <chrono>
namespace cds_std {
    namespace chrono = std::chrono;
}
#else
#   include <boost/chrono.hpp>
namespace cds_std {
    namespace chrono = boost::chrono;
}
#endif

//@endcond

#endif // #ifndef __CDS_DETAILS_STD_CHRONO_H
