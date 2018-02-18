// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_TREE_TEST_ELLEN_BINTREE_UPDATE_DESC_POOL_H
#define CDSUNIT_TREE_TEST_ELLEN_BINTREE_UPDATE_DESC_POOL_H

#include <cds/memory/vyukov_queue_pool.h>
#include <cds/memory/pool_allocator.h>

namespace cds_test {

    typedef cds::intrusive::ellen_bintree::update_desc< void, void > update_desc;

    // update_desc pools
    struct pool_traits: public cds::memory::vyukov_queue_pool_traits
    {
        typedef cds::opt::v::uninitialized_static_buffer< update_desc, 256 > buffer;
    };
    typedef cds::memory::vyukov_queue_pool< update_desc, pool_traits > pool_type;
    typedef cds::memory::lazy_vyukov_queue_pool< update_desc, pool_traits > lazy_pool_type;

    extern pool_type      s_Pool;
    extern lazy_pool_type s_LazyPool;

    struct pool_accessor
    {
        typedef pool_type::value_type value_type;

        pool_type& operator()() const
        {
            return s_Pool;
        }
    };

    struct lazy_pool_accessor
    {
        typedef lazy_pool_type::value_type value_type;

        lazy_pool_type& operator()() const
        {
            return s_LazyPool;
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_TREE_TEST_ELLEN_BINTREE_UPDATE_DESC_POOL_H
