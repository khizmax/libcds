//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_SPLIT_LIST_H
#define CDSUNIT_MAP_TYPE_SPLIT_LIST_H

#include "map2/map_type.h"

#include <cds/container/michael_kvlist_hp.h>
#include <cds/container/michael_kvlist_dhp.h>
#include <cds/container/michael_kvlist_rcu.h>
#include <cds/container/michael_kvlist_nogc.h>

#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/lazy_kvlist_dhp.h>
#include <cds/container/lazy_kvlist_rcu.h>
#include <cds/container/lazy_kvlist_nogc.h>

#include <cds/container/split_list_map.h>
#include <cds/container/split_list_map_rcu.h>
#include <cds/container/split_list_map_nogc.h>

#include "print_split_list_stat.h"

namespace map2 {

    template <class GC, typename Key, typename T, typename Traits = cc::split_list::traits >
    class SplitListMap : public cc::SplitListMap< GC, Key, T, Traits >
    {
        typedef cc::SplitListMap< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        SplitListMap( Config const& cfg)
            : base_class( cfg.c_nMapSize, cfg.c_nLoadFactor )
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
    };

    template <typename Key, typename T, typename Traits >
    class SplitListMap< cds::gc::nogc, Key, T, Traits> : public cc::SplitListMap< cds::gc::nogc, Key, T, Traits >
    {
        typedef cc::SplitListMap< cds::gc::nogc, Key, T, Traits > base_class;
    public:
        template <typename Config>
        SplitListMap( Config const& cfg)
            : base_class( cfg.c_nMapSize, cfg.c_nLoadFactor )
        {}

        template <typename K>
        bool insert( K const& key )
        {
            return base_class::insert( key ) != base_class::end();
        }

        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            return base_class::insert( key, val ) != base_class::end();
        }

        template <typename K, typename Func>
        bool insert_with( K const& key, Func func )
        {
            return base_class::insert_with( key, func ) != base_class::end();
        }

        template <typename K>
        bool find( K const& key )
        {
            return base_class::find( key ) != base_class::end();
        }

        void clear()
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
    };

    struct tag_SplitListMap;

    template <typename Key, typename Value>
    struct map_type< tag_SplitListMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;
        typedef typename base_class::equal_to   equal_to;
        typedef typename base_class::key_hash   key_hash;
        typedef typename base_class::hash       hash;


        // ***************************************************************************
        // SplitListMap based on MichaelList
        struct traits_SplitList_Michael_dyn_cmp: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_HP_dyn_cmp;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_DHP_dyn_cmp;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_NOGC_dyn_cmp;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPI_dyn_cmp;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPB_dyn_cmp;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHB_dyn_cmp;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHT_dyn_cmp;
#endif

        struct traits_SplitList_Michael_dyn_cmp_stat : public traits_SplitList_Michael_dyn_cmp
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_HP_dyn_cmp_stat;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_DHP_dyn_cmp_stat;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_cmp_stat> SplitList_Michael_NOGC_dyn_cmp_stat;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPI_dyn_cmp_stat;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPB_dyn_cmp_stat;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHB_dyn_cmp_stat;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHT_dyn_cmp_stat;
#endif

        struct traits_SplitList_Michael_dyn_cmp_seqcst: public cc::split_list::make_traits<
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
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_HP_dyn_cmp_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_DHP_dyn_cmp_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst> SplitList_Michael_NOGC_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPI_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPB_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHB_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Michael_st_cmp: public cc::split_list::make_traits<
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
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_HP_st_cmp;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_DHP_st_cmp;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_cmp> SplitList_Michael_NOGC_st_cmp;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPI_st_cmp;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPB_st_cmp;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHB_st_cmp;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHT_st_cmp;
#endif

        struct traits_SplitList_Michael_st_cmp_seqcst: public cc::split_list::make_traits<
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
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_HP_st_cmp_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_DHP_st_cmp_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_NOGC_st_cmp_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_GPI_st_cmp_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_GPB_st_cmp_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_SHB_st_cmp_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_SHT_st_cmp_seqcst;
#endif

        //HP + less
        struct traits_SplitList_Michael_dyn_less: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_HP_dyn_less;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_DHP_dyn_less;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_less> SplitList_Michael_NOGC_dyn_less;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPI_dyn_less;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPB_dyn_less;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHB_dyn_less;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHT_dyn_less;
