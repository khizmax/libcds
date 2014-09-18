//$$CDS-header$$

#include "map/hdr_skiplist_map_rcu.h"

#include <cds/urcu/general_instant.h>
#include <cds/container/skip_list_map_rcu.h>

#include "unit/michael_alloc.h"
#include "map/print_skiplist_stat.h"

namespace map {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_instant<> > rcu_type;
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_less()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_cmp()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_cmpless()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_less_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_cmp_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_cmpless_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_xorshift_less()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_xorshift_cmp()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_xorshift_cmpless()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_xorshift_less_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_xorshift_cmp_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_xorshift_cmpless_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_turbopas_less()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_turbopas_cmp()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_turbopas_cmpless()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_turbopas_less_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_turbopas_cmp_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_turbopas_cmpless_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_michaelalloc_less()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_michaelalloc_cmp()
    {
            typedef cc::SkipListMap< rcu_type, key_type, value_type,
                cc::skip_list::make_traits<
                    co::compare< cmp >
                    ,co::item_counter< simple_item_counter >
                    ,co::allocator< memory::MichaelAllocator<int> >
                >::type
            > set;
            test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_michaelalloc_cmpless()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_michaelalloc_less_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_michaelalloc_cmp_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_GPI_michaelalloc_cmpless_stat()
    {
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }
} // namespace map

CPPUNIT_TEST_SUITE_REGISTRATION(map::SkipListMapRCUHdrTest);
