//$$CDS-header$$

#ifndef __CDS_ALGO_ELIMINATION_OPT_H
#define __CDS_ALGO_ELIMINATION_OPT_H

#include <cds/details/defs.h>

namespace cds { namespace opt {

    /// Enable \ref cds_elimination_description "elimination back-off" for the container
    template <bool Enable>
    struct enable_elimination {
        //@cond
        template <class Base> struct pack: public Base
        {
            static CDS_CONSTEXPR_CONST bool enable_elimination = Enable;
        };
        //@endcond
    };

    /// \ref cds_elimination_description "Elimination back-off strategy" option setter
    /**
        Back-off strategy for elimination.
        Usually, elimination back-off strategy is \p cds::backoff::delay.
    */
    template <typename Type>
    struct elimination_backoff {
        //@cond
        template <class Base> struct pack: public Base
        {
            typedef Type elimination_backoff;
        };
        //@endcond
    };
}} // namespace cds::opt

#endif // #ifndef __CDS_ALGO_ELIMINATION_OPT_H
