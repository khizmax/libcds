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

#ifndef CDSUNIT_SET_TYPE_MICHAEL_H
#define CDSUNIT_SET_TYPE_MICHAEL_H

#include "set_type_michael_list.h"
#include "set_type_lazy_list.h"
#include "set_type_iterable_list.h"

#include <cds/container/michael_set.h>
#include <cds/container/michael_set_rcu.h>

#include <cds_test/stat_michael_list_out.h>
#include <cds_test/stat_lazy_list_out.h>
#include <cds_test/stat_iterable_list_out.h>

namespace set {

    template <class GC, typename List, typename Traits = cc::michael_set::traits>
    class MichaelHashSet : public cc::MichaelHashSet< GC, List, Traits >
    {
        typedef cc::MichaelHashSet< GC, List, Traits > base_class;
    public:
        template <class Config>
        MichaelHashSet( Config const& cfg )
            : base_class( cfg.s_nSetSize, cfg.s_nLoadFactor )
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = true;
        static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
    };

    struct tag_MichaelHashSet;

    template <typename Key, typename Val>
    struct set_type< tag_MichaelHashSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;

        // ***************************************************************************
        // MichaelHashSet based on MichaelList

        typedef michael_list_type< Key, Val > ml;

        struct traits_MichaelSet :
            public cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        {};
        typedef MichaelHashSet< cds::gc::HP,  typename ml::MichaelList_HP_cmp,  traits_MichaelSet > MichaelSet_HP_cmp;
        typedef MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_cmp, traits_MichaelSet > MichaelSet_DHP_cmp;
        typedef MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_cmp, traits_MichaelSet > MichaelSet_RCU_GPI_cmp;
        typedef MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_cmp, traits_MichaelSet > MichaelSet_RCU_GPB_cmp;
        typedef MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_cmp, traits_MichaelSet > MichaelSet_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_cmp, traits_MichaelSet > MichaelSet_RCU_SHB_cmp;
        typedef MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_cmp, traits_MichaelSet > MichaelSet_RCU_SHT_cmp;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ml::MichaelList_HP_cmp_stat, traits_MichaelSet > MichaelSet_HP_cmp_stat;
        typedef MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_cmp_stat, traits_MichaelSet > MichaelSet_DHP_cmp_stat;
        typedef MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_cmp_stat, traits_MichaelSet > MichaelSet_RCU_GPI_cmp_stat;
        typedef MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_cmp_stat, traits_MichaelSet > MichaelSet_RCU_GPB_cmp_stat;
        typedef MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_cmp_stat, traits_MichaelSet > MichaelSet_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_cmp_stat, traits_MichaelSet > MichaelSet_RCU_SHB_cmp_stat;
        typedef MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_cmp_stat, traits_MichaelSet > MichaelSet_RCU_SHT_cmp_stat;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ml::MichaelList_HP_less, traits_MichaelSet > MichaelSet_HP_less;
        typedef MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_less, traits_MichaelSet > MichaelSet_DHP_less;
        typedef MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_less, traits_MichaelSet > MichaelSet_RCU_GPI_less;
        typedef MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_less, traits_MichaelSet > MichaelSet_RCU_GPB_less;
        typedef MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_less, traits_MichaelSet > MichaelSet_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_less, traits_MichaelSet > MichaelSet_RCU_SHB_less;
        typedef MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_less, traits_MichaelSet > MichaelSet_RCU_SHT_less;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ml::MichaelList_HP_less_stat, traits_MichaelSet > MichaelSet_HP_less_stat;
        typedef MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_less_stat, traits_MichaelSet > MichaelSet_DHP_less_stat;
        typedef MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_less_stat, traits_MichaelSet > MichaelSet_RCU_GPI_less_stat;
        typedef MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_less_stat, traits_MichaelSet > MichaelSet_RCU_GPB_less_stat;
        typedef MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_less_stat, traits_MichaelSet > MichaelSet_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_less_stat, traits_MichaelSet > MichaelSet_RCU_SHB_less_stat;
        typedef MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_less_stat, traits_MichaelSet > MichaelSet_RCU_SHT_less_stat;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ml::MichaelList_HP_less_seqcst, traits_MichaelSet > MichaelSet_HP_less_seqcst;
        typedef MichaelHashSet< cds::gc::DHP, typename ml::MichaelList_DHP_less_seqcst, traits_MichaelSet > MichaelSet_DHP_less_seqcst;
        typedef MichaelHashSet< rcu_gpi, typename ml::MichaelList_RCU_GPI_less_seqcst, traits_MichaelSet > MichaelSet_RCU_GPI_less_seqcst;
        typedef MichaelHashSet< rcu_gpb, typename ml::MichaelList_RCU_GPB_less_seqcst, traits_MichaelSet > MichaelSet_RCU_GPB_less_seqcst;
        typedef MichaelHashSet< rcu_gpt, typename ml::MichaelList_RCU_GPT_less_seqcst, traits_MichaelSet > MichaelSet_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ml::MichaelList_RCU_SHB_less_seqcst, traits_MichaelSet > MichaelSet_RCU_SHB_less_seqcst;
        typedef MichaelHashSet< rcu_sht, typename ml::MichaelList_RCU_SHT_less_seqcst, traits_MichaelSet > MichaelSet_RCU_SHT_less_seqcst;
#endif


