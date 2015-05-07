/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "map/hdr_map.h"
#include <cds/urcu/general_threaded.h>
#include <cds/container/michael_kvlist_rcu.h>
#include <cds/container/michael_map_rcu.h>

namespace map {
    namespace {
        struct map_traits: public cc::michael_map::traits
        {
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
        };
        typedef cds::urcu::gc< cds::urcu::general_threaded<> > rcu_type;

        struct RCU_GPT_cmp_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
        };

        struct RCU_GPT_less_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::less  less;
        };

        struct RCU_GPT_cmpmix_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
            typedef HashMapHdrTest::less  less;
        };
    }

    void HashMapHdrTest::Michael_RCU_GPT_cmp()
    {
        typedef cc::MichaelKVList< rcu_type, int, HashMapHdrTest::value_type, RCU_GPT_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< rcu_type, list, map_traits > map;
        test_rcu< map >();

        // option-based version
        typedef cc::MichaelHashMap< rcu_type, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_rcu< opt_map >();
    }

    void HashMapHdrTest::Michael_RCU_GPT_less()
    {
        typedef cc::MichaelKVList< rcu_type, int, HashMapHdrTest::value_type, RCU_GPT_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< rcu_type, list, map_traits > map;
        test_rcu< map >();

        // option-based version
        typedef cc::MichaelHashMap< rcu_type, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_rcu< opt_map >();
    }

    void HashMapHdrTest::Michael_RCU_GPT_cmpmix()
    {
        typedef cc::MichaelKVList< rcu_type, int, HashMapHdrTest::value_type, RCU_GPT_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< rcu_type, list, map_traits > map;
        test_rcu< map >();

        // option-based version
        typedef cc::MichaelHashMap< rcu_type, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_rcu< opt_map >();
    }


} // namespace map

