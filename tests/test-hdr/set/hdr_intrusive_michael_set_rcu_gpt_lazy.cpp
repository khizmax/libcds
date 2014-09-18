//$$CDS-header$$

#include "set/hdr_intrusive_set.h"
#include <cds/urcu/general_threaded.h>
#include <cds/intrusive/lazy_list_rcu.h>
#include <cds/intrusive/michael_set_rcu.h>

namespace set {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_threaded<> >   RCU;
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_base_cmp_lazy()
    {
        typedef base_int_item< ci::lazy_list::node<RCU> > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::opt::back_off< cds::backoff::pause >
            >::type
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_rcu_int<set>();
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_base_less_lazy()
    {
        typedef base_int_item< ci::lazy_list::node<RCU> > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
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
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_base_cmpmix_lazy()
    {
        typedef base_int_item< ci::lazy_list::node<RCU> > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
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
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_member_cmp_lazy()
    {
        typedef member_int_item< ci::lazy_list::node<RCU> > item;
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
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_rcu_int<set>();
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_member_less_lazy()
    {
        typedef member_int_item< ci::lazy_list::node<RCU> > item;
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
        >    bucket_type;

        typedef ci::MichaelHashSet< RCU, bucket_type,
            ci::michael_set::make_traits<
                co::hash< hash_int >
            >::type
        > set;

        test_rcu_int<set>();
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_member_cmpmix_lazy()
    {
        typedef member_int_item< ci::lazy_list::node<RCU> > item;
        typedef ci::LazyList< RCU
            ,item
            ,ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::member_hook<
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
    }


} // namespace set
