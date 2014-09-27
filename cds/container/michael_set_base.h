//$$CDS-header$$

#ifndef __CDS_CONTAINER_MICHAEL_SET_BASE_H
#define __CDS_CONTAINER_MICHAEL_SET_BASE_H

#include <cds/intrusive/details/michael_set_base.h>

namespace cds { namespace container {

    /// MichaelHashSet related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace michael_set {

        /// Type traits for MichaelHashSet class (typedef for cds::intrusive::michael_set::type_traits)
        typedef intrusive::michael_set::type_traits  type_traits;

        /// Metafunction converting option list to traits struct
        /**
            This is a synonym for intrusive::michael_set::make_traits
        */
        template <typename... Options>
        struct make_traits {
            typedef typename intrusive::michael_set::make_traits<Options...>::type type  ;   ///< Result of metafunction
        };

        //@cond
        namespace details {
            using intrusive::michael_set::details::init_hash_bitmask;
            using intrusive::michael_set::details::list_iterator_selector;
            using intrusive::michael_set::details::iterator;
        }
        //@endcond
    }

    //@cond
    // Forward declarations
    template <class GC, class OrderedList, class Traits = michael_set::type_traits>
    class MichaelHashSet;
    //@endcond

}} // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_MICHAEL_SET_BASE_H
