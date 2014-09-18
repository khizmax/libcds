//$$CDS-header$$

#include "map/hdr_map.h"
#include <cds/container/lazy_kvlist_ptb.h>
#include <cds/container/michael_map.h>

namespace map {
    namespace {
        struct map_traits: public cc::michael_map::type_traits
        {
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
        };
        struct PTB_cmp_traits: public cc::lazy_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare;
        };

        struct PTB_less_traits: public cc::lazy_list::type_traits
        {
            typedef HashMapHdrTest::less  less;
        };

        struct PTB_cmpmix_traits: public cc::lazy_list::type_traits
        {
            typedef HashMapHdrTest::cmp   compare;
            typedef HashMapHdrTest::less  less;
        };
    }

    void HashMapHdrTest::Lazy_PTB_cmp()
    {
        typedef cc::LazyKVList< cds::gc::PTB, int, HashMapHdrTest::value_type, PTB_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Lazy_PTB_less()
    {
        typedef cc::LazyKVList< cds::gc::PTB, int, HashMapHdrTest::value_type, PTB_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Lazy_PTB_cmpmix()
    {
        typedef cc::LazyKVList< cds::gc::PTB, int, HashMapHdrTest::value_type, PTB_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::PTB, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }


} // namespace map

