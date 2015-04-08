//$$CDS-header$$

#include "map/hdr_map.h"
#include <cds/container/lazy_kvlist_nogc.h>
#include <cds/container/michael_map_nogc.h>

namespace map {
    namespace {
        struct map_traits: public cc::michael_map::traits
        {
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
        };
        struct nogc_cmp_traits: public cc::lazy_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
        };

        struct nogc_less_traits: public cc::lazy_list::traits
        {
            typedef HashMapHdrTest::less  less;
        };

        struct nogc_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
            typedef HashMapHdrTest::less  less;
        };

        struct nogc_equal_traits: public cc::lazy_list::traits
        {
            typedef HashMapHdrTest::equal  equal_to;
            static const bool sort = false;
        };
    }

    void HashMapHdrTest::Lazy_nogc_cmp()
    {
        typedef cc::LazyKVList< cds::gc::nogc, int, HashMapHdrTest::value_type, nogc_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list, map_traits > map;
        test_int_nogc< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int_nogc< opt_map >();
    }

    void HashMapHdrTest::Lazy_nogc_less()
    {
        typedef cc::LazyKVList< cds::gc::nogc, int, HashMapHdrTest::value_type, nogc_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list, map_traits > map;
        test_int_nogc< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int_nogc< opt_map >();
    }

    void HashMapHdrTest::Lazy_nogc_equal()
    {
        typedef cc::LazyKVList< cds::gc::nogc, int, HashMapHdrTest::value_type, nogc_equal_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list, map_traits > map;
        test_int_nogc_unordered< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int_nogc_unordered< opt_map >();
    }

    void HashMapHdrTest::Lazy_nogc_cmpmix()
    {
        typedef cc::LazyKVList< cds::gc::nogc, int, HashMapHdrTest::value_type, nogc_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list, map_traits > map;
        test_int_nogc< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::nogc, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int_nogc< opt_map >();
    }

}   // namespace map
