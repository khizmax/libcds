//$$CDS-header$$

#include "list/hdr_intrusive_michael.h"
#include <cds/intrusive/michael_list_dhp.h>

namespace ordlist {
    void IntrusiveMichaelListHeaderTest::DHP_base_cmp()
    {
        typedef base_int_item< cds::gc::DHP > item;
        struct traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook< co::gc<cds::gc::DHP> > hook;
            typedef cmp<item> compare;
            typedef faked_disposer disposer;
        };
        typedef ci::MichaelList< cds::gc::DHP, item, traits > list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::DHP_base_less()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::DHP_base_cmpmix()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::DHP_base_ic()
    {
        typedef base_int_item< cds::gc::DHP > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<cds::gc::DHP> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::DHP_member_cmp()
    {
        typedef member_int_item< cds::gc::DHP > item;
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
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::DHP_member_less()
    {
        typedef member_int_item< cds::gc::DHP > item;
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
        >    list;
        test_int<list>();
    }
    void IntrusiveMichaelListHeaderTest::DHP_member_cmpmix()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
    void IntrusiveMichaelListHeaderTest::DHP_member_ic()
    {
        typedef member_int_item< cds::gc::DHP > item;
        typedef ci::MichaelList< cds::gc::DHP
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
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
