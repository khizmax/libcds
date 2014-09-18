//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/intrusive/michael_list_hp.h>
#include <cds/intrusive/split_list.h>

namespace set {

    void IntrusiveHashSetHdrTest::split_dyn_HP_base_cmp()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    ord_list;

        typedef ci::SplitListSet< cds::gc::HP, ord_list,
            ci::split_list::make_traits<
                co::hash< hash_int >
                ,ci::split_list::dynamic_bucket_table<true>
                ,co::memory_model<co::v::relaxed_ordering>
            >::type
        > set;
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_base_less()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
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
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_base_cmpmix()
    {
        typedef base_int_item< ci::split_list::node<ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
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
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_member_cmp()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_member_less()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_dyn_HP_member_cmpmix()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
        static_assert( set::options::dynamic_bucket_table, "Set has static bucket table" );

        test_int<set>();
    }


    // Static bucket table
    void IntrusiveHashSetHdrTest::split_st_HP_base_cmp()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
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
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_base_less()
    {
        typedef base_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
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
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_base_cmpmix()
    {
        typedef base_int_item< ci::split_list::node<ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
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
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_member_cmp()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_member_less()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::split_st_HP_member_cmpmix()
    {
        typedef member_int_item< ci::split_list::node< ci::michael_list::node<cds::gc::HP> > > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
        static_assert( !set::options::dynamic_bucket_table, "Set has dynamic bucket table" );

        test_int<set>();
    }


} // namespace set
