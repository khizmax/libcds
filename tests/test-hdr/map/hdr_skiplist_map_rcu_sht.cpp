//$$CDS-header$$

#include "map/hdr_skiplist_map_rcu.h"

#include <cds/urcu/signal_threaded.h>
#include <cds/container/skip_list_map_rcu.h>

#include "unit/michael_alloc.h"
#include "map/print_skiplist_stat.h"

namespace map {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> > rcu_type;
    }
#endif

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_cmpless()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_cmp_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_cmpless_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_xorshift_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_xorshift_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_xorshift_cmpless()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_xorshift_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_xorshift_cmp_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_xorshift_cmpless_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_turbopas_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_turbopas_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_turbopas_cmpless()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_turbopas_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_turbopas_cmp_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_turbopas_cmpless_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_michaelalloc_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_michaelalloc_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_michaelalloc_cmpless()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_michaelalloc_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::less< less >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_michaelalloc_cmp_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_type, key_type, value_type,
            cc::skip_list::make_traits<
                co::compare< cmp >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
#endif
    }

    void SkipListMapRCUHdrTest::SkipList_RCU_SHT_michaelalloc_cmpless_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

} // namespace map
