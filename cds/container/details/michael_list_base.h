//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_LIST_BASE_H
#define CDSLIB_CONTAINER_DETAILS_MICHAEL_LIST_BASE_H

#include <cds/container/details/base.h>
#include <cds/intrusive/details/michael_list_base.h>
#include <cds/urcu/options.h>

namespace cds { namespace container {

    /// MichaelList ordered list related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace michael_list {
        /// MichaelList traits
        struct traits
        {
            typedef CDS_DEFAULT_ALLOCATOR   allocator       ;   ///< allocator used to allocate new node

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key comparison.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// Back-off strategy
            typedef cds::backoff::empty             back_off;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_MichaelList_rcu "RCU-based MichaelList")
            /**
                List of available options see opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            //@cond
            // MichaelKVList: supporting for split-ordered list
            // key accessor (opt::none = internal key type is equal to user key type)
            typedef opt::none                       key_accessor;
            //@endcond
        };

        /// Metafunction converting option list to \p michael_list::traits
        /**
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#endif
        };


    } // namespace michael_list

    // Forward declarations
    template <typename GC, typename T, typename Traits=michael_list::traits>
    class MichaelList;

    template <typename GC, typename Key, typename Value, typename Traits=michael_list::traits>
    class MichaelKVList;

    // Tag for selecting Michael's list implementation
    /**
        This struct is empty and it is used only as a tag for selecting MichaelList
        as ordered list implementation in declaration of some classes.

        See split_list::traits::ordered_list as an example.
    */
    struct michael_list_tag
    {};

}}  // namespace cds::container


#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MICHAEL_LIST_BASE_H
