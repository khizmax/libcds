//$$CDS-header$$

#ifndef __CDS_CONTAINER_MICHAEL_MAP_BASE_H
#define __CDS_CONTAINER_MICHAEL_MAP_BASE_H

#include <cds/container/michael_set_base.h>

namespace cds { namespace container {

    /// MichaelHashMap related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace michael_map {
        /// Type traits for MichaelHashMap class
        typedef container::michael_set::type_traits  type_traits;

        using container::michael_set::make_traits;

        //@cond
        namespace details {
            using michael_set::details::init_hash_bitmask;
        }
        //@endcond

    }   // namespace michael_map

    //@cond
    // Forward declarations
    template <class GC, class OrderedList, class Traits = michael_map::type_traits>
    class MichaelHashMap;
    //@endcond

}}  // namespace cds::container


#endif  // ifndef __CDS_CONTAINER_MICHAEL_MAP_BASE_H
