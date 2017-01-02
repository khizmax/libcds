/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSUNIT_MAP_TYPE_MICHAEL_H
#define CDSUNIT_MAP_TYPE_MICHAEL_H

#include "map_type_michael_list.h"
#include "map_type_lazy_list.h"
#include "map_type_iterable_list.h"

#include <cds/container/michael_map.h>
#include <cds/container/michael_map_rcu.h>
#include <cds/container/michael_map_nogc.h>

namespace map {

    template <class GC, typename List, typename Traits = cc::michael_map::traits>
    class MichaelHashMap : public cc::MichaelHashMap< GC, List, Traits >
    {
        typedef cc::MichaelHashMap< GC, List, Traits > base_class;
    public:
        template <typename Config>
        MichaelHashMap( Config const& cfg)
            : base_class( cfg.s_nMapSize, cfg.s_nLoadFactor )
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
        static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
    };

    struct tag_MichaelHashMap;

    template <typename Key, typename Value>
    struct map_type< tag_MichaelHashMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;
        typedef typename base_class::equal_to    equal_to;
        typedef typename base_class::key_hash    hash;

        struct traits_MichaelMap_hash :
            public cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        {};

        // ***************************************************************************
        // MichaelHashMap based on MichaelKVList
        typedef michael_list_type< Key, Value > ml;

