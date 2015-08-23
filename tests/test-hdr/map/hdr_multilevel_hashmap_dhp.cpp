//$$CDS-header$$

#include "map/hdr_multilevel_hashmap.h"
#include <cds/container/multilevel_hashmap_dhp.h>
#include "unit/print_multilevel_hashset_stat.h"

namespace map {
    namespace {
        typedef cds::gc::DHP gc_type;
    } // namespace

    void MultiLevelHashMapHdrTest::dhp_stdhash()
    {
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item > map_type;

        test_hp<map_type>(4, 2);
    }

    void MultiLevelHashMapHdrTest::dhp_hash128()
    {
        struct traits : public cc::multilevel_hashmap::traits {
            typedef hash128::make hash;
            typedef hash128::less less;
        };
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item, traits > map_type;
        test_hp<map_type>(4, 2);

        typedef cc::MultiLevelHashMap< gc_type, size_t, Item,
            typename cc::multilevel_hashmap::make_traits<
                co::hash< hash128::make >
                , co::less< hash128::less >
            >::type
        > map_type2;
        test_hp<map_type2>(4, 2);
    }

    void MultiLevelHashMapHdrTest::dhp_stdhash_stat()
    {
        struct traits : public cc::multilevel_hashmap::traits {
            typedef cc::multilevel_hashmap::stat<> stat;
        };
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item, traits > map_type;
        test_hp<map_type>(4, 2);

        typedef cc::MultiLevelHashMap< gc_type, size_t, Item,
            typename cc::multilevel_hashmap::make_traits<
                co::stat< cc::multilevel_hashmap::stat<>>
            >::type
        > map_type2;
        test_hp<map_type2>(4, 2);
    }

        void MultiLevelHashMapHdrTest::dhp_hash128_stat()
    {
        struct traits : public cc::multilevel_hashmap::traits {
            typedef cc::multilevel_hashmap::stat<> stat;
            typedef hash128::make hash;
            typedef hash128::cmp compare;
        };
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item, traits > map_type;
        test_hp<map_type>(4, 2);

        typedef cc::MultiLevelHashMap< gc_type, size_t, Item,
            typename cc::multilevel_hashmap::make_traits<
                co::stat< cc::multilevel_hashmap::stat<>>
                , co::hash< hash128::make >
                , co::compare< hash128::cmp >
            >::type
        > map_type2;
        test_hp<map_type2>(4, 2);
    }

    void MultiLevelHashMapHdrTest::dhp_stdhash_5_3()
    {
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item > map_type;

        test_hp<map_type>(5, 3);
    }

    void MultiLevelHashMapHdrTest::dhp_stdhash_5_3_stat()
    {
        struct traits : public cc::multilevel_hashmap::traits {
            typedef cc::multilevel_hashmap::stat<> stat;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item, traits > map_type;
        test_hp<map_type>(5, 3);

        typedef cc::MultiLevelHashMap< gc_type, size_t, Item,
            typename cc::multilevel_hashmap::make_traits<
                co::stat< cc::multilevel_hashmap::stat<>>
                ,co::back_off< cds::backoff::empty >
            >::type
        > map_type2;
        test_hp<map_type2>(5, 3);
    }

    void MultiLevelHashMapHdrTest::dhp_hash128_4_3()
    {
        struct traits : public cc::multilevel_hashmap::traits {
            typedef hash128::make hash;
            typedef hash128::less less;
        };
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item, traits > map_type;
        test_hp<map_type>(4, 3);

        typedef cc::MultiLevelHashMap< gc_type, size_t, Item,
            typename cc::multilevel_hashmap::make_traits<
                co::hash< hash128::make >
                , co::less< hash128::less >
            >::type
        > map_type2;
        test_hp<map_type2>(4, 3);
    }

    void MultiLevelHashMapHdrTest::dhp_hash128_4_3_stat()
    {
        struct traits : public cc::multilevel_hashmap::traits {
            typedef hash128::make hash;
            typedef hash128::less less;
            typedef cc::multilevel_hashmap::stat<> stat;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef cc::MultiLevelHashMap< gc_type, size_t, Item, traits > map_type;
        test_hp<map_type>(4, 3);

        typedef cc::MultiLevelHashMap< gc_type, size_t, Item,
            typename cc::multilevel_hashmap::make_traits<
                co::hash< hash128::make >
                , co::less< hash128::less >
                , co::stat< cc::multilevel_hashmap::stat<>>
                , co::memory_model< co::v::sequential_consistent >
            >::type
        > map_type2;
        test_hp<map_type2>(4, 3);
    }

} // namespace map

CPPUNIT_TEST_SUITE_REGISTRATION(map::MultiLevelHashMapHdrTest);
