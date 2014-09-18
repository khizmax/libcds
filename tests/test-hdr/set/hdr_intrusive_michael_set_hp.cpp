//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/intrusive/michael_list_hp.h>
#include <cds/intrusive/michael_set.h>

namespace set {

    void IntrusiveHashSetHdrTest::HP_base_cmp()
    {
        typedef base_int_item< ci::michael_list::node<cds::gc::HP> > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< cds::gc::HP, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::HP_base_less()
    {
        typedef base_int_item< ci::michael_list::node<cds::gc::HP> > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< cds::gc::HP, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::HP_base_cmpmix()
    {
        typedef base_int_item< ci::michael_list::node<cds::gc::HP> > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< cds::gc::HP, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::HP_member_cmp()
    {
        typedef member_int_item< ci::michael_list::node<cds::gc::HP> > item;
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
        >    bucket_type;

        typedef ci::MichaelHashSet< cds::gc::HP, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::HP_member_less()
    {
        typedef member_int_item< ci::michael_list::node<cds::gc::HP> > item;
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
        >    bucket_type;

        typedef ci::MichaelHashSet< cds::gc::HP, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_int<set>();
    }

    void IntrusiveHashSetHdrTest::HP_member_cmpmix()
    {
        typedef member_int_item< ci::michael_list::node<cds::gc::HP> > item;
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
        >    bucket_type;

        typedef ci::MichaelHashSet< cds::gc::HP, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;

        test_int<set>();
    }


} // namespace set
CPPUNIT_TEST_SUITE_REGISTRATION(set::IntrusiveHashSetHdrTest);
