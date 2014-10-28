//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/intrusive/lazy_list_nogc.h>
#include <cds/intrusive/split_list_nogc.h>

namespace set {

    void IntrusiveHashSetHdrTest::split_dyn_nogc_base_cmp_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::opt::back_off< cds::backoff::empty >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_nogc_base_less_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_nogc_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<true>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_nogc_base_cmpmix_stat_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        struct list_traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > hook;
            typedef IntrusiveHashSetHdrTest::less<item> less;
            typedef cmp<item> compare;
            typedef faked_disposer disposer;
        };
        typedef ci::LazyList< cds::gc::nogc, item, list_traits > bucket_type;

        struct set_traits : public ci::split_list::make_traits<ci::split_list::dynamic_bucket_table<true>>::type
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::split_list::stat<> stat;
        };
        typedef ci::SplitListSet< cds::gc::nogc, bucket_type, set_traits > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_nogc_member_cmp_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_nogc_member_less_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_nogc_member_cmpmix_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<true>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_nogc_member_cmpmix_stat_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        struct list_traits :
            public ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        {};
        typedef ci::LazyList< cds::gc::nogc, item, list_traits > bucket_type;

        struct set_traits :
            public ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::stat< ci::split_list::stat<> >
            >::type
        {};
        typedef ci::SplitListSet< cds::gc::nogc, bucket_type, set_traits > set;

        test_int_nogc<set>();
    }

    // Static bucket table
    void IntrusiveHashSetHdrTest::split_st_nogc_base_cmp_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_nogc_base_less_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_nogc_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_nogc_base_cmpmix_stat_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::nogc> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::stat< ci::split_list::stat<> >
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_nogc_member_cmp_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_nogc_member_less_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_nogc_member_cmpmix_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        > set;

        test_int_nogc<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_nogc_member_cmpmix_stat_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::nogc> > > item;
        typedef ci::LazyList< cds::gc::nogc
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::nogc>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::SplitListSet< cds::gc::nogc, bucket_type,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::stat< ci::split_list::stat<> >
            >::type
        > set;

        test_int_nogc<set>();
    }

} // namespace set
