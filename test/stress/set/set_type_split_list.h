// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TYPE_SPLIT_LIST_H
#define CDSUNIT_SET_TYPE_SPLIT_LIST_H

#include "set_type.h"

#include <cds/container/michael_list_hp.h>
#include <cds/container/michael_list_dhp.h>
#include <cds/container/michael_list_rcu.h>
#include <cds/container/lazy_list_hp.h>
#include <cds/container/lazy_list_dhp.h>
#include <cds/container/lazy_list_rcu.h>
#include <cds/container/iterable_list_hp.h>
#include <cds/container/iterable_list_dhp.h>

#include <cds/container/split_list_set.h>
#include <cds/container/split_list_set_rcu.h>

#include <cds_test/stat_splitlist_out.h>
#include <cds_test/stat_michael_list_out.h>
#include <cds_test/stat_lazy_list_out.h>
#include <cds_test/stat_iterable_list_out.h>

namespace set {

    template <typename GC, typename T, typename Traits = cc::split_list::traits>
    class SplitListSet : public cc::SplitListSet< GC, T, Traits >
    {
        typedef cc::SplitListSet< GC, T, Traits > base_class;
    public:
        template <typename Config>
        SplitListSet( Config const& cfg )
            : base_class( cfg.s_nSetSize, cfg.s_nLoadFactor )
        {}

        template <typename Iterator>
        typename std::enable_if< std::is_same< Iterator, typename base_class::iterator>::value, Iterator>::type
        get_begin()
        {
            return base_class::begin();
        }

        template <typename Iterator>
        typename std::enable_if< std::is_same< Iterator, typename base_class::iterator>::value, Iterator>::type
        get_end()
        {
            return base_class::end();
        }

        // for testing
        static constexpr bool const c_bExtractSupported = true;
        static constexpr bool const c_bLoadFactorDepended = true;
        static constexpr bool const c_bEraseExactKey = false;
    };

    struct tag_SplitListSet;

    template <typename Key, typename Val>
    struct set_type< tag_SplitListSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;

        // ***************************************************************************
        // SplitListSet based on MichaelList

        struct traits_SplitList_Michael_dyn_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_HP_dyn_cmp;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_DHP_dyn_cmp;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPI_dyn_cmp;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPB_dyn_cmp;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHB_dyn_cmp;
#endif

        struct traits_SplitList_Michael_dyn_cmp_swar: public traits_SplitList_Michael_dyn_cmp
        {
            typedef cds::algo::bit_reversal::swar bit_reversal;
        };
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Michael_dyn_cmp_swar > SplitList_Michael_HP_dyn_cmp_swar;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp_swar > SplitList_Michael_DHP_dyn_cmp_swar;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp_swar > SplitList_Michael_RCU_GPI_dyn_cmp_swar;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp_swar > SplitList_Michael_RCU_GPB_dyn_cmp_swar;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp_swar > SplitList_Michael_RCU_GPT_dyn_cmp_swar;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp_swar > SplitList_Michael_RCU_SHB_dyn_cmp_swar;
#endif

        struct traits_SplitList_Michael_dyn_cmp_stat :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,co::stat< cc::split_list::stat<> >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::stat< cc::michael_list::stat<>>
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_HP_dyn_cmp_stat;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_DHP_dyn_cmp_stat;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPI_dyn_cmp_stat;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPB_dyn_cmp_stat;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHB_dyn_cmp_stat;
#endif

        struct traits_SplitList_Michael_dyn_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_HP_dyn_cmp_seqcst;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_DHP_dyn_cmp_seqcst;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPI_dyn_cmp_seqcst;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPB_dyn_cmp_seqcst;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHB_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Michael_st_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_HP_st_cmp;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_DHP_st_cmp;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPI_st_cmp;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPB_st_cmp;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHB_st_cmp;
#endif

        //HP + less
        struct traits_SplitList_Michael_dyn_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_HP_dyn_less;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_DHP_dyn_less;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPI_dyn_less;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPB_dyn_less;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHB_dyn_less;
#endif

