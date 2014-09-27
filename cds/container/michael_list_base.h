//$$CDS-header$$

#ifndef __CDS_CONTAINER_MICHAEL_LIST_BASE_H
#define __CDS_CONTAINER_MICHAEL_LIST_BASE_H

#include <cds/container/details/base.h>
#include <cds/intrusive/details/michael_list_base.h>
#include <cds/urcu/options.h>

namespace cds { namespace container {

    /// MichaelList ordered list related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace michael_list {
        /// Michael list default type traits
        struct type_traits
        {
            typedef CDS_DEFAULT_ALLOCATOR   allocator       ;   ///< allocator used to allocate new node

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// specifies binary predicate used for key comparison.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// back-off strategy used
            /**
                If the option is not specified, the cds::backoff::empty is used.
            */
            typedef cds::backoff::empty             back_off;

            /// Item counter
            /**
                The type for item counting feature.
                Default is no item counter (\ref atomicity::empty_item_counter)
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// Link fields checking feature
            /**
                Default is \ref intrusive::opt::debug_check_link
            */
            static const opt::link_check_type link_checker = opt::debug_check_link;

            /// C++ memory ordering model
            /**
                List of available memory ordering see opt::memory_model
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

        /// Metafunction converting option list to MichaelList traits
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>

            See \ref MichaelList, \ref type_traits, \ref cds::opt::make_options.
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< type_traits, Options... >::type
                ,Options...
            >::type   type;
#endif
        };


    } // namespace michael_list

    // Forward declarations
    template <typename GC, typename T, typename Traits=michael_list::type_traits>
    class MichaelList;

    template <typename GC, typename Key, typename Value, typename Traits=michael_list::type_traits>
    class MichaelKVList;

    // Tag for selecting Michael's list implementation
    /**
        This struct is empty and it is used only as a tag for selecting MichaelList
        as ordered list implementation in declaration of some classes.

        See split_list::type_traits::ordered_list as an example.
    */
    struct michael_list_tag
    {};

}}  // namespace cds::container


#endif  // #ifndef __CDS_CONTAINER_MICHAEL_LIST_BASE_H
