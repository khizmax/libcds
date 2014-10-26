//$$CDS-header$$

#include "map/hdr_map.h"
#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/michael_map.h>

namespace map {
    namespace {
        struct map_traits: public cc::michael_map::traits
        {
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
        };
        struct HP_cmp_traits: public cc::lazy_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
        };

        struct HP_less_traits: public cc::lazy_list::traits
        {
            typedef HashMapHdrTest::less  less;
        };

        struct HP_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
            typedef HashMapHdrTest::less  less;
        };
    }

    void HashMapHdrTest::Lazy_HP_cmp()
    {
        typedef cc::LazyKVList< cds::gc::HP, int, HashMapHdrTest::value_type, HP_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Lazy_HP_less()
    {
        typedef cc::LazyKVList< cds::gc::HP, int, HashMapHdrTest::value_type, HP_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Lazy_HP_cmpmix()
    {
        typedef cc::LazyKVList< cds::gc::HP, int, HashMapHdrTest::value_type, HP_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }


} // namespace map

