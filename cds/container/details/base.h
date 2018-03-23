// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_BASE_H
#define CDSLIB_CONTAINER_DETAILS_BASE_H

#include <cds/intrusive/details/base.h>

namespace cds {

/// Standard (non-intrusive) containers
/**
    @ingroup cds_nonintrusive_containers
    This namespace contains implementations of non-intrusive (std-like) lock-free containers.
*/
namespace container {

    /// Common options for non-intrusive containers
    /** @ingroup cds_nonintrusive_helper
        This namespace contains options for non-intrusive containers that is, in general, the same as for the intrusive containers.
        It imports all definitions from cds::opt and cds::intrusive::opt namespaces
    */
    namespace opt {
        using namespace cds::intrusive::opt;
    }   // namespace opt

    /// @defgroup cds_nonintrusive_containers Non-intrusive containers
    /** @defgroup cds_nonintrusive_helper Helper structs for non-intrusive containers
        @ingroup cds_nonintrusive_containers
    */

    /** @defgroup cds_nonintrusive_stack Stack
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_queue Queue
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_deque Deque
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_priority_queue Priority queue
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_map Map
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_set Set
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_list List
        @ingroup cds_nonintrusive_containers
    */
    /** @defgroup cds_nonintrusive_tree Tree
        @ingroup cds_nonintrusive_containers
    */


    // Tag for selecting iterable list implementation
    /** @ingroup cds_nonintrusive_helper
        This struct is empty and it is used only as a tag for selecting \p IterableList
        as ordered list implementation in declaration of some classes.

        See \p split_list::traits::ordered_list as an example.
    */
    typedef intrusive::iterable_list_tag iterable_list_tag;

    //@cond
    template <typename List>
    struct is_iterable_list: public cds::intrusive::is_iterable_list< List >
    {};
    //@endcond

}   // namespace container
}   // namespace cds

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_BASE_H
