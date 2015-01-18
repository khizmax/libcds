/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "ordered_list/hdr_lazy.h"
#include <cds/urcu/signal_buffered.h>
#include <cds/container/lazy_list_rcu.h>

namespace ordlist {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> >    rcu_type;

        struct RCU_SHB_cmp_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHB_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHB_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHB_less_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHB_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHB_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHB_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>  less;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHB_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHB_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHB_ic_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
#endif
    void LazyListTestHeader::RCU_SHB_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_SHB_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

}   // namespace ordlist

