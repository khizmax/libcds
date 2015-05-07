/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set/hdr_intrusive_set.h"
#include <cds/urcu/general_instant.h>
#include <cds/intrusive/lazy_list_rcu.h>
#include <cds/intrusive/michael_set_rcu.h>

namespace set {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_instant<> >   RCU;
    }

    void IntrusiveHashSetHdrTest::RCU_GPI_base_cmp_lazy()
    {
        typedef base_int_item< ci::lazy_list::node<RCU> > item;
        struct list_traits :
            public ci::lazy_list::make_traits<
                ci::opt::hook< ci::lazy_list::base_hook< co::gc<RCU> > >
                ,co::compare< cmp<item> >
                ,ci::opt::disposer< faked_disposer >
                ,ci::opt::back_off< cds::backoff::pause >
            >::type
        {};

        typedef ci::LazyList< RCU, item, list_traits >    bucket_type;

        struct set_traits : public ci::michael_set::traits
        {
            typedef hash_int hash;
        };
        typedef ci::MichaelHashSet< RCU, bucket_type, set_traits > set;

        test_rcu_int<set>();
    }

    void IntrusiveHashSetHdrTest::RCU_GPI_base_less_lazy()
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

    void IntrusiveHashSetHdrTest::RCU_GPI_base_cmpmix_lazy()
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

    void IntrusiveHashSetHdrTest::RCU_GPI_member_cmp_lazy()
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

    void IntrusiveHashSetHdrTest::RCU_GPI_member_less_lazy()
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

    void IntrusiveHashSetHdrTest::RCU_GPI_member_cmpmix_lazy()
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
