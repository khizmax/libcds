//$$CDS-header$$

#include "list/hdr_intrusive_lazy.h"
#include <cds/urcu/signal_buffered.h>
#include <cds/intrusive/lazy_list_rcu.h>

namespace ordlist {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> >    RCU;
    }
#endif

    void IntrusiveLazyListHeaderTest::RCU_SHB_base_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        typedef ci::lazy_list::make_traits<
            ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
            ,co::compare< cmp<item> >
            ,ci::opt::disposer< faked_disposer >
        >::type list_traits;
        typedef ci::LazyList< RCU, item, list_traits > list;

        test_rcu_int<list>();
#endif
    }

    void IntrusiveLazyListHeaderTest::RCU_SHB_base_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }

    void IntrusiveLazyListHeaderTest::RCU_SHB_base_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }

    void IntrusiveLazyListHeaderTest::RCU_SHB_base_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< RCU > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        >    list;
        test_rcu_int<list>();
#endif
    }

    void IntrusiveLazyListHeaderTest::RCU_SHB_member_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
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

    void IntrusiveLazyListHeaderTest::RCU_SHB_member_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
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

    void IntrusiveLazyListHeaderTest::RCU_SHB_member_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
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

    void IntrusiveLazyListHeaderTest::RCU_SHB_member_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< RCU > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
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

}   // namespace ordlist
