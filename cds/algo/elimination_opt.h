// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_ALGO_ELIMINATION_OPT_H
#define CDSLIB_ALGO_ELIMINATION_OPT_H

#include <cds/details/defs.h>

namespace cds { namespace opt {

    /// Enable \ref cds_elimination_description "elimination back-off" for the container
    template <bool Enable>
    struct enable_elimination {
        //@cond
        template <class Base> struct pack: public Base
        {
            static constexpr const bool enable_elimination = Enable;
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

#endif // #ifndef CDSLIB_ALGO_ELIMINATION_OPT_H
