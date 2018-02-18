// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_LAZY_LIST_BASE_H
#define CDSLIB_CONTAINER_DETAILS_LAZY_LIST_BASE_H

#include <cds/container/details/base.h>
#include <cds/intrusive/details/lazy_list_base.h>
#include <cds/urcu/options.h>

namespace cds { namespace container {

    /// \p LazyList ordered list related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace lazy_list {

        /// \p LazyList internal statistics, see \p cds::intrusive::lazy_list::stat
        template <typename EventCounter = cds::intrusive::lazy_list::stat<>::event_counter>
        using stat = cds::intrusive::lazy_list::stat< EventCounter >;

        /// \p LazyList empty internal statistics, see \p cds::intrusive::lazy_list::empty_stat
        typedef cds::intrusive::lazy_list::empty_stat empty_stat;

        //@cond
        template <typename Stat = cds::intrusive::lazy_list::wrapped_stat<>::stat_type>
        using wrapped_stat = cds::intrusive::lazy_list::wrapped_stat< Stat >;
        //@endif

        /// LazyList traits
        /**
            Either \p compare or \p less or both must be specified.
        */
        struct traits
        {
            /// allocator used to allocate new node
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

            /// Key comparing functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key comparing
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// Specifies binary functor used for comparing keys for equality
            /**
                No default functor is provided. If \p equal_to option is not spcified, \p compare is used, if \p compare is not
                specified, \p less is used.
            */
            typedef opt::none                       equal_to;

            /// Specifies list ordering policy.
            /**
                If \p sort is \p true, than list maintains items in sorted order, otherwise items are unordered. Default is \p true.
                Note that if \p sort is \p false then lookup operations scan entire list.
            */
            static const bool sort = true;

            /// Lock type used to lock modifying items
            /**
                Default is cds::sync::spin
            */
            typedef cds::sync::spin                 lock_type;

            /// back-off strategy used
            typedef cds::backoff::Default           back_off;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter     item_counter;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p lazy_list::empty_stat).
                Use \p lazy_list::stat to enable it.
            */
            typedef empty_stat                      stat;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consistent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_LazyList_rcu "RCU-based LazyList")
            /**
                List of available options see \p opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            //@cond
            // LazyKVList: supporting for split-ordered list
            // key accessor (opt::none = internal key type is equal to user key type)
            typedef opt::none                       key_accessor;

            //@endcond
        };

        /// Metafunction converting option list to \p lazy_list::traits
        /**
            \p Options are:
            - \p opt::lock_type - lock type for node-level locking. Default \p is cds::sync::spin. Note that <b>each</b> node
                of the list has member of type \p lock_type, therefore, heavy-weighted locking primitive is not
                acceptable as candidate for \p lock_type.
            - \p opt::compare - key compare functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for key compare. Default is \p std::less<T>.
            - \p opt::equal_to - specifies binary functor for comparing keys for equality. This option is applicable only for unordered list.
                No default is provided. If \p equal_to is not specified, \p compare is used, if \p compare is not specified, \p less is used.
            - \p opt::sort - specifies ordering policy. Default value is \p true, i.e. the list is ordered.
                Note: unordering feature is not fully supported yet.
            - \p opt::back_off - back-off strategy used. If the option is not specified, \p cds::backoff::Default is used.
            - \p opt::item_counter - the type of item counting feature. Default is disabled (\p atomicity::empty_item_counter).
                To enable item counting use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter
            - \p opt::stat - internal statistics. By default, it is disabled (\p lazy_list::empty_stat).
                To enable it use \p lazy_list::stat
            - \p opt::allocator - the allocator used for creating and freeing list's item. Default is \ref CDS_DEFAULT_ALLOCATOR macro.
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consistent memory model).
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


    } // namespace lazy_list

    // Forward declarations
    template <typename GC, typename T, typename Traits=lazy_list::traits>
    class LazyList;

    template <typename GC, typename Key, typename Value, typename Traits=lazy_list::traits>
    class LazyKVList;

    // Tag for selecting lazy list implementation
    /**
        This empty struct is used only as a tag for selecting \p LazyList
        as ordered list implementation in declaration of some classes.

        See \p split_list::traits::ordered_list as an example.
    */
    struct lazy_list_tag
    {};

}}  // namespace cds::container


#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_LAZY_LIST_BASE_H
