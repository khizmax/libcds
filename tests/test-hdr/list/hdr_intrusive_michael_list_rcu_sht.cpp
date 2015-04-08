//$$CDS-header$$

#include "list/hdr_intrusive_michael.h"
#include <cds/urcu/signal_threaded.h>
#include <cds/intrusive/michael_list_rcu.h>

namespace ordlist {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> >    RCU;
    }
#endif

    void IntrusiveMichaelListHeaderTest::RCU_SHT_base_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        struct traits : public ci::michael_list::traits
        {
            typedef ci::michael_list::base_hook< co::gc<RCU> > hook;
            typedef cmp<item> compare;
            typedef faked_disposer disposer;
        };
        typedef ci::MichaelList< RCU, item, traits > list;
        test_rcu_int<list>();
#endif
    }
    void IntrusiveMichaelListHeaderTest::RCU_SHT_base_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        struct traits : public ci::michael_list::traits
        {
            typedef ci::michael_list::base_hook< co::gc<RCU> > hook;
            typedef IntrusiveMichaelListHeaderTest::less<item> less;
            typedef faked_disposer disposer;
        };
        typedef ci::MichaelList< RCU, item, traits > list;
        test_rcu_int<list>();
#endif
    }
    void IntrusiveMichaelListHeaderTest::RCU_SHT_base_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<RCU> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }
    void IntrusiveMichaelListHeaderTest::RCU_SHT_base_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<RCU> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }
    void IntrusiveMichaelListHeaderTest::RCU_SHT_member_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<RCU>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }
    void IntrusiveMichaelListHeaderTest::RCU_SHT_member_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<RCU>
                > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }
    void IntrusiveMichaelListHeaderTest::RCU_SHT_member_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<RCU>
                > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }
    void IntrusiveMichaelListHeaderTest::RCU_SHT_member_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<RCU>
                > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }

}
