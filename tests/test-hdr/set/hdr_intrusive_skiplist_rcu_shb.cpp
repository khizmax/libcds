//$$CDS-header$$

#include "set/hdr_intrusive_skiplist_set_rcu.h"

#include <cds/urcu/signal_buffered.h>
#include <cds/intrusive/skip_list_rcu.h>
#include "map/print_skiplist_stat.h"

namespace set {

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> > rcu_type;
    }
#endif

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmp_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmpmix_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmp_xorshift()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_less_xorshift()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmpmix_xorshift()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmp_xorshift_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_less_xorshift_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmpmix_xorshift_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }


    //*********
    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmp_pascal()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_less_pascal()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmpmix_pascal()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

        void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmp_pascal_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_less_pascal_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

    void IntrusiveSkipListSetRCU::skiplist_rcu_shb_base_cmpmix_pascal_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
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
#endif
    }

}   // namespace set

CPPUNIT_TEST_SUITE_REGISTRATION(set::IntrusiveSkipListSetRCU);
