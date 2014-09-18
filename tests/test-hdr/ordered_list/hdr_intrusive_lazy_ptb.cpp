//$$CDS-header$$

#include "ordered_list/hdr_intrusive_lazy.h"
#include <cds/intrusive/lazy_list_ptb.h>

namespace ordlist {
    void IntrusiveLazyListHeaderTest::PTB_base_cmp()
    {
        typedef base_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::PTB> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::PTB_base_less()
    {
        typedef base_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::PTB> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::PTB_base_cmpmix()
    {
        typedef base_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::PTB> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::PTB_base_ic()
    {
        typedef base_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::PTB> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::PTB_member_cmp()
    {
        typedef member_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::PTB>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::PTB_member_less()
    {
        typedef member_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::PTB>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::PTB_member_cmpmix()
    {
        typedef member_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::PTB>
                > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveLazyListHeaderTest::PTB_member_ic()
    {
        typedef member_int_item< cds::gc::PTB > item;
        typedef ci::LazyList< cds::gc::PTB
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::PTB>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }

} // namespace ordlist
