// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_ITERABLE_LIST_BASE_H
#define CDSLIB_CONTAINER_DETAILS_ITERABLE_LIST_BASE_H

#include <cds/container/details/base.h>
#include <cds/intrusive/details/iterable_list_base.h>
#include <cds/urcu/options.h>

namespace cds { namespace container {

    /// \p IterableList ordered list related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace iterable_list {

        /// \p IterableList internal statistics, see \p cds::intrusive::iterable_list::stat
        template <typename EventCounter = cds::intrusive::iterable_list::stat<>::event_counter >
        using stat = cds::intrusive::iterable_list::stat< EventCounter >;

        /// \p IterableList empty internal statistics, see \p cds::intrusive::iterable_list::empty_stat
        typedef cds::intrusive::iterable_list::empty_stat empty_stat;

        //@cond
        template <typename Stat = cds::intrusive::iterable_list::wrapped_stat<>::stat_type >
        using wrapped_stat = cds::intrusive::iterable_list::wrapped_stat< Stat >;
        //@endcond

        /// \p IterableList traits
        struct traits
        {
            /// Allocator used to allocate new data
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

            /// Node allocator
            typedef intrusive::iterable_list::traits::node_allocator node_allocator;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none compare;

            /// Specifies binary predicate used for key comparison.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none less;

            /// Back-off strategy
            typedef intrusive::iterable_list::traits::back_off back_off;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter or \p atomicity::cache_friendly_item_counter to enable item counting
            typedef intrusive::iterable_list::traits::item_counter item_counter;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p iterable_list::empty_stat).
                Use \p iterable_list::stat to enable it.
            */
            typedef intrusive::iterable_list::traits::stat stat;

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
            // IterableKVList: supporting for split-ordered list
            // key accessor (opt::none = internal key type is equal to user key type)
            typedef opt::none                       key_accessor;
            //@endcond
        };

        /// Metafunction converting option list to \p iterable_list::traits
        /**
            Supported \p Options are:
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p opt::allocator - an allocator for data, default is \p CDS_DEFAULT_ALLOCATOR
            - \p opt::node_allocator - node allocator, default is \p std::allocator.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::item_counter - the type of item counting feature. Default is disabled (\p atomicity::empty_item_counter).
                 To enable item counting use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter.
            - \p opt::stat - internal statistics. By default, it is disabled (\p iterable_list::empty_stat).
                To enable it use \p iterable_list::stat
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


    } // namespace iterable_list

    // Forward declarations
    template <typename GC, typename T, typename Traits=iterable_list::traits>
    class IterableList;

    template <typename GC, typename Key, typename Value, typename Traits=iterable_list::traits>
    class IterableKVList;

}}  // namespace cds::container


#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_ITERABLE_LIST_BASE_H
