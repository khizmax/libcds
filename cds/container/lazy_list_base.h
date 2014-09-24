//$$CDS-header$$

#ifndef __CDS_CONTAINER_LAZY_LIST_BASE_H
#define __CDS_CONTAINER_LAZY_LIST_BASE_H

#include <cds/container/base.h>
#include <cds/intrusive/lazy_list_base.h>
#include <cds/urcu/options.h>

namespace cds { namespace container {

    /// LazyList ordered list related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace lazy_list {
        /// Lazy list default type traits
        /**
            Either \p compare or \p less or both must be specified.
        */
        struct type_traits
        {
            /// allocator used to allocate new node
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

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

            /// Lock type used to lock modifying items
            /**
                Default is cds::lock::Spin
            */
            typedef cds::lock::Spin                 lock_type;

            /// back-off strategy used
            /**
                If the option is not specified, the cds::backoff::Default is used.
            */
            typedef cds::backoff::Default           back_off;

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

            /// RCU deadlock checking policy (only for \ref cds_intrusive_LazyList_rcu "RCU-based LazyList")
            /**
                List of available options see opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            //@cond
            // LazyKVList: supporting for split-ordered list
            // key accessor (opt::none = internal key type is equal to user key type)
            typedef opt::none                       key_accessor;

            // for internal use only!!!
            typedef opt::none                       boundary_node_type;

            //@endcond
        };

        /// Metafunction converting option list to traits for LazyList
        /**
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>

            See \ref LazyList, \ref type_traits, \ref cds::opt::make_options.
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


    } // namespace lazy_list

    // Forward declarations
    template <typename GC, typename T, typename Traits=lazy_list::type_traits>
    class LazyList;

    template <typename GC, typename Key, typename Value, typename Traits=lazy_list::type_traits>
    class LazyKVList;

    // Tag for selecting lazy list implementation
    /**
        This struct is empty and it is used only as a tag for selecting LazyList
        as ordered list implementation in declaration of some classes.

        See split_list::type_traits::ordered_list as an example.
    */
    struct lazy_list_tag
    {};


}}  // namespace cds::container


#endif  // #ifndef __CDS_CONTAINER_LAZY_LIST_BASE_H
