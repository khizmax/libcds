//$$CDS-header$$

#include "unordered_list/hdr_intrusive_lazy.h"
#include <cds/intrusive/lazy_list_nogc.h>

namespace unordlist {
    namespace {
        typedef base_int_item< cds::gc::nogc > base_item;
        typedef member_int_item< cds::gc::nogc > member_item;

        struct cmp_traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > hook;
            typedef unordlist::cmp<base_item> compare;
            typedef faked_disposer disposer;
            static const bool sort = false;
        };

        struct less_traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > hook;
            typedef unordlist::less<base_item> less;
            typedef faked_disposer disposer;
            static const bool sort = false;
        };

        struct equal_to_traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > hook;
            typedef unordlist::equal_to<base_item> equal_to;
            typedef faked_disposer disposer;
            static const bool sort = false;
        };

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
            ,co::less< less<base_item> >
            ,co::compare< cmp<base_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::sort< false > >::type cmpmix_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
            ,co::compare< cmp<base_item> >
            ,co::equal_to< equal_to<base_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::sort< false > >::type equal_to_mix_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
            ,co::equal_to< equal_to<base_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::sort< false > >::type ic_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::compare< cmp<member_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::sort< false > >::type member_cmp_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::less< less<member_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::sort< false > >::type member_less_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::equal_to< equal_to<member_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::sort< false > >::type member_equal_to_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::less< less<member_item> >
            ,co::compare< cmp<member_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::sort< false > >::type member_cmpmix_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::compare< cmp<member_item> >
            ,co::equal_to< equal_to<member_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::sort< false > >::type member_equal_to_mix_traits;

        typedef typename ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember ),
                co::gc<cds::gc::nogc> > >
            ,co::equal_to< equal_to<member_item> >
            ,ci::opt::disposer< faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::sort< false > >::type member_ic_traits;

    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_base_cmp()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, cmp_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_base_less()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, less_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_base_equal_to()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, equal_to_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_base_cmpmix()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, cmpmix_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_base_equal_to_mix()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, equal_to_mix_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_base_ic()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, ic_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_member_cmp()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_cmp_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_member_less()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_less_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_member_equal_to()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_equal_to_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_member_cmpmix()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_cmpmix_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_member_equal_to_mix()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_equal_to_mix_traits > list;
        test_nogc_int<list>();
    }
    void UnorderedIntrusiveLazyListHeaderTest::nogc_member_ic()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_ic_traits > list;
        test_nogc_int<list>();
    }

} // namespace unordlist

CPPUNIT_TEST_SUITE_REGISTRATION(unordlist::UnorderedIntrusiveLazyListHeaderTest);