        typedef MichaelHashMap< cds::gc::HP,  typename ml::MichaelList_HP_cmp,  traits_MichaelMap_hash > MichaelMap_HP_cmp;
        typedef MichaelHashMap< cds::gc::DHP, typename ml::MichaelList_DHP_cmp, traits_MichaelMap_hash > MichaelMap_DHP_cmp;
        typedef MichaelHashMap< cds::gc::nogc, typename ml::MichaelList_NOGC_cmp, traits_MichaelMap_hash > MichaelMap_NOGC_cmp;
        typedef MichaelHashMap< rcu_gpi, typename ml::MichaelList_RCU_GPI_cmp, traits_MichaelMap_hash > MichaelMap_RCU_GPI_cmp;
        typedef MichaelHashMap< rcu_gpb, typename ml::MichaelList_RCU_GPB_cmp, traits_MichaelMap_hash > MichaelMap_RCU_GPB_cmp;
        typedef MichaelHashMap< rcu_gpt, typename ml::MichaelList_RCU_GPT_cmp, traits_MichaelMap_hash > MichaelMap_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ml::MichaelList_RCU_SHB_cmp, traits_MichaelMap_hash > MichaelMap_RCU_SHB_cmp;
        typedef MichaelHashMap< rcu_sht, typename ml::MichaelList_RCU_SHT_cmp, traits_MichaelMap_hash > MichaelMap_RCU_SHT_cmp;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ml::MichaelList_HP_cmp_stat, traits_MichaelMap_hash > MichaelMap_HP_cmp_stat;
        typedef MichaelHashMap< cds::gc::DHP, typename ml::MichaelList_DHP_cmp_stat, traits_MichaelMap_hash > MichaelMap_DHP_cmp_stat;
        typedef MichaelHashMap< cds::gc::nogc, typename ml::MichaelList_NOGC_cmp_stat, traits_MichaelMap_hash > MichaelMap_NOGC_cmp_stat;
        typedef MichaelHashMap< rcu_gpi, typename ml::MichaelList_RCU_GPI_cmp_stat, traits_MichaelMap_hash > MichaelMap_RCU_GPI_cmp_stat;
        typedef MichaelHashMap< rcu_gpb, typename ml::MichaelList_RCU_GPB_cmp_stat, traits_MichaelMap_hash > MichaelMap_RCU_GPB_cmp_stat;
        typedef MichaelHashMap< rcu_gpt, typename ml::MichaelList_RCU_GPT_cmp_stat, traits_MichaelMap_hash > MichaelMap_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ml::MichaelList_RCU_SHB_cmp_stat, traits_MichaelMap_hash > MichaelMap_RCU_SHB_cmp_stat;
        typedef MichaelHashMap< rcu_sht, typename ml::MichaelList_RCU_SHT_cmp_stat, traits_MichaelMap_hash > MichaelMap_RCU_SHT_cmp_stat;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ml::MichaelList_HP_less, traits_MichaelMap_hash > MichaelMap_HP_less;
        typedef MichaelHashMap< cds::gc::DHP, typename ml::MichaelList_DHP_less, traits_MichaelMap_hash > MichaelMap_DHP_less;
        typedef MichaelHashMap< cds::gc::nogc, typename ml::MichaelList_NOGC_less, traits_MichaelMap_hash > MichaelMap_NOGC_less;
        typedef MichaelHashMap< rcu_gpi, typename ml::MichaelList_RCU_GPI_less, traits_MichaelMap_hash > MichaelMap_RCU_GPI_less;
        typedef MichaelHashMap< rcu_gpb, typename ml::MichaelList_RCU_GPB_less, traits_MichaelMap_hash > MichaelMap_RCU_GPB_less;
        typedef MichaelHashMap< rcu_gpt, typename ml::MichaelList_RCU_GPT_less, traits_MichaelMap_hash > MichaelMap_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ml::MichaelList_RCU_SHB_less, traits_MichaelMap_hash > MichaelMap_RCU_SHB_less;
        typedef MichaelHashMap< rcu_sht, typename ml::MichaelList_RCU_SHT_less, traits_MichaelMap_hash > MichaelMap_RCU_SHT_less;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ml::MichaelList_HP_less_stat, traits_MichaelMap_hash > MichaelMap_HP_less_stat;
        typedef MichaelHashMap< cds::gc::DHP, typename ml::MichaelList_DHP_less_stat, traits_MichaelMap_hash > MichaelMap_DHP_less_stat;
        typedef MichaelHashMap< cds::gc::nogc, typename ml::MichaelList_NOGC_less_stat, traits_MichaelMap_hash > MichaelMap_NOGC_less_stat;
        typedef MichaelHashMap< rcu_gpi, typename ml::MichaelList_RCU_GPI_less_stat, traits_MichaelMap_hash > MichaelMap_RCU_GPI_less_stat;
        typedef MichaelHashMap< rcu_gpb, typename ml::MichaelList_RCU_GPB_less_stat, traits_MichaelMap_hash > MichaelMap_RCU_GPB_less_stat;
        typedef MichaelHashMap< rcu_gpt, typename ml::MichaelList_RCU_GPT_less_stat, traits_MichaelMap_hash > MichaelMap_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ml::MichaelList_RCU_SHB_less_stat, traits_MichaelMap_hash > MichaelMap_RCU_SHB_less_stat;
        typedef MichaelHashMap< rcu_sht, typename ml::MichaelList_RCU_SHT_less_stat, traits_MichaelMap_hash > MichaelMap_RCU_SHT_less_stat;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ml::MichaelList_HP_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_HP_cmp_seqcst;
        typedef MichaelHashMap< cds::gc::DHP, typename ml::MichaelList_DHP_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_DHP_cmp_seqcst;
        typedef MichaelHashMap< cds::gc::nogc, typename ml::MichaelList_NOGC_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_NOGC_cmp_seqcst;
        typedef MichaelHashMap< rcu_gpi, typename ml::MichaelList_RCU_GPI_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPI_cmp_seqcst;
        typedef MichaelHashMap< rcu_gpb, typename ml::MichaelList_RCU_GPB_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPB_cmp_seqcst;
        typedef MichaelHashMap< rcu_gpt, typename ml::MichaelList_RCU_GPT_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPT_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ml::MichaelList_RCU_SHB_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHB_cmp_seqcst;
        typedef MichaelHashMap< rcu_sht, typename ml::MichaelList_RCU_SHT_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHT_cmp_seqcst;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ml::MichaelList_HP_less_seqcst, traits_MichaelMap_hash > MichaelMap_HP_less_seqcst;
        typedef MichaelHashMap< cds::gc::DHP, typename ml::MichaelList_DHP_less_seqcst, traits_MichaelMap_hash > MichaelMap_DHP_less_seqcst;
        typedef MichaelHashMap< cds::gc::nogc, typename ml::MichaelList_NOGC_less_seqcst, traits_MichaelMap_hash > MichaelMap_NOGC_less_seqcst;
        typedef MichaelHashMap< rcu_gpi, typename ml::MichaelList_RCU_GPI_less_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPI_less_seqcst;
        typedef MichaelHashMap< rcu_gpb, typename ml::MichaelList_RCU_GPB_less_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPB_less_seqcst;
        typedef MichaelHashMap< rcu_gpt, typename ml::MichaelList_RCU_GPT_less_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ml::MichaelList_RCU_SHB_less_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHB_less_seqcst;
        typedef MichaelHashMap< rcu_sht, typename ml::MichaelList_RCU_SHT_less_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHT_less_seqcst;
#endif


