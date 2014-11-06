//$$CDS-header$$

#include "set/hdr_skiplist_set.h"
#include <cds/container/skip_list_set_nogc.h>
#include "unit/michael_alloc.h"
#include "map/print_skiplist_stat.h"

namespace set {

    void SkipListSetHdrTest::SkipList_NOGC_less()
    {
        struct set_traits : public cc::skip_list::traits
        {
            typedef SkipListSetHdrTest::less<item> less;
            typedef simple_item_counter item_counter;
        };
        typedef cc::SkipListSet< cds::gc::nogc, item, set_traits > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_cmp()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
            co::compare< cmp<item > >
            ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_cmpless()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
            co::less< less<item > >
            ,co::compare< cmp<item > >
            ,co::item_counter< simple_item_counter >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_less_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_cmp_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_cmpless_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_xorshift_less()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_xorshift_cmp()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_xorshift_cmpless()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_xorshift_less_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_xorshift_cmp_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_xorshift_cmpless_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_turbopas_less()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_turbopas_cmp()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_turbopas_cmpless()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_turbopas_less_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_turbopas_cmp_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_turbopas_cmpless_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_michaelalloc_less()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_michaelalloc_cmp()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_michaelalloc_cmpless()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_michaelalloc_less_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_michaelalloc_cmp_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }

    void SkipListSetHdrTest::SkipList_NOGC_michaelalloc_cmpless_stat()
    {
        typedef cc::SkipListSet< cds::gc::nogc, item,
            cc::skip_list::make_traits<
                co::less< less<item > >
                ,co::compare< cmp<item > >
                ,co::item_counter< simple_item_counter >
                ,co::stat< cc::skip_list::stat<> >
                ,co::allocator< memory::MichaelAllocator<int> >
            >::type
        > set;
        test_nogc< set, misc::print_skiplist_stat<set::stat> >();
    }
} // namespace set