        struct traits_SplitList_Michael_st_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less > SplitList_Michael_HP_st_less;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_DHP_st_less;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPI_st_less;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPB_st_less;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHB_st_less;
#endif

        struct traits_SplitList_Michael_st_less_stat :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,co::stat< cc::split_list::stat<>>
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::stat< cc::michael_list::stat<>>
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_HP_st_less_stat;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_DHP_st_less_stat;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPI_st_less_stat;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPB_st_less_stat;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHB_st_less_stat;
#endif

        // ***************************************************************************
        // SplitListSet based on LazyList

        struct traits_SplitList_Lazy_dyn_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_HP_dyn_cmp;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_DHP_dyn_cmp;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPI_dyn_cmp;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPB_dyn_cmp;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp;
#endif

        struct traits_SplitList_Lazy_dyn_cmp_stat : public traits_SplitList_Lazy_dyn_cmp
        {
            typedef cc::split_list::stat<> stat;
            typedef typename cc::lazy_list::make_traits<
                co::compare< compare >
                , co::stat< cc::lazy_list::stat<>>
            >::type ordered_list_traits;
        };
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_HP_dyn_cmp_stat;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_DHP_dyn_cmp_stat;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPI_dyn_cmp_stat;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPB_dyn_cmp_stat;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp_stat;
#endif

        struct traits_SplitList_Lazy_dyn_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_HP_dyn_cmp_seqcst;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_DHP_dyn_cmp_seqcst;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPI_dyn_cmp_seqcst;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPB_dyn_cmp_seqcst;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHB_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Lazy_st_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_HP_st_cmp;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_DHP_st_cmp;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPI_st_cmp;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPB_st_cmp;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHB_st_cmp;
#endif

        struct traits_SplitList_Lazy_dyn_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_HP_dyn_less;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_DHP_dyn_less;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPI_dyn_less;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPB_dyn_less;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHB_dyn_less;
#endif

        struct traits_SplitList_Lazy_st_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_HP_st_less;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_DHP_st_less;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPI_st_less;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPB_st_less;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHB_st_less;
#endif

        struct traits_SplitList_Lazy_st_less_stat : public traits_SplitList_Lazy_st_less
        {
            typedef cc::split_list::stat<> stat;
            typedef typename cc::lazy_list::make_traits<
                co::less< less >
                , co::stat< cc::lazy_list::stat<>>
            >::type ordered_list_traits;
        };
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_HP_st_less_stat;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_DHP_st_less_stat;
        typedef SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPI_st_less_stat;
        typedef SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPB_st_less_stat;
        typedef SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHB_st_less_stat;
#endif

        // ***************************************************************************
        // SplitListSet based on IterableList

        struct traits_SplitList_Iterable_dyn_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::iterable_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::iterable_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Iterable_dyn_cmp > SplitList_Iterable_HP_dyn_cmp;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Iterable_dyn_cmp > SplitList_Iterable_DHP_dyn_cmp;

        struct traits_SplitList_Iterable_dyn_cmp_stat:
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::iterable_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,co::stat< cc::split_list::stat<> >
                ,cc::split_list::ordered_list_traits<
                    typename cc::iterable_list::make_traits<
                        co::compare< compare >
                        ,co::stat< cc::iterable_list::stat<>>
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP, key_val, traits_SplitList_Iterable_dyn_cmp_stat > SplitList_Iterable_HP_dyn_cmp_stat;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Iterable_dyn_cmp_stat > SplitList_Iterable_DHP_dyn_cmp_stat;

        struct traits_SplitList_Iterable_dyn_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::iterable_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::iterable_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Iterable_dyn_cmp_seqcst > SplitList_Iterable_HP_dyn_cmp_seqcst;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Iterable_dyn_cmp_seqcst > SplitList_Iterable_DHP_dyn_cmp_seqcst;

        struct traits_SplitList_Iterable_st_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::iterable_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::iterable_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Iterable_st_cmp > SplitList_Iterable_HP_st_cmp;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Iterable_st_cmp > SplitList_Iterable_DHP_st_cmp;

