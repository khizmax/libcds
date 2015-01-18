/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "ordered_list/hdr_lazy_kv.h"
#include <cds/urcu/general_buffered.h>
#include <cds/container/lazy_kvlist_rcu.h>

namespace ordlist {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

        struct RCU_GPB_cmp_traits: public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
        };
    }
    void LazyKVListTestHeader::RCU_GPB_cmp()
    {
        // traits-based version
        typedef cc::LazyKVList< rcu_type, key_type, value_type, RCU_GPB_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyKVList< rcu_type, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPB_less_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
        };
    }
    void LazyKVListTestHeader::RCU_GPB_less()
    {
        // traits-based version
        typedef cc::LazyKVList< rcu_type, key_type, value_type, RCU_GPB_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyKVList< rcu_type, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPB_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>  less;
        };
    }
    void LazyKVListTestHeader::RCU_GPB_cmpmix()
    {
        // traits-based version
        typedef cc::LazyKVList< rcu_type, key_type, value_type, RCU_GPB_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyKVList< rcu_type, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPB_ic_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyKVListTestHeader::RCU_GPB_ic()
    {
        // traits-based version
        typedef cc::LazyKVList< rcu_type, key_type, value_type, RCU_GPB_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyKVList< rcu_type, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

}   // namespace ordlist

