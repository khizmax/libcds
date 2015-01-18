/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "ordered_list/hdr_michael_kv.h"
#include <cds/urcu/signal_buffered.h>
#include <cds/container/michael_kvlist_rcu.h>

namespace ordlist {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> > rcu_type;
        struct RCU_SHB_cmp_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
        };
    }
#endif
    void MichaelKVListTestHeader::RCU_SHB_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_SHB_cmp_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHB_less_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
        };
    }
#endif
    void MichaelKVListTestHeader::RCU_SHB_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_SHB_less_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHB_cmpmix_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>  less;
        };
    }
#endif
    void MichaelKVListTestHeader::RCU_SHB_cmpmix()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_SHB_cmpmix_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        struct RCU_SHB_ic_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
#endif
    void MichaelKVListTestHeader::RCU_SHB_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // traits-based version
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, RCU_SHB_ic_traits > list;
        test_rcu< list >();

        // option-based version

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test_rcu< opt_list >();
#endif
    }

}   // namespace ordlist
