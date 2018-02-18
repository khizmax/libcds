// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_LAZY_LIST_H
#define CDSUNIT_MAP_TYPE_LAZY_LIST_H

#include "map_type.h"

#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/lazy_kvlist_dhp.h>
#include <cds/container/lazy_kvlist_rcu.h>
#include <cds/container/lazy_kvlist_nogc.h>

#include <cds_test/stat_lazy_list_out.h>

namespace map {

    template <typename Key, typename Value>
    struct lazy_list_type
    {
        typedef typename map_type_base<Key, Value>::key_compare compare;
        typedef typename map_type_base<Key, Value>::key_less    less;
        typedef typename map_type_base<Key, Value>::equal_to    equal_to;

        struct traits_LazyList_cmp:
            public cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp > LazyList_HP_cmp;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp > LazyList_DHP_cmp;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp > LazyList_NOGC_cmp;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp > LazyList_RCU_GPI_cmp;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp > LazyList_RCU_GPB_cmp;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp > LazyList_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp > LazyList_RCU_SHB_cmp;
#endif

        struct traits_LazyList_cmp_stat: public traits_LazyList_cmp
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_stat > LazyList_HP_cmp_stat;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_stat > LazyList_DHP_cmp_stat;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_stat > LazyList_NOGC_cmp_stat;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_GPI_cmp_stat;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_GPB_cmp_stat;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_stat > LazyList_RCU_SHB_cmp_stat;
#endif

        struct traits_LazyList_unord :
            public cc::lazy_list::make_traits<
                co::equal_to< equal_to >
                ,co::sort< false >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_unord > LazyList_NOGC_unord;

        struct traits_LazyList_cmp_seqcst :
            public cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_seqcst > LazyList_HP_cmp_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_seqcst > LazyList_DHP_cmp_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_seqcst > LazyList_NOGC_cmp_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_GPI_cmp_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_GPB_cmp_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_GPT_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_seqcst > LazyList_RCU_SHB_cmp_seqcst;
#endif

        struct traits_LazyList_less :
            public cc::lazy_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less > LazyList_HP_less;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less > LazyList_DHP_less;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less > LazyList_NOGC_less;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less > LazyList_RCU_GPI_less;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less > LazyList_RCU_GPB_less;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less > LazyList_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less > LazyList_RCU_SHB_less;
#endif

        struct traits_LazyList_less_stat: public traits_LazyList_less
        {
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_stat > LazyList_HP_less_stat;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_stat > LazyList_DHP_less_stat;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_stat > LazyList_NOGC_less_stat;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_stat > LazyList_RCU_GPI_less_stat;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_stat > LazyList_RCU_GPB_less_stat;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_stat > LazyList_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_stat > LazyList_RCU_SHB_less_stat;
#endif

        struct traits_LazyList_less_seqcst :
            public cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_seqcst > LazyList_HP_less_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_seqcst > LazyList_DHP_less_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_seqcst > LazyList_NOGC_less_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_GPI_less_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_GPB_less_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_seqcst > LazyList_RCU_SHB_less_seqcst;
#endif

    };

} // namespace map

#endif // ifndef CDSUNIT_MAP_TYPE_LAZY_LIST_H
