//$$CDS-header$$

//@cond
#ifndef __CDS_DETAILS_STD_TUPLE_H
#define __CDS_DETAILS_STD_TUPLE_H

#include <cds/details/defs.h>

#if (CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL) && _MSC_VER < 1600
// MS VC 2008
#include <boost/tuple/tuple.hpp>

namespace std {
    using boost::tuple;
    using boost::get;
    using boost::make_tuple;
    using boost::tie;

    template <class Tuple>
    struct tuple_size {
        static size_t const value = boost::tuples::length<Tuple>::value;
    };

}   // namespace std
#else
#   include <tuple>
#endif

#endif // #ifndef __CDS_DETAILS_STD_MEMORY_H
//@endcond
