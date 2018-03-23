// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_BOUNDED_CONTAINER_H
#define CDSLIB_BOUNDED_CONTAINER_H

namespace cds {
    /// Bounded container
    /**
        If a container has upper limit of item then it should be based on bounded_container class.
        Example of those containers: cyclic queue (\p cds::container::VyukovMPMCCycleQueue)
    */
    struct bounded_container {};
}    // namespace cds

#endif    // CDSLIB_BOUNDED_CONTAINER_H