        // ***************************************************************************
        // MichaelHashMap based on LazyKVList
        typedef lazy_list_type< Key, Value > ll;

        typedef MichaelHashMap< cds::gc::HP, typename ll::LazyList_HP_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_HP_cmp;
        typedef MichaelHashMap< cds::gc::DHP, typename ll::LazyList_DHP_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_cmp;
        typedef MichaelHashMap< cds::gc::nogc, typename ll::LazyList_NOGC_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_cmp;
        typedef MichaelHashMap< rcu_gpi, typename ll::LazyList_RCU_GPI_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_cmp;
        typedef MichaelHashMap< rcu_gpb, typename ll::LazyList_RCU_GPB_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_cmp;
        typedef MichaelHashMap< rcu_gpt, typename ll::LazyList_RCU_GPT_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ll::LazyList_RCU_SHB_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_cmp;
        typedef MichaelHashMap< rcu_sht, typename ll::LazyList_RCU_SHT_cmp, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_cmp;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ll::LazyList_HP_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_HP_cmp_stat;
        typedef MichaelHashMap< cds::gc::DHP, typename ll::LazyList_DHP_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_cmp_stat;
        typedef MichaelHashMap< cds::gc::nogc, typename ll::LazyList_NOGC_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_cmp_stat;
        typedef MichaelHashMap< rcu_gpi, typename ll::LazyList_RCU_GPI_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_cmp_stat;
        typedef MichaelHashMap< rcu_gpb, typename ll::LazyList_RCU_GPB_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_cmp_stat;
        typedef MichaelHashMap< rcu_gpt, typename ll::LazyList_RCU_GPT_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ll::LazyList_RCU_SHB_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_cmp_stat;
        typedef MichaelHashMap< rcu_sht, typename ll::LazyList_RCU_SHT_cmp_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_cmp_stat;
#endif

        typedef MichaelHashMap< cds::gc::nogc, typename ll::LazyList_NOGC_unord, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_unord;

        typedef MichaelHashMap< cds::gc::HP, typename ll::LazyList_HP_less, traits_MichaelMap_hash > MichaelMap_Lazy_HP_less;
        typedef MichaelHashMap< cds::gc::DHP, typename ll::LazyList_DHP_less, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_less;
        typedef MichaelHashMap< cds::gc::nogc, typename ll::LazyList_NOGC_less, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_less;
        typedef MichaelHashMap< rcu_gpi, typename ll::LazyList_RCU_GPI_less, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_less;
        typedef MichaelHashMap< rcu_gpb, typename ll::LazyList_RCU_GPB_less, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_less;
        typedef MichaelHashMap< rcu_gpt, typename ll::LazyList_RCU_GPT_less, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ll::LazyList_RCU_SHB_less, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_less;
        typedef MichaelHashMap< rcu_sht, typename ll::LazyList_RCU_SHT_less, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_less;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ll::LazyList_HP_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_HP_less_stat;
        typedef MichaelHashMap< cds::gc::DHP, typename ll::LazyList_DHP_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_less_stat;
        typedef MichaelHashMap< cds::gc::nogc, typename ll::LazyList_NOGC_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_less_stat;
        typedef MichaelHashMap< rcu_gpi, typename ll::LazyList_RCU_GPI_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_less_stat;
        typedef MichaelHashMap< rcu_gpb, typename ll::LazyList_RCU_GPB_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_less_stat;
        typedef MichaelHashMap< rcu_gpt, typename ll::LazyList_RCU_GPT_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ll::LazyList_RCU_SHB_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_less_stat;
        typedef MichaelHashMap< rcu_sht, typename ll::LazyList_RCU_SHT_less_stat, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_less_stat;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ll::LazyList_HP_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_HP_cmp_seqcst;
        typedef MichaelHashMap< cds::gc::DHP, typename ll::LazyList_DHP_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_cmp_seqcst;
        typedef MichaelHashMap< cds::gc::nogc, typename ll::LazyList_NOGC_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_cmp_seqcst;
        typedef MichaelHashMap< rcu_gpi, typename ll::LazyList_RCU_GPI_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_cmp_seqcst;
        typedef MichaelHashMap< rcu_gpb, typename ll::LazyList_RCU_GPB_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_cmp_seqcst;
        typedef MichaelHashMap< rcu_gpt, typename ll::LazyList_RCU_GPT_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ll::LazyList_RCU_SHB_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_cmp_seqcst;
        typedef MichaelHashMap< rcu_sht, typename ll::LazyList_RCU_SHT_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_cmp_seqcst;
#endif

