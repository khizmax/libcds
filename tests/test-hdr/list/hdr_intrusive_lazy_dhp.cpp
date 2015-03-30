//$$CDS-header$$

#include "list/hdr_intrusive_lazy.h"
#include <cds/intrusive/lazy_list_dhp.h>

namespace ordlist {
    void IntrusiveLazyListHeaderTest::DHP_base_cmp()
    {
        typedef base_int_item< cds::gc::DHP > item;
        struct traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > hook;
            typedef cmp<item> compare;
            typedef faked_disposer disposer;
        };
        typedef ci::LazyList< cds::gc::DHP, item, traits > list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_base_less()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_base_cmpmix()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_base_ic()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_cmp()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_less()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_cmpmix()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::DHP_member_ic()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::LazyList< cds::gc::DHP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }

} // namespace ordlist