        //HP + less
        struct traits_SplitList_Iterable_dyn_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::iterable_list_tag>
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::iterable_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Iterable_dyn_less > SplitList_Iterable_HP_dyn_less;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Iterable_dyn_less > SplitList_Iterable_DHP_dyn_less;

        struct traits_SplitList_Iterable_st_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::iterable_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,cc::split_list::ordered_list_traits<
                    typename cc::iterable_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Iterable_st_less > SplitList_Iterable_HP_st_less;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Iterable_st_less > SplitList_Iterable_DHP_st_less;

        struct traits_SplitList_Iterable_st_less_stat :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::iterable_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                , co::item_counter<cds::atomicity::cache_friendly_item_counter >
                ,co::stat< cc::split_list::stat<>>
                ,cc::split_list::ordered_list_traits<
                    typename cc::iterable_list::make_traits<
                        co::less< less >
                        ,co::stat< cc::iterable_list::stat<>>
                    >::type
                >
            >::type
        {};
        typedef SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Iterable_st_less_stat > SplitList_Iterable_HP_st_less_stat;
        typedef SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Iterable_st_less_stat > SplitList_Iterable_DHP_st_less_stat;

    };

    template <typename GC, typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, SplitListSet<GC, T, Traits> const& s )
    {
        o << s.statistics()
          << cds_test::stat_prefix( "list_stat" )
          << s.list_statistics()
          << cds_test::stat_prefix( "" );
    }

} // namespace set


#define CDSSTRESS_SplitListSet_case( fixture, test_case, splitlist_set_type, key_type, value_type ) \
    TEST_P( fixture, splitlist_set_type ) \
    { \
        typedef set::set_type< tag_SplitListSet, key_type, value_type >::splitlist_set_type set_type; \
        test_case<set_type>(); \
    }


#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 1
#   define CDSSTRESS_SplitListSet_SHRCU_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHB_dyn_cmp_seqcst,  key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHT_dyn_cmp_seqcst,  key_type, value_type ) \

#else
#   define CDSSTRESS_SplitListSet_SHRCU_2( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL == 1
#   define CDSSTRESS_SplitListSet_SHRCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHT_dyn_cmp,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHB_dyn_cmp_stat,    key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHB_st_cmp,          key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHB_dyn_less,        key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHB_st_less,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHT_st_less_stat,    key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHT_dyn_cmp,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHB_dyn_cmp_stat,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHT_st_cmp,             key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHB_dyn_less,           key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHT_st_less,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHB_st_less_stat,       key_type, value_type ) \

#else
#   define CDSSTRESS_SplitListSet_SHRCU_1( fixture, test_case, key_type, value_type )
#endif


#   define CDSSTRESS_SplitListSet_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHB_dyn_cmp,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHT_dyn_cmp_stat,    key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHT_st_cmp,          key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHT_dyn_less,        key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHT_st_less,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_SHB_st_less_stat,    key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHB_dyn_cmp,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHT_dyn_cmp_stat,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHB_st_cmp,             key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHT_dyn_less,           key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHB_st_less,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_SHT_st_less_stat,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_SHRCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_SHRCU_2( fixture, test_case, key_type, value_type ) \

#else
#   define CDSSTRESS_SplitListSet_SHRCU( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 1
#   define CDSSTRESS_SplitListSet_HP_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_dyn_cmp_seqcst,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_dyn_cmp_seqcst,      key_type, value_type ) \

#   define CDSSTRESS_SplitListSet_RCU_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPI_dyn_cmp_seqcst,  key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_dyn_cmp_seqcst,  key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_dyn_cmp_seqcst,  key_type, value_type ) \

