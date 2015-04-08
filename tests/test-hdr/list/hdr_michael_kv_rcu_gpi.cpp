//$$CDS-header$$

#include "list/hdr_michael_kv.h"
#include <cds/urcu/general_instant.h>
#include <cds/container/michael_kvlist_rcu.h>

namespace ordlist {
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_instant<> > rcu_type;
        struct RCU_GPI_cmp_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
        };
    }
    void MichaelKVListTestHeader::RCU_GPI_cmp()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPI_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPI_less_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
        };
    }
    void MichaelKVListTestHeader::RCU_GPI_less()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPI_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPI_cmpmix_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>  less;
        };
    }
    void MichaelKVListTestHeader::RCU_GPI_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPI_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

    namespace {
        struct RCU_GPI_ic_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelKVListTestHeader::RCU_GPI_ic()
    {
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_GPI_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
    }

}   // namespace ordlist

