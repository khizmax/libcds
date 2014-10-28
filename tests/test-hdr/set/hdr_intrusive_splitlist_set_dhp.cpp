//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/intrusive/michael_list_dhp.h>
#include <cds/intrusive/split_list.h>

namespace set {

    void IntrusiveHashSetHdrTest::split_dyn_DHP_base_cmp()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_DHP_base_less()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_DHP_base_cmpmix()
    {
        typedef base_int_item< ci::split_list::node<ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<true>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_DHP_base_cmpmix_stat()
    {
        typedef base_int_item< ci::split_list::node<ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
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

    void IntrusiveHashSetHdrTest::split_dyn_DHP_member_cmp()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_DHP_member_less()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_DHP_member_cmpmix()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_DHP_member_cmpmix_stat()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        struct list_traits : public ci::michael_list::traits
        {
            typedef ci::michael_list::member_hook< offsetof( item, hMember ), co::gc<cds::gc::DHP> > hook;
            typedef cmp<item> compare;
            typedef IntrusiveHashSetHdrTest::less<item> less;
            typedef faked_disposer disposer;
        };
        typedef ci::MichaelList< cds::gc::DHP, item, list_traits > ord_list;

        struct set_traits : public ci::split_list::traits {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
            typedef co::v::sequential_consistent memory_model;
            typedef ci::split_list::stat<> stat;
        };
        typedef ci::SplitListSet< cds::gc::DHP, ord_list, set_traits > set;
        static_assert( set::traits::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }


    // Static bucket table
    void IntrusiveHashSetHdrTest::split_st_DHP_base_cmp()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_DHP_base_less()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_DHP_base_cmpmix()
    {
        typedef base_int_item< ci::split_list::node<ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_DHP_base_cmpmix_stat()
    {
        typedef base_int_item< ci::split_list::node<ci::michael_list::node<cds::gc::DHP> > > item;
        struct list_traits :
            public ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        {};
        typedef ci::MichaelList< cds::gc::DHP, item, list_traits > ord_list;

        struct set_traits :
            public ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::stat< ci::split_list::stat<> >
            >::type
        {};
        typedef ci::SplitListSet< cds::gc::DHP, ord_list, set_traits > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_DHP_member_cmp()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<false>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_DHP_member_less()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                ci::split_list::dynamic_bucket_table<false>
                ,co::hash< hash_int >
                ,co::memory_model<co::v::sequential_consistent>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_DHP_member_cmpmix()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
                ,ci::split_list::dynamic_bucket_table<false>
            >::type
        > set;
        static_assert( !set::traits::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_DHP_member_cmpmix_stat()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::DHP> > > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::DHP>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::DHP, ord_list,
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
