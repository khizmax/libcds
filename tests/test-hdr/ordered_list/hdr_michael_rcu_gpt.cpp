/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "ordered_list/hdr_michael.h"
#include <cds/urcu/general_threaded.h>
#include <cds/container/michael_list_rcu.h>

namespace ordlist {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_threaded<> >    rcu_type;

        struct RCU_GPT_cmp_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
        };
    }

    void MichaelListTestHeader::RCU_GPT_cmp()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_less_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
        };
    }
    void MichaelListTestHeader::RCU_GPT_less()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_cmpmix_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>  less;
        };
    }
    void MichaelListTestHeader::RCU_GPT_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_ic_traits : public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelListTestHeader::RCU_GPT_ic()
    {
        // traits-based version
        typedef cc::MichaelList< rcu_type, item, RCU_GPT_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelList< rcu_type, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

}   // namespace ordlist

