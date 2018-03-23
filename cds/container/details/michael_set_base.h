// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_SET_BASE_H
#define CDSLIB_CONTAINER_DETAILS_MICHAEL_SET_BASE_H

#include <cds/intrusive/details/michael_set_base.h>

namespace cds { namespace container {

    /// MichaelHashSet related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace michael_set {
        /// MichaelHashSet traits
        typedef cds::intrusive::michael_set::traits  traits;

        /// Metafunction converting option list to \p michael_set::traits
        template <typename... Options>
        using make_traits = cds::intrusive::michael_set::make_traits< Options... >;

        //@cond
        namespace details {
            using cds::intrusive::michael_set::details::init_hash_bitmask;
            using cds::intrusive::michael_set::details::list_iterator_selector;
            using cds::intrusive::michael_set::details::iterator;
        }
        //@endcond
    }

    //@cond
    // Forward declarations
    template <class GC, class OrderedList, class Traits = michael_set::traits>
    class MichaelHashSet;
    //@endcond

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_SET_BASE_H
