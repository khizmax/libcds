// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_MAP_BOOST_FLAT_MAP_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_MAP_BOOST_FLAT_MAP_ADAPTER_H

#include <boost/version.hpp>
#if BOOST_VERSION < 104800
#   error "For boost::container::flat_map you must use boost 1.48 or above"
#endif

#include <cds/container/striped_set/adapter.h>
#include <boost/container/flat_map.hpp>


//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for map
        template <typename Key, typename T, typename Traits, typename Alloc>
        struct copy_item_policy< boost::container::flat_map< Key, T, Traits, Alloc > >
            : public details::boost_map_copy_policies<boost::container::flat_map< Key, T, Traits, Alloc > >::copy_item_policy
        {};

        // Swap item policy
        template <typename Key, typename T, typename Traits, typename Alloc>
        struct swap_item_policy< boost::container::flat_map< Key, T, Traits, Alloc > >
            : public details::boost_map_copy_policies<boost::container::flat_map< Key, T, Traits, Alloc > >::swap_item_policy
        {};

        // Move policy for map
        template <typename Key, typename T, typename Traits, typename Alloc>
        struct move_item_policy< boost::container::flat_map< Key, T, Traits, Alloc > >
            : public details::boost_map_copy_policies<boost::container::flat_map< Key, T, Traits, Alloc > >::move_item_policy
        {};
    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {

    template <typename Key, typename T, class Traits, class Alloc, typename... Options>
    class adapt< boost::container::flat_map< Key, T, Traits, Alloc>, Options... >
    {
    public:
        typedef boost::container::flat_map< Key, T, Traits, Alloc>     container_type          ;   ///< underlying container type
        typedef cds::container::striped_set::details::boost_map_adapter< container_type, Options... >    type;
    };
}}} // namespace cds::intrusive::striped_set

//@endcond

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_MAP_BOOST_FLAT_MAP_ADAPTER_H
