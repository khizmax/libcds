//$$CDS-header$$

#include "list/hdr_intrusive_lazy.h"
#include <cds/intrusive/lazy_list_nogc.h>

namespace ordlist {
    namespace {
        typedef IntrusiveLazyListHeaderTest::base_int_item< cds::gc::nogc > base_item;
        typedef IntrusiveLazyListHeaderTest::member_int_item< cds::gc::nogc > member_item;

        struct cmp_traits : public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > hook;
            typedef IntrusiveLazyListHeaderTest::cmp<base_item> compare;
            typedef IntrusiveLazyListHeaderTest::faked_disposer disposer;
            static const bool sort = false;
        };

        struct less_traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > hook;
            typedef IntrusiveLazyListHeaderTest::less<base_item> less;
            typedef IntrusiveLazyListHeaderTest::faked_disposer disposer;
            static const bool sort = false;
        };

        struct equal_to_traits: public ci::lazy_list::traits {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > hook;
            typedef IntrusiveLazyListHeaderTest::equal_to<base_item> equal_to;
            typedef IntrusiveLazyListHeaderTest::faked_disposer disposer;
            static const bool sort = false;
        };

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
            ,co::less< IntrusiveLazyListHeaderTest::less<base_item> >
            ,co::compare< IntrusiveLazyListHeaderTest::cmp<base_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::sort< false > >::type cmpmix_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
            ,co::compare< IntrusiveLazyListHeaderTest::cmp<base_item> >
            ,co::equal_to< IntrusiveLazyListHeaderTest::equal_to<base_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::sort< false > >::type equal_to_mix_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
            ,co::equal_to< IntrusiveLazyListHeaderTest::equal_to<base_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::sort< false > >::type ic_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::compare< IntrusiveLazyListHeaderTest::cmp<member_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::sort< false > >::type member_cmp_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::less< IntrusiveLazyListHeaderTest::less<member_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::sort< false > >::type member_less_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::equal_to< IntrusiveLazyListHeaderTest::equal_to<member_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::sort< false > >::type member_equal_to_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::less< IntrusiveLazyListHeaderTest::less<member_item> >
            ,co::compare< IntrusiveLazyListHeaderTest::cmp<member_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::sort< false > >::type member_cmpmix_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember )
                ,co::gc<cds::gc::nogc> > >
            ,co::compare< IntrusiveLazyListHeaderTest::cmp<member_item> >
            ,co::equal_to< IntrusiveLazyListHeaderTest::equal_to<member_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::sort< false > >::type member_equal_to_mix_traits;

        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::member_hook<
                offsetof( member_item, hMember ),
                co::gc<cds::gc::nogc> > >
            ,co::equal_to< IntrusiveLazyListHeaderTest::equal_to<member_item> >
            ,ci::opt::disposer< IntrusiveLazyListHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::sort< false > >::type member_ic_traits;

    }
    void IntrusiveLazyListHeaderTest::nogc_base_cmp_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, cmp_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_base_less_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, less_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_base_equal_to_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, equal_to_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_base_cmpmix_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, cmpmix_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_base_equal_to_mix_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, equal_to_mix_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_base_ic_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, base_item, ic_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_member_cmp_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_cmp_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_member_less_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_less_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_member_equal_to_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_equal_to_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_member_cmpmix_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_cmpmix_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_member_equal_to_mix_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_equal_to_mix_traits > list;
        test_nogc_int_unordered<list>();
    }
    void IntrusiveLazyListHeaderTest::nogc_member_ic_unord()
    {
        typedef ci::LazyList< cds::gc::nogc, member_item, member_ic_traits > list;
        test_nogc_int_unordered<list>();
    }

} // namespace ordlist
