//$$CDS-header$$

#ifndef CDSUNIT_SET_TYPE_SPLIT_LIST_H
#define CDSUNIT_SET_TYPE_SPLIT_LIST_H

#include "set2/set_type.h"

#include <cds/container/michael_list_hp.h>
#include <cds/container/michael_list_dhp.h>
#include <cds/container/michael_list_rcu.h>
#include <cds/container/lazy_list_hp.h>
#include <cds/container/lazy_list_dhp.h>
#include <cds/container/lazy_list_rcu.h>

#include <cds/container/split_list_set.h>
#include <cds/container/split_list_set_rcu.h>

#include "print_split_list_stat.h"

namespace set2 {

    template <typename Key, typename Val>
    struct set_type< cc::split_list::implementation_tag, Key, Val >: public set_type_base< Key, Val >
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
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_HP_dyn_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_DHP_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPI_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPB_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHB_dyn_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHT_dyn_cmp;
#endif

        struct traits_SplitList_Michael_dyn_cmp_stat :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::stat< cc::split_list::stat<> >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_HP_dyn_cmp_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_DHP_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPI_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHT_dyn_cmp_stat;
#endif

        struct traits_SplitList_Michael_dyn_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_HP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_DHP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPI_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Michael_st_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_HP_st_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_DHP_st_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPI_st_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPB_st_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHB_st_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHT_st_cmp;
#endif

        struct traits_SplitList_Michael_st_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_HP_st_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_DHP_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_GPI_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_GPB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_SHB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_SHT_st_cmp_seqcst;
#endif

        //HP + less
        struct traits_SplitList_Michael_dyn_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_HP_dyn_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_DHP_dyn_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPI_dyn_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPB_dyn_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHB_dyn_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHT_dyn_less;
#endif

        struct traits_SplitList_Michael_dyn_less_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val,  traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_HP_dyn_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_DHP_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPI_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHT_dyn_less_seqcst;
#endif

        struct traits_SplitList_Michael_st_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less > SplitList_Michael_HP_st_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_DHP_st_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPI_st_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPB_st_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHB_st_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHT_st_less;
#endif

        struct traits_SplitList_Michael_st_less_stat :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,co::stat< cc::split_list::stat<>>
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_HP_st_less_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_DHP_st_less_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPI_st_less_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPB_st_less_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHB_st_less_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHT_st_less_stat;
#endif

        struct traits_SplitList_Michael_st_less_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_HP_st_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_DHP_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPI_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHT_st_less_seqcst;
#endif

        // ***************************************************************************
        // SplitListSet based on LazyList

        struct traits_SplitList_Lazy_dyn_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_HP_dyn_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_DHP_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPI_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPB_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHT_dyn_cmp;
#endif

        struct traits_SplitList_Lazy_dyn_cmp_stat : public traits_SplitList_Lazy_dyn_cmp
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_HP_dyn_cmp_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_DHP_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPI_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHT_dyn_cmp_stat;
#endif

        struct traits_SplitList_Lazy_dyn_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_HP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_DHP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPI_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Lazy_st_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_HP_st_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_DHP_st_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPI_st_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPB_st_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHB_st_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHT_st_cmp;
#endif

        struct traits_SplitList_Lazy_st_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_HP_st_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_DHP_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_GPI_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_GPB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_SHB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_SHT_st_cmp_seqcst;
#endif

        struct traits_SplitList_Lazy_dyn_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_HP_dyn_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_DHP_dyn_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPI_dyn_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPB_dyn_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHB_dyn_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHT_dyn_less;
#endif

        struct traits_SplitList_Lazy_dyn_less_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_HP_dyn_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_DHP_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPI_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHT_dyn_less_seqcst;
#endif

        struct traits_SplitList_Lazy_st_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_HP_st_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_DHP_st_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPI_st_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPB_st_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHB_st_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHT_st_less;
#endif

        struct traits_SplitList_Lazy_st_less_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_HP_st_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_DHP_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPI_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHT_st_less_seqcst;
#endif

        struct traits_SplitList_Lazy_st_less_stat : public traits_SplitList_Lazy_st_less
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_HP_st_less_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_DHP_st_less_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPI_st_less_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPB_st_less_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHB_st_less_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHT_st_less_stat;
#endif
    };

    template <typename GC, typename T, typename Traits>
    static inline void print_stat( cc::SplitListSet<GC, T, Traits> const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }

} // namespace set2

#endif // #ifndef CDSUNIT_SET_TYPE_SPLIT_LIST_H