#else
#   define CDSSTRESS_SplitListSet_HP_2( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SplitListSet_RCU_2( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL == 1
#   define CDSSTRESS_SplitListSet_HP_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_dyn_cmp,             key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_dyn_cmp_swar,        key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_dyn_cmp_stat,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_st_cmp,               key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_dyn_less,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_st_less,              key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_st_less_stat,        key_type, value_type ) \
        \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_DHP_dyn_cmp,                key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_HP_dyn_cmp_stat,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_DHP_st_cmp,                 key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_HP_dyn_less,                key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_DHP_st_less,                key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_HP_st_less_stat,            key_type, value_type ) \

#   define CDSSTRESS_SplitListSet_RCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_dyn_cmp_swar,    key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPI_dyn_cmp_stat,    key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_dyn_cmp_stat,    key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPI_st_cmp,          key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_st_cmp,          key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_dyn_less,        key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPI_st_less,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_st_less,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_st_less_stat,    key_type, value_type ) \
        \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPB_dyn_cmp,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPI_dyn_cmp_stat,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPT_dyn_cmp_stat,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPB_st_cmp,             key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPI_dyn_less,           key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPT_dyn_less,           key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPB_st_less,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPI_st_less_stat,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPT_st_less_stat,       key_type, value_type ) \
        \
        CDSSTRESS_SplitListSet_SHRCU( fixture, test_case, key_type, value_type )


#else
#   define CDSSTRESS_SplitListSet_HP_1( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SplitListSet_RCU_1( fixture, test_case, key_type, value_type )
#endif

#define CDSSTRESS_SplitListSet_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_dyn_cmp,              key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_dyn_cmp_swar,         key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_dyn_cmp_stat,        key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_st_cmp,              key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_dyn_less,             key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_DHP_st_less,             key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_HP_st_less_stat,         key_type, value_type ) \
    \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_HP_dyn_cmp,                 key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_DHP_dyn_cmp_stat,           key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_HP_st_cmp,                  key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_DHP_dyn_less,               key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_HP_st_less,                 key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_DHP_st_less_stat,           key_type, value_type ) \
    CDSSTRESS_SplitListSet_HP_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SplitListSet_HP_2( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_SplitListSet_RCU( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPI_dyn_cmp,         key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_dyn_cmp,         key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_dyn_cmp_swar,    key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_dyn_cmp,         key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_dyn_cmp_stat,    key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_st_cmp,          key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPI_dyn_less,        key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_dyn_less,        key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPB_st_less,         key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPI_st_less_stat,    key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Michael_RCU_GPT_st_less_stat,    key_type, value_type ) \
    \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPI_dyn_cmp,            key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPT_dyn_cmp,            key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPB_dyn_cmp_stat,       key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPI_st_cmp,             key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPT_st_cmp,             key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPB_dyn_less,           key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPI_st_less,            key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPT_st_less,            key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Lazy_RCU_GPB_st_less_stat,       key_type, value_type ) \
    CDSSTRESS_SplitListSet_RCU_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SplitListSet_RCU_2( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_SplitListSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SplitListSet_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SplitListSet_RCU( fixture, test_case, key_type, value_type ) \

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 0
#   define CDSSTRESS_SplitListIterableSet_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_DHP_dyn_cmp,             key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_HP_dyn_cmp_stat,         key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_HP_dyn_cmp_seqcst,       key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_DHP_dyn_cmp_seqcst,      key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_HP_st_cmp,               key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_DHP_dyn_less,            key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_HP_st_less,              key_type, value_type ) \
        CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_DHP_st_less_stat,        key_type, value_type ) \

#else
#   define CDSSTRESS_SplitListIterableSet_1( fixture, test_case, key_type, value_type )
#endif

#define CDSSTRESS_SplitListIterableSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_HP_dyn_cmp,              key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_DHP_dyn_cmp_stat,        key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_DHP_st_cmp,              key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_HP_dyn_less,             key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_DHP_st_less,             key_type, value_type ) \
    CDSSTRESS_SplitListSet_case( fixture, test_case, SplitList_Iterable_HP_st_less_stat,         key_type, value_type ) \
    CDSSTRESS_SplitListIterableSet_1( fixture, test_case, key_type, value_type )

#endif // #ifndef CDSUNIT_SET_TYPE_SPLIT_LIST_H
