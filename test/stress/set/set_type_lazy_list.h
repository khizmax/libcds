// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TYPE_LAZY_LIST_H
#define CDSUNIT_SET_TYPE_LAZY_LIST_H

#include "set_type.h"

#include <cds/container/lazy_list_hp.h>
#include <cds/container/lazy_list_dhp.h>
#include <cds/container/lazy_list_rcu.h>

namespace set {

    template <typename Key, typename Val>
    struct lazy_list_type
    {
        typedef typename set_type_base< Key, Val >::key_val key_val;
        typedef typename set_type_base< Key, Val >::compare compare;
        typedef typename set_type_base< Key, Val >::less    less;

        struct traits_LazyList_cmp :
            public cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_cmp > LazyList_HP_cmp;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp > LazyList_DHP_cmp;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp > LazyList_RCU_GPI_cmp;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp > LazyList_RCU_GPB_cmp;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp > LazyList_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp > LazyList_RCU_SHB_cmp;
#endif

        struct traits_LazyList_cmp_stat : public traits_LazyList_cmp
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_cmp_stat > LazyList_HP_cmp_stat;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp_stat > LazyList_DHP_cmp_stat;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp_stat > LazyList_RCU_GPI_cmp_stat;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp_stat > LazyList_RCU_GPB_cmp_stat;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp_stat > LazyList_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp_stat > LazyList_RCU_SHB_cmp_stat;
#endif

        struct traits_LazyList_cmp_seqcst :
            public cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP, key_val,  traits_LazyList_cmp_seqcst > LazyList_HP_cmp_seqcst;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp_seqcst > LazyList_DHP_cmp_seqcst;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_GPI_cmp_seqcst;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_GPB_cmp_seqcst;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_GPT_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp_seqcst > LazyList_RCU_SHB_cmp_seqcst;
#endif

        struct traits_LazyList_less:
            public cc::lazy_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_less > LazyList_HP_less;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less > LazyList_DHP_less;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less > LazyList_RCU_GPI_less;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less > LazyList_RCU_GPB_less;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less > LazyList_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less > LazyList_RCU_SHB_less;
#endif

        struct traits_LazyList_less_stat: public traits_LazyList_less
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyList< cds::gc::HP, key_val, traits_LazyList_less_stat > LazyList_HP_less_stat;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less_stat > LazyList_DHP_less_stat;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less_stat > LazyList_RCU_GPI_less_stat;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less_stat > LazyList_RCU_GPB_less_stat;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less_stat > LazyList_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less_stat > LazyList_RCU_SHB_less_stat;
#endif

        struct traits_LazyList_less_seqcst :
            public cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP, key_val,  traits_LazyList_less_seqcst > LazyList_HP_less_seqcst;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less_seqcst > LazyList_DHP_less_seqcst;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less_seqcst > LazyList_RCU_GPI_less_seqcst;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less_seqcst > LazyList_RCU_GPB_less_seqcst;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less_seqcst > LazyList_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less_seqcst > LazyList_RCU_SHB_less_seqcst;
#endif

    };

} // namespace set

#endif // #ifndef CDSUNIT_SET_TYPE_LAZY_LIST_H