        typedef MichaelHashMap< cds::gc::HP, typename ll::LazyList_HP_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_HP_less_seqcst;
        typedef MichaelHashMap< cds::gc::DHP, typename ll::LazyList_DHP_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_less_seqcst;
        typedef MichaelHashMap< cds::gc::nogc, typename ll::LazyList_NOGC_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_less_seqcst;
        typedef MichaelHashMap< rcu_gpi, typename ll::LazyList_RCU_GPI_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_less_seqcst;
        typedef MichaelHashMap< rcu_gpb, typename ll::LazyList_RCU_GPB_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_less_seqcst;
        typedef MichaelHashMap< rcu_gpt, typename ll::LazyList_RCU_GPT_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashMap< rcu_shb, typename ll::LazyList_RCU_SHB_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_less_seqcst;
        typedef MichaelHashMap< rcu_sht, typename ll::LazyList_RCU_SHT_less_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_less_seqcst;
#endif


        // ***************************************************************************
        // MichaelHashMap based on IterableKVList
        typedef iterable_list_type< Key, Value > il;

        typedef MichaelHashMap< cds::gc::HP,  typename il::IterableList_HP_cmp,  traits_MichaelMap_hash > MichaelMap_Iterable_HP_cmp;
        typedef MichaelHashMap< cds::gc::DHP, typename il::IterableList_DHP_cmp, traits_MichaelMap_hash > MichaelMap_Iterable_DHP_cmp;

        typedef MichaelHashMap< cds::gc::HP, typename il::IterableList_HP_cmp_stat, traits_MichaelMap_hash > MichaelMap_Iterable_HP_cmp_stat;
        typedef MichaelHashMap< cds::gc::DHP, typename il::IterableList_DHP_cmp_stat, traits_MichaelMap_hash > MichaelMap_Iterable_DHP_cmp_stat;

        typedef MichaelHashMap< cds::gc::HP, typename il::IterableList_HP_less, traits_MichaelMap_hash > MichaelMap_Iterable_HP_less;
        typedef MichaelHashMap< cds::gc::DHP, typename il::IterableList_DHP_less, traits_MichaelMap_hash > MichaelMap_Iterable_DHP_less;

        typedef MichaelHashMap< cds::gc::HP, typename il::IterableList_HP_less_stat, traits_MichaelMap_hash > MichaelMap_Iterable_HP_less_stat;
        typedef MichaelHashMap< cds::gc::DHP, typename il::IterableList_DHP_less_stat, traits_MichaelMap_hash > MichaelMap_Iterable_DHP_less_stat;

        typedef MichaelHashMap< cds::gc::HP, typename il::IterableList_HP_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Iterable_HP_cmp_seqcst;
        typedef MichaelHashMap< cds::gc::DHP, typename il::IterableList_DHP_cmp_seqcst, traits_MichaelMap_hash > MichaelMap_Iterable_DHP_cmp_seqcst;

    };
}   // namespace map

