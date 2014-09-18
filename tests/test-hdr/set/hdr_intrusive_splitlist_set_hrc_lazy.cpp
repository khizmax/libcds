//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/intrusive/lazy_list_hrc.h>
#include <cds/intrusive/split_list.h>

namespace set {

    void IntrusiveHashSetHdrTest::split_dyn_HRC_base_cmp_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HRC> > > item;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HRC, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HRC_base_less_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HRC> > > item;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HRC, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HRC_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::split_list::node<ci::lazy_list::node<cds::gc::HRC> > > item;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HRC, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<true>
            >::type
        > set;
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    // Static bucket table
    void IntrusiveHashSetHdrTest::split_st_HRC_base_cmp_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HRC> > > item;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HRC, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HRC_base_less_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HRC> > > item;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HRC, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HRC_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::split_list::node<ci::lazy_list::node<cds::gc::HRC> > > item;
        typedef ci::LazyList< cds::gc::HRC
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HRC> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HRC, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        > set;
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

} // namespace set