        // ***************************************************************************
        // MichaelHashSet based on LazyList

        typedef lazy_list_type< Key, Val > ll;

        typedef MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_cmp, traits_MichaelSet > MichaelSet_Lazy_HP_cmp;
        typedef MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_cmp, traits_MichaelSet > MichaelSet_Lazy_DHP_cmp;
        typedef MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_cmp, traits_MichaelSet > MichaelSet_Lazy_RCU_GPI_cmp;
        typedef MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_cmp, traits_MichaelSet > MichaelSet_Lazy_RCU_GPB_cmp;
        typedef MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_cmp, traits_MichaelSet > MichaelSet_Lazy_RCU_GPT_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_cmp, traits_MichaelSet > MichaelSet_Lazy_RCU_SHB_cmp;
        typedef MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_cmp, traits_MichaelSet > MichaelSet_Lazy_RCU_SHT_cmp;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_cmp_stat, traits_MichaelSet > MichaelSet_Lazy_HP_cmp_stat;
        typedef MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_cmp_stat, traits_MichaelSet > MichaelSet_Lazy_DHP_cmp_stat;
        typedef MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_cmp_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_GPI_cmp_stat;
        typedef MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_cmp_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_GPB_cmp_stat;
        typedef MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_cmp_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_GPT_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_cmp_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_SHB_cmp_stat;
        typedef MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_cmp_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_SHT_cmp_stat;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_less, traits_MichaelSet > MichaelSet_Lazy_HP_less;
        typedef MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_less, traits_MichaelSet > MichaelSet_Lazy_DHP_less;
        typedef MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_less, traits_MichaelSet > MichaelSet_Lazy_RCU_GPI_less;
        typedef MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_less, traits_MichaelSet > MichaelSet_Lazy_RCU_GPB_less;
        typedef MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_less, traits_MichaelSet > MichaelSet_Lazy_RCU_GPT_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_less, traits_MichaelSet > MichaelSet_Lazy_RCU_SHB_less;
        typedef MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_less, traits_MichaelSet > MichaelSet_Lazy_RCU_SHT_less;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_less_stat, traits_MichaelSet > MichaelSet_Lazy_HP_less_stat;
        typedef MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_less_stat, traits_MichaelSet > MichaelSet_Lazy_DHP_less_stat;
        typedef MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_less_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_GPI_less_stat;
        typedef MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_less_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_GPB_less_stat;
        typedef MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_less_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_GPT_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_less_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_SHB_less_stat;
        typedef MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_less_stat, traits_MichaelSet > MichaelSet_Lazy_RCU_SHT_less_stat;
#endif

        typedef MichaelHashSet< cds::gc::HP, typename ll::LazyList_HP_less_seqcst, traits_MichaelSet > MichaelSet_Lazy_HP_less_seqcst;
        typedef MichaelHashSet< cds::gc::DHP, typename ll::LazyList_DHP_less_seqcst, traits_MichaelSet > MichaelSet_Lazy_DHP_less_seqcst;
        typedef MichaelHashSet< rcu_gpi, typename ll::LazyList_RCU_GPI_less_seqcst, traits_MichaelSet > MichaelSet_Lazy_RCU_GPI_less_seqcst;
        typedef MichaelHashSet< rcu_gpb, typename ll::LazyList_RCU_GPB_less_seqcst, traits_MichaelSet > MichaelSet_Lazy_RCU_GPB_less_seqcst;
        typedef MichaelHashSet< rcu_gpt, typename ll::LazyList_RCU_GPT_less_seqcst, traits_MichaelSet > MichaelSet_Lazy_RCU_GPT_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MichaelHashSet< rcu_shb, typename ll::LazyList_RCU_SHB_less_seqcst, traits_MichaelSet > MichaelSet_Lazy_RCU_SHB_less_seqcst;
        typedef MichaelHashSet< rcu_sht, typename ll::LazyList_RCU_SHT_less_seqcst, traits_MichaelSet > MichaelSet_Lazy_RCU_SHT_less_seqcst;
#endif


        // ***************************************************************************
        // MichaelHashSet based on IterableList

        typedef iterable_list_type< Key, Val > il;

        typedef MichaelHashSet< cds::gc::HP, typename il::IterableList_HP_cmp, traits_MichaelSet > MichaelSet_Iterable_HP_cmp;
        typedef MichaelHashSet< cds::gc::DHP, typename il::IterableList_DHP_cmp, traits_MichaelSet > MichaelSet_Iterable_DHP_cmp;