#define CDSSTRESS_MichaelMap_case( fixture, test_case, michael_map_type, key_type, value_type, level ) \
    TEST_P( fixture, michael_map_type ) \
    { \
        if ( !check_detail_level( level )) return; \
        typedef map::map_type< tag_MichaelHashMap, key_type, value_type >::michael_map_type map_type; \
        test_case<map_type>(); \
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSSTRESS_MichaelMap_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHB_cmp,              key_type, value_type, 0 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHT_cmp,              key_type, value_type, 1 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHB_less,             key_type, value_type, 1 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHT_less,             key_type, value_type, 0 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHB_cmp_seqcst,       key_type, value_type, 2 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHT_cmp_seqcst,       key_type, value_type, 2 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHB_less_seqcst,      key_type, value_type, 2 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_SHT_less_seqcst,      key_type, value_type, 2 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHB_cmp,         key_type, value_type, 0 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHT_cmp,         key_type, value_type, 1 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHB_less,        key_type, value_type, 1 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHT_less,        key_type, value_type, 0 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHB_cmp_seqcst,  key_type, value_type, 2 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHT_cmp_seqcst,  key_type, value_type, 2 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHB_less_seqcst, key_type, value_type, 2 ) \
        CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_SHT_less_seqcst, key_type, value_type, 2 ) \

#else
#   define CDSSTRESS_MichaelMap_SHRCU( fixture, test_case, key_type, value_type )
#endif

#define CDSSTRESS_MichaelMap( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_HP_cmp,                      key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_DHP_cmp,                     key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_HP_cmp_stat,                 key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_DHP_cmp_stat,                key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPI_cmp,                 key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPB_cmp,                 key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPT_cmp,                 key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_HP_less,                     key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_DHP_less,                    key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_HP_less_stat,                key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_DHP_less_stat,               key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPI_less,                key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPB_less,                key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPT_less,                key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_HP_cmp_seqcst,               key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_DHP_cmp_seqcst,              key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPI_cmp_seqcst,          key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPB_cmp_seqcst,          key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPT_cmp_seqcst,          key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_HP_less_seqcst,              key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_DHP_less_seqcst,             key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPI_less_seqcst,         key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPB_less_seqcst,         key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_RCU_GPT_less_seqcst,         key_type, value_type, 2 ) \
    \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_HP_cmp,                 key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_DHP_cmp,                key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_HP_cmp_stat,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_DHP_cmp_stat,           key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPI_cmp,            key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPB_cmp,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPT_cmp,            key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_HP_less,                key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_DHP_less,               key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_HP_less_stat,           key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_DHP_less_stat,          key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPI_less,           key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPB_less,           key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPT_less,           key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_HP_cmp_seqcst,          key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_DHP_cmp_seqcst,         key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPI_cmp_seqcst,     key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPB_cmp_seqcst,     key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPT_cmp_seqcst,     key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_HP_less_seqcst,         key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_DHP_less_seqcst,        key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPI_less_seqcst,    key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPB_less_seqcst,    key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_RCU_GPT_less_seqcst,    key_type, value_type, 2 ) \
    \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_HP_cmp,             key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_DHP_cmp,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_HP_cmp_stat,        key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_DHP_cmp_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_HP_less,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_DHP_less,           key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_HP_less_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_DHP_less_stat,      key_type, value_type, 1 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_HP_cmp_seqcst,      key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Iterable_DHP_cmp_seqcst,     key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_SHRCU( fixture, test_case, key_type, value_type )

#define CDSSTRESS_MichaelMap_nogc( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_NOGC_cmp,                    key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_NOGC_less,                   key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_NOGC_cmp_seqcst,             key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_NOGC_less_seqcst,            key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_NOGC_cmp,               key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_NOGC_unord,             key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_NOGC_less,              key_type, value_type, 0 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_NOGC_cmp_seqcst,        key_type, value_type, 2 ) \
    CDSSTRESS_MichaelMap_case( fixture, test_case, MichaelMap_Lazy_NOGC_less_seqcst,       key_type, value_type, 2 ) \


#endif // ifndef CDSUNIT_MAP_TYPE_MICHAEL_H
