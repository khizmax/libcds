//$$CDS-header$$

#include "set/hdr_intrusive_skiplist_set.h"

#include <cds/intrusive/skip_list_hp.h>
#include "map/print_skiplist_stat.h"

namespace set {

    void IntrusiveSkipListSet::skiplist_hp_base_cmp()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;
        struct set_traits : public ci::skip_list::traits
        {
            typedef ci::skip_list::base_hook< co::gc<cds::gc::HP> > hook;
            typedef cmp<item> compare;
            typedef faked_disposer disposer;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::SkipListSet< cds::gc::HP, item, set_traits > set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_less()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmpmix()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmp_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_less_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmpmix_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmp_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_less_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmpmix_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSet::skiplist_hp_base_cmp_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_less_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmpmix_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }


    //*********
    void IntrusiveSkipListSet::skiplist_hp_base_cmp_pascal()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_less_pascal()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmpmix_pascal()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSet::skiplist_hp_base_cmp_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_less_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSet::skiplist_hp_base_cmpmix_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< cds::gc::HP> > item;

        typedef ci::SkipListSet< cds::gc::HP, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<cds::gc::HP> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

}   // namespace set

CPPUNIT_TEST_SUITE_REGISTRATION(set::IntrusiveSkipListSet);
