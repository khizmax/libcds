//$$CDS-header$$

#ifndef __CDS_DETAILS_TRIVIAL_ASSIGN_H
#define __CDS_DETAILS_TRIVIAL_ASSIGN_H

#include <cds/details/defs.h>

//@cond
namespace cds { namespace details {

    template <typename Dest, typename Source>
    struct trivial_assign
    {
        Dest& operator()( Dest& dest, const Source& src )
        {
            return dest = src;
        }
    };
}}  // namespace cds::details
//@endcond

#endif // #ifndef __CDS_DETAILS_TRIVIAL_ASSIGN_H
