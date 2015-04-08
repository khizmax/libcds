//$$CDS-header$$

#include "list/hdr_lazy.h"
#include <cds/urcu/general_threaded.h>
#include <cds/container/lazy_list_rcu.h>

namespace ordlist {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_threaded<> >    rcu_type;

        struct RCU_GPT_cmp_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
        };
    }
    void LazyListTestHeader::RCU_GPT_cmp()
    {
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_GPT_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_less_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
        };
    }
    void LazyListTestHeader::RCU_GPT_less()
    {
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_GPT_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>  less;
        };
    }
    void LazyListTestHeader::RCU_GPT_cmpmix()
    {
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_GPT_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPT_ic_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyListTestHeader::RCU_GPT_ic()
    {
        // traits-based version
        typedef cc::LazyList< rcu_type, item, RCU_GPT_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::LazyList< rcu_type, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

}   // namespace ordlist

