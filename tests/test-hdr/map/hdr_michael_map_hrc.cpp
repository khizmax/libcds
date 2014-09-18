//$$CDS-header$$

#include "map/hdr_map.h"
#include <cds/container/michael_kvlist_hrc.h>
#include <cds/container/michael_map.h>

namespace map {
    namespace {
        struct map_traits: public cc::michael_map::type_traits
        {
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
        };
        struct HRC_cmp_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare;
        };

        struct HRC_less_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::less  less;
        };

        struct HRC_cmpmix_traits: public cc::michael_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare;
            typedef HashMapHdrTest::less  less;
        };
    }

    void HashMapHdrTest::Michael_HRC_cmp()
    {
        typedef cc::MichaelKVList< cds::gc::HRC, int, HashMapHdrTest::value_type, HRC_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HRC, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HRC, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Michael_HRC_less()
    {
        typedef cc::MichaelKVList< cds::gc::HRC, int, HashMapHdrTest::value_type, HRC_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HRC, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HRC, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Michael_HRC_cmpmix()
    {
        typedef cc::MichaelKVList< cds::gc::HRC, int, HashMapHdrTest::value_type, HRC_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HRC, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HRC, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }


} // namespace map