        typedef MichaelHashSet< cds::gc::HP, typename il::IterableList_HP_cmp_stat, traits_MichaelSet > MichaelSet_Iterable_HP_cmp_stat;
        typedef MichaelHashSet< cds::gc::DHP, typename il::IterableList_DHP_cmp_stat, traits_MichaelSet > MichaelSet_Iterable_DHP_cmp_stat;

        typedef MichaelHashSet< cds::gc::HP, typename il::IterableList_HP_less, traits_MichaelSet > MichaelSet_Iterable_HP_less;
        typedef MichaelHashSet< cds::gc::DHP, typename il::IterableList_DHP_less, traits_MichaelSet > MichaelSet_Iterable_DHP_less;

        typedef MichaelHashSet< cds::gc::HP, typename il::IterableList_HP_less_stat, traits_MichaelSet > MichaelSet_Iterable_HP_less_stat;
        typedef MichaelHashSet< cds::gc::DHP, typename il::IterableList_DHP_less_stat, traits_MichaelSet > MichaelSet_Iterable_DHP_less_stat;

        typedef MichaelHashSet< cds::gc::HP, typename il::IterableList_HP_less_seqcst, traits_MichaelSet > MichaelSet_Iterable_HP_less_seqcst;
        typedef MichaelHashSet< cds::gc::DHP, typename il::IterableList_DHP_less_seqcst, traits_MichaelSet > MichaelSet_Iterable_DHP_less_seqcst;

    };

    template <typename GC, typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, MichaelHashSet<GC, T, Traits> const& s )
    {
        o << s.statistics();
    }

} // namespace set


#define CDSSTRESS_MichaelSet_case( fixture, test_case, michael_set_type, key_type, value_type, level ) \
    TEST_P( fixture, michael_set_type ) \
    { \
        if ( !check_detail_level( level )) return; \
        typedef set::set_type< tag_MichaelHashSet, key_type, value_type >::michael_set_type set_type; \
        test_case<set_type>(); \
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSSTRESS_MichaelSet_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHB_cmp,             key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHT_cmp,             key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHB_cmp_stat,        key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHT_cmp_stat,        key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHB_less,            key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHT_less,            key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHB_less_stat,       key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_SHT_less_stat,       key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHB_cmp,        key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHT_cmp,        key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHB_cmp_stat,   key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHT_cmp_stat,   key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHB_less,       key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHT_less,       key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHB_less_stat,  key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_SHT_less_stat,  key_type, value_type, 1 ) \

#   define CDSSTRESS_MichaelIterableSet_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHB_cmp,        key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHT_cmp,        key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHB_cmp_stat,   key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHT_cmp_stat,   key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHB_less,       key_type, value_type, 1 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHT_less,       key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHB_less_stat,  key_type, value_type, 0 ) \
        CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_RCU_SHT_less_stat,  key_type, value_type, 1 ) \

#else
#   define CDSSTRESS_MichaelSet_SHRCU( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_MichaelIterableSet_SHRCU( fixture, test_case, key_type, value_type )
#endif


#define CDSSTRESS_MichaelSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_HP_cmp,                  key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_DHP_cmp,                 key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPI_cmp,             key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPB_cmp,             key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPT_cmp,             key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_HP_cmp_stat,             key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_DHP_cmp_stat,            key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPI_cmp_stat,        key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPB_cmp_stat,        key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPT_cmp_stat,        key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_HP_less,                 key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_DHP_less,                key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPI_less,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPB_less,            key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPT_less,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_HP_less_stat,            key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_DHP_less_stat,           key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPI_less_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPB_less_stat,       key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_RCU_GPT_less_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_HP_cmp,             key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_DHP_cmp,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPI_cmp,        key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPB_cmp,        key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPT_cmp,        key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_HP_cmp_stat,        key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_DHP_cmp_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPI_cmp_stat,   key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPB_cmp_stat,   key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPT_cmp_stat,   key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_HP_less,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_DHP_less,           key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPI_less,       key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPB_less,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPT_less,       key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_HP_less_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_DHP_less_stat,      key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPI_less_stat,  key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPB_less_stat,  key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Lazy_RCU_GPT_less_stat,  key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_SHRCU( fixture, test_case, key_type, value_type )

#define CDSSTRESS_MichaelIterableSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_HP_cmp,             key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_DHP_cmp,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_HP_cmp_stat,        key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_DHP_cmp_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_HP_less,            key_type, value_type, 1 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_DHP_less,           key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_HP_less_stat,       key_type, value_type, 0 ) \
    CDSSTRESS_MichaelSet_case( fixture, test_case, MichaelSet_Iterable_DHP_less_stat,      key_type, value_type, 1 ) \

#endif // #ifndef CDSUNIT_SET_TYPE_MICHAEL_H
