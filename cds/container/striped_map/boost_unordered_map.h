/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSLIB_CONTAINER_STRIPED_MAP_BOOST_UNORDERED_MAP_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_MAP_BOOST_UNORDERED_MAP_ADAPTER_H

#include <cds/container/striped_set/adapter.h>
#include <boost/unordered_map.hpp>

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for map
        template <typename Key, typename T, typename Traits, typename Alloc>
        struct copy_item_policy< boost::unordered_map< Key, T, Traits, Alloc > >
            : public details::boost_map_copy_policies<boost::unordered_map< Key, T, Traits, Alloc > >::copy_item_policy
        {};

        // Swap policy for map
        template <typename Key, typename T, typename Traits, typename Alloc>
        struct swap_item_policy< boost::unordered_map< Key, T, Traits, Alloc > >
            : public details::boost_map_copy_policies<boost::unordered_map< Key, T, Traits, Alloc > >::swap_item_policy
        {};

        // Move policy for map
        template <typename Key, typename T, typename Traits, typename Alloc>
        struct move_item_policy< boost::unordered_map< Key, T, Traits, Alloc > >
            : public details::boost_map_copy_policies<boost::unordered_map< Key, T, Traits, Alloc > >::move_item_policy
        {};
    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {

    /// boost::unordered_map  adapter for hash map bucket
    template <typename Key, typename T, class Hash, class Pred, class Alloc, typename... Options>
    class adapt< boost::unordered_map< Key, T, Hash, Pred, Alloc>, Options... >
    {
    public:
        typedef boost::unordered_map< Key, T, Hash, Pred, Alloc>  container_type  ;   ///< underlying container type
        typedef cds::container::striped_set::details::boost_map_adapter< container_type, Options... >    type;
    };
}}} // namespace cds::intrusive::striped_set


//@endcond

#endif  // #ifndef CDSLIB_CONTAINER_STRIPED_MAP_BOOST_UNORDERED_MAP_ADAPTER_H
