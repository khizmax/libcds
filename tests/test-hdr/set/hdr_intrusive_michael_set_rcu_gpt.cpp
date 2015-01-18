/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set/hdr_intrusive_set.h"
#include <cds/urcu/general_threaded.h>
#include <cds/intrusive/michael_list_rcu.h>
#include <cds/intrusive/michael_set_rcu.h>

namespace set {

    namespace {
        typedef cds::urcu::gc< cds::urcu::general_threaded<> >   RCU;
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_base_cmp()
    {
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
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_base_less()
    {
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
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_base_cmpmix()
    {
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
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_member_cmp()
    {
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
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_member_less()
    {
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
    }

    void IntrusiveHashSetHdrTest::RCU_GPT_member_cmpmix()
    {
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
    }


} // namespace set
