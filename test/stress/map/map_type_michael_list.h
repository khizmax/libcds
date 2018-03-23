// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_MICHAEL_LIST_H
#define CDSUNIT_MAP_TYPE_MICHAEL_LIST_H

#include "map_type.h"

#include <cds/container/michael_kvlist_hp.h>
#include <cds/container/michael_kvlist_dhp.h>
#include <cds/container/michael_kvlist_rcu.h>
#include <cds/container/michael_kvlist_nogc.h>

#include <cds_test/stat_michael_list_out.h>

namespace map {

    template <typename Key, typename Value>
    struct michael_list_type
    {
        typedef typename map_type_base<Key, Value>::key_compare compare;
        typedef typename map_type_base<Key, Value>::key_less    less;

        struct traits_MichaelList_cmp :
            public cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_cmp > MichaelList_HP_cmp;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_cmp > MichaelList_DHP_cmp;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_cmp > MichaelList_NOGC_cmp;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_cmp > MichaelList_RCU_GPI_cmp;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_cmp > MichaelList_RCU_GPB_cmp;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_cmp > MichaelList_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_cmp > MichaelList_RCU_SHB_cmp;
#endif

        struct traits_MichaelList_cmp_stat : public traits_MichaelList_cmp
        {
            typedef cc::michael_list::stat<> stat;
        };
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_cmp_stat > MichaelList_HP_cmp_stat;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_cmp_stat > MichaelList_DHP_cmp_stat;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_cmp_stat > MichaelList_NOGC_cmp_stat;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_cmp_stat > MichaelList_RCU_GPI_cmp_stat;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_cmp_stat > MichaelList_RCU_GPB_cmp_stat;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_cmp_stat > MichaelList_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_cmp_stat > MichaelList_RCU_SHB_cmp_stat;
#endif

        struct traits_MichaelList_cmp_seqcst :
            public cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_cmp_seqcst > MichaelList_HP_cmp_seqcst;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_cmp_seqcst > MichaelList_DHP_cmp_seqcst;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_cmp_seqcst > MichaelList_NOGC_cmp_seqcst;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_cmp_seqcst > MichaelList_RCU_GPI_cmp_seqcst;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_cmp_seqcst > MichaelList_RCU_GPB_cmp_seqcst;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_cmp_seqcst > MichaelList_RCU_GPT_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_cmp_seqcst > MichaelList_RCU_SHB_cmp_seqcst;
#endif

        struct traits_MichaelList_less :
            public cc::michael_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_less > MichaelList_HP_less;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_less > MichaelList_DHP_less;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_less > MichaelList_NOGC_less;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_less > MichaelList_RCU_GPI_less;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_less > MichaelList_RCU_GPB_less;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_less > MichaelList_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_less > MichaelList_RCU_SHB_less;
#endif

        struct traits_MichaelList_less_stat: public traits_MichaelList_less
        {
            typedef cc::michael_list::stat<> stat;
        };
        typedef cc::MichaelKVList< cds::gc::HP, Key, Value, traits_MichaelList_less_stat > MichaelList_HP_less_stat;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_less_stat > MichaelList_DHP_less_stat;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_less_stat > MichaelList_NOGC_less_stat;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_less_stat > MichaelList_RCU_GPI_less_stat;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_less_stat > MichaelList_RCU_GPB_less_stat;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_less_stat > MichaelList_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_less_stat > MichaelList_RCU_SHB_less_stat;
#endif

        struct traits_MichaelList_less_seqcst :
            public cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_less_seqcst > MichaelList_HP_less_seqcst;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_less_seqcst > MichaelList_DHP_less_seqcst;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_less_seqcst > MichaelList_NOGC_less_seqcst;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_less_seqcst > MichaelList_RCU_GPI_less_seqcst;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_less_seqcst > MichaelList_RCU_GPB_less_seqcst;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_less_seqcst > MichaelList_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_less_seqcst > MichaelList_RCU_SHB_less_seqcst;
#endif

    };

} // namespace map

#endif // ifndef CDSUNIT_MAP_TYPE_MICHAEL_LIST_H
