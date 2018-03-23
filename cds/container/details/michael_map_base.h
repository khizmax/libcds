// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_MAP_BASE_H
#define CDSLIB_CONTAINER_DETAILS_MICHAEL_MAP_BASE_H

#include <cds/container/details/michael_set_base.h>

namespace cds { namespace container {

    /// MichaelHashMap related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace michael_map {
        /// \p MichaelHashMap traits
        typedef container::michael_set::traits  traits;

        /// Metafunction converting option list to \p michael_map::traits
        template <typename... Options>
        using make_traits = cds::intrusive::michael_set::make_traits< Options... >;

        //@cond
        namespace details {
            using michael_set::details::init_hash_bitmask;
        }
        //@endcond

    }   // namespace michael_map

    //@cond
    // Forward declarations
    template <class GC, class OrderedList, class Traits = michael_map::traits>
    class MichaelHashMap;
    //@endcond

}}  // namespace cds::container


#endif  // ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_MAP_BASE_H
