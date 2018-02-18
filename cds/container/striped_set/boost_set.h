// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_SET_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_SET_BOOST_SET_ADAPTER_H

#include <boost/version.hpp>
#if BOOST_VERSION < 104800
#   error "For boost::container::set you must use boost 1.48 or above"
#endif

#include <cds/container/striped_set/adapter.h>
#include <boost/container/set.hpp>

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for boost::container::set
        template <typename T, typename Traits, typename Alloc>
        struct copy_item_policy< boost::container::set< T, Traits, Alloc > >
            : public details::boost_set_copy_policies< boost::container::set< T, Traits, Alloc > >::copy_item_policy
        {};
        // Copy policy for boost::container::set
        template <typename T, typename Traits, typename Alloc>
        struct swap_item_policy< boost::container::set< T, Traits, Alloc > >
            : public details::boost_set_copy_policies< boost::container::set< T, Traits, Alloc > >::swap_item_policy
        {};

        // Swap policy is not defined for boost::container::set

        // Move policy for boost::container::set
        template <typename T, typename Traits, typename Alloc>
        struct move_item_policy< boost::container::set< T, Traits, Alloc > >
            : public details::boost_set_copy_policies< boost::container::set< T, Traits, Alloc > >::move_item_policy
        {};

    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {
    /// boost::container::flat_set adapter for hash set bucket
    template <typename T, class Traits, class Alloc, typename... Options>
    class adapt< boost::container::set<T, Traits, Alloc>, Options... >
    {
    public:
        typedef boost::container::set<T, Traits, Alloc>     container_type ;   ///< underlying container type
        typedef cds::container::striped_set::details::boost_set_adapter< container_type, Options... >    type;
    };
}}} // namespace cds::intrusive::striped_set


//@endcond

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_SET_ADAPTER_H
