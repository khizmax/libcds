//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/urcu/signal_threaded.h>
#include <cds/intrusive/michael_list_rcu.h>
#include <cds/intrusive/michael_set_rcu.h>

namespace set {

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> >   RCU;
    }
#endif

    void IntrusiveHashSetHdrTest::RCU_SHT_base_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< ci::michael_list::node<RCU> > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<RCU> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_rcu_int<set>();
#endif
    }

    void IntrusiveHashSetHdrTest::RCU_SHT_base_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< ci::michael_list::node<RCU> > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<RCU> > >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_rcu_int<set>();
#endif
    }

    void IntrusiveHashSetHdrTest::RCU_SHT_base_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef base_int_item< ci::michael_list::node<RCU> > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::base_hook< co::gc<RCU> > >
                ,co::less< less<item> >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;

        test_rcu_int<set>();
#endif
    }

    void IntrusiveHashSetHdrTest::RCU_SHT_member_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< ci::michael_list::node<RCU> > item;
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
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_rcu_int<set>();
#endif
    }

    void IntrusiveHashSetHdrTest::RCU_SHT_member_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< ci::michael_list::node<RCU> > item;
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
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_rcu_int<set>();
#endif
    }

    void IntrusiveHashSetHdrTest::RCU_SHT_member_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef member_int_item< ci::michael_list::node<RCU> > item;
        typedef ci::MichaelList< RCU
            ,item
            ,ci::michael_list::make_traits<
                ci::opt::hook< ci::michael_list::member_hook<
                    offsetof( item, hMember ),
                    co::gc<RCU>
                > >
                ,co::compare< cmp<item> >
                ,co::less< less<item> >
                ,ci::opt::disposer< faked_disposer >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
                ,co::item_counter< simple_item_counter >
            >::type
        > set;

        test_rcu_int<set>();
#endif
    }


} // namespace set
