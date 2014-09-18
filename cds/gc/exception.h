//$$CDS-header$$

#ifndef __CDS_GC_EXCEPTION_H
#define __CDS_GC_EXCEPTION_H

#include <cds/details/defs.h>

namespace cds { namespace gc {

    /// %Exception "Too few hazard pointers"
    CDS_DECLARE_EXCEPTION( too_few_hazard_pointers, "Too few hazard pointers" );

}} // namespace cds::gc

#endif // #ifndef __CDS_GC_EXCEPTION_H
