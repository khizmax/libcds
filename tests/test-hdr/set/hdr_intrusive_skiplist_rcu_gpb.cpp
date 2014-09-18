//$$CDS-header$$

#include "set/hdr_intrusive_skiplist_set_rcu.h"

#include <cds/urcu/general_buffered.h>
#include <cds/intrusive/skip_list_rcu.h>
#include "map/print_skiplist_stat.h"

namespace set {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmp()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_less()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmpmix()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::compare< cmp<item> >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmp_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_less_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmpmix_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmp_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_less_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmpmix_xorshift()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmp_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_less_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmpmix_xorshift_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,ci::skip_list::random_level_generator< ci::skip_list::xorshift >
                ,co::stat< ci::skip_list::stat<> >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }


    //*********
    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmp_pascal()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_less_pascal()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmpmix_pascal()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::compare< cmp<item> >
                ,co::item_counter< cds::atomicity::item_counter >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

        void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmp_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_less_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::skip_list::random_level_generator< ci::skip_list::turbo_pascal >
                ,co::stat< ci::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >   set_type;

        test_skiplist<set_type, misc::print_skiplist_stat<set_type::stat> >();
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_gpb_base_cmpmix_pascal_stat()
    {
        typedef base_int_item< ci::skip_list::node< rcu_type> > item;

        typedef ci::SkipListSet< rcu_type, item,
            ci::skip_list::make_traits<
                ci::opt::hook< ci::skip_list::base_hook< co::gc<rcu_type> > >
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

CPPUNIT_TEST_SUITE_REGISTRATION(set::IntrusiveSkipListSetRCU);
