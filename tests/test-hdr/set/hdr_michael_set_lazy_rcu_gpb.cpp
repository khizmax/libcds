//$$CDS-header$$

#include "set/hdr_set.h"
#include <cds/urcu/general_buffered.h>
#include <cds/container/lazy_list_rcu.h>
#include <cds/container/michael_set_rcu.h>

namespace set {

    namespace {
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
        };

        struct RCU_GPB_cmp_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
        };

        struct RCU_GPB_less_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };

        struct RCU_GPB_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };
    }

    void HashSetHdrTest::Lazy_RCU_GPB_cmp()
    {
        typedef cc::LazyList< rcu_type, item, RCU_GPB_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< rcu_type, list, set_traits > set;
        test_int_rcu< set >();

        // option-based version
        typedef cc::MichaelHashSet< rcu_type, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int_rcu< opt_set >();
    }

    void HashSetHdrTest::Lazy_RCU_GPB_less()
    {
        typedef cc::LazyList< rcu_type, item, RCU_GPB_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< rcu_type, list, set_traits > set;
        test_int_rcu< set >();

        // option-based version
        typedef cc::MichaelHashSet< rcu_type, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int_rcu< opt_set >();
    }

    void HashSetHdrTest::Lazy_RCU_GPB_cmpmix()
    {
        typedef cc::LazyList< rcu_type, item, RCU_GPB_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< rcu_type, list, set_traits > set;
        test_int_rcu< set >();

        // option-based version
        typedef cc::MichaelHashSet< rcu_type, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int_rcu< opt_set >();
    }


} // namespace set
