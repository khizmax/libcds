//$$CDS-header$$

#include "set/hdr_skiplist_set_rcu.h"

#include <cds/urcu/general_threaded.h>
#include <cds/container/skip_list_set_rcu.h>

#include "unit/michael_alloc.h"
#include "map/print_skiplist_stat.h"

namespace set {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_threaded<> > rcu_type;
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_less()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
            co::compare< cmp<item > >
            ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
            co::less< less<item > >
            ,co::compare< cmp<item > >
            ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_xorshift_less()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_xorshift_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_xorshift_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_xorshift_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_xorshift_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_xorshift_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_turbopas_less()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_turbopas_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_turbopas_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_turbopas_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_turbopas_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_turbopas_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_michaelalloc_less()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_michaelalloc_cmp()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_michaelalloc_cmpless()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_michaelalloc_less_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_michaelalloc_cmp_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetRCUHdrTest::SkipList_RCU_GPT_michaelalloc_cmpless_stat()
    {
        typedef cc::SkipListSet< rcu_type, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test< set, misc::print_skiplist_stat<set::stat> >();
    }
} // namespace set
