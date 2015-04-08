//$$CDS-header$$

#include "list/hdr_intrusive_michael.h"
#include <cds/intrusive/michael_list_hp.h>

namespace ordlist {
    void IntrusiveMichaelListHeaderTest::HP_base_cmp()
    {
        typedef base_int_item< cds::gc::HP > item;
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< co::gc<cds::gc::HP> > hook;
            typedef cmp<item> compare;
            typedef faked_disposer disposer;
        };
        typedef ci::MichaelList< cds::gc::HP, item, traits > list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::HP_base_less()
    {
        typedef base_int_item< cds::gc::HP > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::HP_base_cmpmix()
    {
        typedef base_int_item< cds::gc::HP > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::HP_base_ic()
    {
        typedef base_int_item< cds::gc::HP > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::HP_member_cmp()
    {
        typedef member_int_item< cds::gc::HP > item;
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
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::HP_member_less()
    {
        typedef member_int_item< cds::gc::HP > item;
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
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::HP_member_cmpmix()
    {
        typedef member_int_item< cds::gc::HP > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::HP_member_ic()
    {
        typedef member_int_item< cds::gc::HP > item;
        typedef ci::MichaelList< cds::gc::HP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<cds::gc::HP>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }
}   // namespace ordlist

CPPUNIT_TEST_SUITE_REGISTRATION(ordlist::IntrusiveMichaelListHeaderTest);
