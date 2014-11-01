//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/intrusive/lazy_list_hp.h>
#include <cds/intrusive/split_list.h>

namespace set {

    void IntrusiveHashSetHdrTest::split_dyn_HP_base_cmp_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::opt::back_off< cds::backoff::empty >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_base_less_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::split_list::node<ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<true>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_base_cmpmix_stat_lazy()
    {
        typedef base_int_item< ci::split_list::node<ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::stat< ci::split_list::stat<> >
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_member_cmp_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_member_less_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_member_cmpmix_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_member_cmpmix_stat_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        struct list_traits : public ci::lazy_list::traits
        {
            typedef ci::lazy_list::member_hook< offsetof( item, hMember ), co::gc<cds::gc::HP>> hook;
            typedef cmp<item> compare;
            typedef IntrusiveHashSetHdrTest::less<item> less;
            typedef faked_disposer disposer;
        };
        typedef ci::LazyList< cds::gc::HP, item, list_traits > ord_list;

        struct set_traits : public ci::split_list::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef ci::split_list::stat<> stat;
        };
        typedef ci::SplitListSet< cds::gc::HP, ord_list, set_traits > set;

        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }


    // Static bucket table
    void IntrusiveHashSetHdrTest::split_st_HP_base_cmp_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_base_less_lazy()
    {
        typedef base_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::split_list::node<ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_base_cmpmix_stat_lazy()
    {
        typedef base_int_item< ci::split_list::node<ci::lazy_list::node<cds::gc::HP> > > item;
        struct list_traits :
            public ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        {};
        typedef ci::LazyList< cds::gc::HP, item, list_traits > ord_list;

        struct set_traits :
            public ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        {
            typedef ci::split_list::stat<> stat;
        };
        typedef ci::SplitListSet< cds::gc::HP, ord_list, set_traits > set;

        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_member_cmp_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_member_less_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                ci::split_list::dynamic_bucket_table<false>
                ,co::hash< hash_int >
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_member_cmpmix_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_member_cmpmix_stat_lazy()
    {
        typedef member_int_item< ci::split_list::node< ci::lazy_list::node<cds::gc::HP> > > item;
        typedef ci::LazyList< cds::gc::HP
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::stat< ci::split_list::stat<> >
            >::type
        > set;

        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

} // namespace set