#endif


        struct traits_SplitList_Michael_dyn_less_seqcst: public cc::split_list::make_traits<
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
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_HP_dyn_less_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_DHP_dyn_less_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_less_seqcst> SplitList_Michael_NOGC_dyn_less_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPI_dyn_less_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPB_dyn_less_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHB_dyn_less_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHT_dyn_less_seqcst;
#endif

        struct traits_SplitList_Michael_st_less: public cc::split_list::make_traits<
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
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_HP_st_less;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_DHP_st_less;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_less> SplitList_Michael_NOGC_st_less;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPI_st_less;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPB_st_less;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHB_st_less;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHT_st_less;
#endif

        struct traits_SplitList_Michael_st_less_stat : traits_SplitList_Michael_st_less
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_HP_st_less_stat;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_DHP_st_less_stat;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_less_stat> SplitList_Michael_NOGC_st_less_stat;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPI_st_less_stat;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPB_st_less_stat;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHB_st_less_stat;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHT_st_less_stat;
#endif


        class traits_SplitList_Michael_st_less_seqcst: public cc::split_list::make_traits<
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
        typedef SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_HP_st_less_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_DHP_st_less_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_less_seqcst> SplitList_Michael_NOGC_st_less_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPI_st_less_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPB_st_less_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHB_st_less_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHT_st_less_seqcst;
#endif


        // ***************************************************************************
        // SplitListMap based on LazyKVList

        struct SplitList_Lazy_dyn_cmp :
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_HP_dyn_cmp;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_DHP_dyn_cmp;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_cmp> SplitList_Lazy_NOGC_dyn_cmp;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPI_dyn_cmp;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPB_dyn_cmp;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHT_dyn_cmp;
#endif

        struct SplitList_Lazy_dyn_cmp_stat : public SplitList_Lazy_dyn_cmp
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_HP_dyn_cmp_stat;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_DHP_dyn_cmp_stat;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_cmp_stat> SplitList_Lazy_NOGC_dyn_cmp_stat;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPI_dyn_cmp_stat;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPB_dyn_cmp_stat;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_SHB_dyn_cmp_stat;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_SHT_dyn_cmp_stat;
#endif

        struct SplitList_Lazy_dyn_cmp_seqcst :
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_HP_dyn_cmp_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_DHP_dyn_cmp_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_cmp_seqcst> SplitList_Lazy_NOGC_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPI_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPB_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHB_dyn_cmp_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct SplitList_Lazy_st_cmp :
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_HP_st_cmp;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_DHP_st_cmp;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_cmp> SplitList_Lazy_NOGC_st_cmp;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPI_st_cmp;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPB_st_cmp;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHB_st_cmp;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHT_st_cmp;
#endif


        struct SplitList_Lazy_st_cmp_seqcst :
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_HP_st_cmp_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_DHP_st_cmp_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_NOGC_st_cmp_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_GPI_st_cmp_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_GPB_st_cmp_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_SHB_st_cmp_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_SHT_st_cmp_seqcst;
#endif


        struct SplitList_Lazy_dyn_less :
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_HP_dyn_less;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_DHP_dyn_less;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_less> SplitList_Lazy_NOGC_dyn_less;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPI_dyn_less;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPB_dyn_less;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHB_dyn_less;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHT_dyn_less;
#endif

        struct SplitList_Lazy_dyn_less_seqcst:
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_HP_dyn_less_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_DHP_dyn_less_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_less_seqcst> SplitList_Lazy_NOGC_dyn_less_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPI_dyn_less_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPB_dyn_less_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHB_dyn_less_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHT_dyn_less_seqcst;
#endif

        struct SplitList_Lazy_st_less :
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_HP_st_less;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_DHP_st_less;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_less> SplitList_Lazy_NOGC_st_less;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPI_st_less;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPB_st_less;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHB_st_less;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHT_st_less;
#endif

        struct SplitList_Lazy_st_less_stat : public SplitList_Lazy_st_less
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_HP_st_less_stat;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_DHP_st_less_stat;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_less_stat> SplitList_Lazy_NOGC_st_less_stat;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPI_st_less_stat;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPB_st_less_stat;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHB_st_less_stat;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHT_st_less_stat;
#endif

        struct SplitList_Lazy_st_less_seqcst :
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
        typedef SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_HP_st_less_seqcst;
        typedef SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_DHP_st_less_seqcst;
        typedef SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_less_seqcst> SplitList_Lazy_NOGC_st_less_seqcst;
        typedef SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPI_st_less_seqcst;
        typedef SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPB_st_less_seqcst;
        typedef SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHB_st_less_seqcst;
        typedef SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHT_st_less_seqcst;
#endif
    };

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( SplitListMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() );
    }

}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_SPLIT_LIST_H
