/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "map/hdr_map.h"
#include <cds/container/michael_kvlist_dhp.h>
#include <cds/container/michael_map.h>

namespace map {
    namespace {
        struct map_traits: public cc::michael_map::traits
        {
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
        };
        struct DHP_cmp_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
        };

        struct DHP_less_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::less  less;
        };

        struct DHP_cmpmix_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
            typedef HashMapHdrTest::less  less;
        };
    }

    void HashMapHdrTest::Michael_DHP_cmp()
    {
        typedef cc::MichaelKVList< cds::gc::DHP, int, HashMapHdrTest::value_type, DHP_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::DHP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::DHP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Michael_DHP_less()
    {
        typedef cc::MichaelKVList< cds::gc::DHP, int, HashMapHdrTest::value_type, DHP_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::DHP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::DHP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Michael_DHP_cmpmix()
    {
        typedef cc::MichaelKVList< cds::gc::DHP, int, HashMapHdrTest::value_type, DHP_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::DHP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::DHP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }


} // namespace map

