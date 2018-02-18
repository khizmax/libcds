// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TYPE_SKIP_LIST_H
#define CDSUNIT_SET_TYPE_SKIP_LIST_H

#include "set_type.h"

#include <cds/container/skip_list_set_hp.h>
#include <cds/container/skip_list_set_dhp.h>
#include <cds/container/skip_list_set_rcu.h>

#include <cds_test/stat_skiplist_out.h>

namespace set {

    template <typename GC, typename T, typename Traits = cc::skip_list::traits >
    class SkipListSet : public cc::SkipListSet<GC, T, Traits>
    {
        typedef cc::SkipListSet<GC, T, Traits> base_class;
    public:
        template <typename Config>
        SkipListSet( Config const& /*cfg*/ )
        {}

        // for testing
        static constexpr bool const c_bExtractSupported = true;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = false;
    };

    struct tag_SkipListSet;

    template <typename Key, typename Val>
    struct set_type< tag_SkipListSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;

        class traits_SkipListSet_less_turbo32: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_turbo32 > SkipListSet_hp_less_turbo32;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_turbo32 > SkipListSet_dhp_less_turbo32;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_turbo32 > SkipListSet_rcu_gpi_less_turbo32;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_turbo32 > SkipListSet_rcu_gpb_less_turbo32;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_turbo32 > SkipListSet_rcu_gpt_less_turbo32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_turbo32 > SkipListSet_rcu_shb_less_turbo32;
#endif

        class traits_SkipListSet_less_turbo24: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo24 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_turbo24 > SkipListSet_hp_less_turbo24;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_turbo24 > SkipListSet_dhp_less_turbo24;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_turbo24 > SkipListSet_rcu_gpi_less_turbo24;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_turbo24 > SkipListSet_rcu_gpb_less_turbo24;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_turbo24 > SkipListSet_rcu_gpt_less_turbo24;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_turbo24 > SkipListSet_rcu_shb_less_turbo24;
#endif

        class traits_SkipListSet_less_turbo16: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo16 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_turbo16 > SkipListSet_hp_less_turbo16;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_turbo16 > SkipListSet_dhp_less_turbo16;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_turbo16 > SkipListSet_rcu_gpi_less_turbo16;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_turbo16 > SkipListSet_rcu_gpb_less_turbo16;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_turbo16 > SkipListSet_rcu_gpt_less_turbo16;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_turbo16 > SkipListSet_rcu_shb_less_turbo16;
#endif

        class traits_SkipListSet_less_turbo32_seqcst: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_turbo32_seqcst > SkipListSet_hp_less_turbo32_seqcst;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_turbo32_seqcst > SkipListSet_dhp_less_turbo32_seqcst;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_turbo32_seqcst > SkipListSet_rcu_gpi_less_turbo32_seqcst;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_turbo32_seqcst > SkipListSet_rcu_gpb_less_turbo32_seqcst;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_turbo32_seqcst > SkipListSet_rcu_gpt_less_turbo32_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_turbo32_seqcst > SkipListSet_rcu_shb_less_turbo32_seqcst;
#endif

        class traits_SkipListSet_less_turbo32_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_turbo32_stat > SkipListSet_hp_less_turbo32_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_turbo32_stat > SkipListSet_dhp_less_turbo32_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_turbo32_stat > SkipListSet_rcu_gpi_less_turbo32_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_turbo32_stat > SkipListSet_rcu_gpb_less_turbo32_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_turbo32_stat > SkipListSet_rcu_gpt_less_turbo32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_turbo32_stat > SkipListSet_rcu_shb_less_turbo32_stat;
#endif

        class traits_SkipListSet_less_turbo24_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo24 >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_turbo24_stat > SkipListSet_hp_less_turbo24_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_turbo24_stat > SkipListSet_dhp_less_turbo24_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_turbo24_stat > SkipListSet_rcu_gpi_less_turbo24_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_turbo24_stat > SkipListSet_rcu_gpb_less_turbo24_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_turbo24_stat > SkipListSet_rcu_gpt_less_turbo24_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_turbo24_stat > SkipListSet_rcu_shb_less_turbo24_stat;
#endif

        class traits_SkipListSet_less_turbo16_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo16 >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_turbo16_stat > SkipListSet_hp_less_turbo16_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_turbo16_stat > SkipListSet_dhp_less_turbo16_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_turbo16_stat > SkipListSet_rcu_gpi_less_turbo16_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_turbo16_stat > SkipListSet_rcu_gpb_less_turbo16_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_turbo16_stat > SkipListSet_rcu_gpt_less_turbo16_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_turbo16_stat > SkipListSet_rcu_shb_less_turbo16_stat;
#endif

        class traits_SkipListSet_cmp_turbo32: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_turbo32 > SkipListSet_hp_cmp_turbo32;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_turbo32 > SkipListSet_dhp_cmp_turbo32;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_turbo32 > SkipListSet_rcu_gpi_cmp_turbo32;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_turbo32 > SkipListSet_rcu_gpb_cmp_turbo32;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_turbo32 > SkipListSet_rcu_gpt_cmp_turbo32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_turbo32 > SkipListSet_rcu_shb_cmp_turbo32;
#endif

        class traits_SkipListSet_cmp_turbo32_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_turbo32_stat > SkipListSet_hp_cmp_turbo32_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_turbo32_stat > SkipListSet_dhp_cmp_turbo32_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_turbo32_stat > SkipListSet_rcu_gpi_cmp_turbo32_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_turbo32_stat > SkipListSet_rcu_gpb_cmp_turbo32_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_turbo32_stat > SkipListSet_rcu_gpt_cmp_turbo32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_turbo32_stat > SkipListSet_rcu_shb_cmp_turbo32_stat;
#endif

        class traits_SkipListSet_less_xorshift32: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift32 > SkipListSet_hp_less_xorshift32;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift32 > SkipListSet_dhp_less_xorshift32;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift32 > SkipListSet_rcu_gpi_less_xorshift32;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift32 > SkipListSet_rcu_gpb_less_xorshift32;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift32 > SkipListSet_rcu_gpt_less_xorshift32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift32 > SkipListSet_rcu_shb_less_xorshift32;
#endif

        class traits_SkipListSet_less_xorshift24: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift24 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift24 > SkipListSet_hp_less_xorshift24;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift24 > SkipListSet_dhp_less_xorshift24;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift24 > SkipListSet_rcu_gpi_less_xorshift24;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift24 > SkipListSet_rcu_gpb_less_xorshift24;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift24 > SkipListSet_rcu_gpt_less_xorshift24;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift24 > SkipListSet_rcu_shb_less_xorshift24;
#endif

        class traits_SkipListSet_less_xorshift16: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::xorshift16 >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift16 > SkipListSet_hp_less_xorshift16;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift16 > SkipListSet_dhp_less_xorshift16;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift16 > SkipListSet_rcu_gpi_less_xorshift16;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift16 > SkipListSet_rcu_gpb_less_xorshift16;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift16 > SkipListSet_rcu_gpt_less_xorshift16;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift16 > SkipListSet_rcu_shb_less_xorshift16;
#endif

        class traits_SkipListSet_less_xorshift32_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift32_stat > SkipListSet_hp_less_xorshift32_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift32_stat > SkipListSet_dhp_less_xorshift32_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift32_stat > SkipListSet_rcu_gpi_less_xorshift32_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift32_stat > SkipListSet_rcu_gpb_less_xorshift32_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift32_stat > SkipListSet_rcu_gpt_less_xorshift32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift32_stat > SkipListSet_rcu_shb_less_xorshift32_stat;
#endif

        class traits_SkipListSet_less_xorshift24_stat: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::xorshift24 >
            , co::stat< cc::skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift24_stat > SkipListSet_hp_less_xorshift24_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift24_stat > SkipListSet_dhp_less_xorshift24_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift24_stat > SkipListSet_rcu_gpi_less_xorshift24_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift24_stat > SkipListSet_rcu_gpb_less_xorshift24_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift24_stat > SkipListSet_rcu_gpt_less_xorshift24_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift24_stat > SkipListSet_rcu_shb_less_xorshift24_stat;
#endif

        class traits_SkipListSet_less_xorshift16_stat: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::xorshift16 >
            , co::stat< cc::skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift16_stat > SkipListSet_hp_less_xorshift16_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift16_stat > SkipListSet_dhp_less_xorshift16_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift16_stat > SkipListSet_rcu_gpi_less_xorshift16_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift16_stat > SkipListSet_rcu_gpb_less_xorshift16_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift16_stat > SkipListSet_rcu_gpt_less_xorshift16_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift16_stat > SkipListSet_rcu_shb_less_xorshift16_stat;
#endif

        class traits_SkipListSet_cmp_xorshift32: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_xorshift32 > SkipListSet_hp_cmp_xorshift32;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_xorshift32 > SkipListSet_dhp_cmp_xorshift32;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_xorshift32 > SkipListSet_rcu_gpi_cmp_xorshift32;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_xorshift32 > SkipListSet_rcu_gpb_cmp_xorshift32;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_xorshift32 > SkipListSet_rcu_gpt_cmp_xorshift32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_xorshift32 > SkipListSet_rcu_shb_cmp_xorshift32;
#endif

        class traits_SkipListSet_cmp_xorshift32_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_xorshift32_stat > SkipListSet_hp_cmp_xorshift32_stat;
        typedef SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_xorshift32_stat > SkipListSet_dhp_cmp_xorshift32_stat;
        typedef SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_xorshift32_stat > SkipListSet_rcu_gpi_cmp_xorshift32_stat;
        typedef SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_xorshift32_stat > SkipListSet_rcu_gpb_cmp_xorshift32_stat;
        typedef SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_xorshift32_stat > SkipListSet_rcu_gpt_cmp_xorshift32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_xorshift32_stat > SkipListSet_rcu_shb_cmp_xorshift32_stat;
#endif
    };

    template <typename GC, typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, SkipListSet<GC, T, Traits> const& s )
    {
        o << s.statistics();
    }

} // namespace set

#define CDSSTRESS_SkipListSet_case( fixture, test_case, skiplist_set_type, key_type, value_type ) \
    TEST_F( fixture, skiplist_set_type ) \
    { \
        typedef set::set_type< tag_SkipListSet, key_type, value_type >::skiplist_set_type set_type; \
        test_case<set_type>(); \
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 1
#   define CDSSTRESS_SkipListSet_SHRCU_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_less_turbo32_seqcst, key_type, value_type) \

#else
#   define CDSSTRESS_SkipListSet_SHRCU_2( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL == 1
#   define CDSSTRESS_SkipListSet_SHRCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_less_turbo32_stat,   key_type, value_type) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_cmp_turbo32,         key_type, value_type) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_less_xorshift32_stat, key_type, value_type) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_cmp_xorshift32_stat,  key_type, value_type) \

#else
#   define CDSSTRESS_SkipListSet_SHRCU_1( fixture, test_case, key_type, value_type )
#endif


#   define CDSSTRESS_SkipListSet_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_less_turbo32,        key_type, value_type) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_cmp_turbo32_stat,    key_type, value_type) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_less_xorshift32,      key_type, value_type) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_shb_cmp_xorshift32,       key_type, value_type) \
        CDSSTRESS_SkipListSet_SHRCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListSet_SHRCU_2( fixture, test_case, key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListSet_SHRCU( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 1
#   define CDSSTRESS_SkipListSet_HP_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_turbo32_seqcst,      key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_turbo32_seqcst,     key_type, value_type ) \

#   define CDSSTRESS_SkipListSet_RCU_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_less_turbo32_seqcst, key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_turbo32_seqcst, key_type, value_type ) \
        CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_less_turbo32_seqcst, key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListSet_HP_2( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SkipListSet_RCU_2( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL == 1
#   define CDSSTRESS_SkipListSet_HP_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_turbo32,            key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_turbo32_stat,        key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_cmp_turbo32,              key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_cmp_turbo32_stat,        key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_xorshift32,          key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_xorshift32_stat,      key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_cmp_xorshift32,            key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_cmp_xorshift32_stat,      key_type, value_type ) \

#   define CDSSTRESS_SkipListSet_RCU_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_turbo32,        key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_less_turbo32_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_less_turbo32_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_cmp_turbo32,         key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_cmp_turbo32,         key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_cmp_turbo32_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_xorshift32,      key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_less_xorshift32_stat, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_less_xorshift32_stat, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_cmp_xorshift32,       key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_cmp_xorshift32,       key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_cmp_xorshift32_stat,  key_type, value_type ) \
    CDSSTRESS_SkipListSet_SHRCU( fixture, test_case, key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListSet_HP_1( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SkipListSet_RCU_1( fixture, test_case, key_type, value_type )
#endif


#define CDSSTRESS_SkipListSet_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_turbo32,             key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_turbo24,             key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_turbo16,             key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_turbo32_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_turbo24_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_turbo16_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_cmp_turbo32,             key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_cmp_turbo32_stat,         key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_xorshift32,           key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_xorshift24,           key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_less_xorshift16,           key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_xorshift32_stat,     key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_xorshift24_stat,     key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_less_xorshift16_stat,     key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_dhp_cmp_xorshift32,           key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_hp_cmp_xorshift32_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListSet_HP_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_HP_2( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_SkipListSet_RCU( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_less_turbo32,        key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_less_turbo32,        key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_turbo32_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_turbo24_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_turbo16_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_cmp_turbo32,         key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_cmp_turbo32_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_cmp_turbo32_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_less_xorshift32,      key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_less_xorshift32,      key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_xorshift32_stat, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_xorshift24_stat, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_less_xorshift16_stat, key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpb_cmp_xorshift32,       key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpi_cmp_xorshift32_stat,  key_type, value_type ) \
    CDSSTRESS_SkipListSet_case( fixture, test_case, SkipListSet_rcu_gpt_cmp_xorshift32_stat,  key_type, value_type ) \
    CDSSTRESS_SkipListSet_RCU_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_RCU_2( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_SkipListSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListSet_RCU( fixture, test_case, key_type, value_type ) \

#endif // #ifndef CDSUNIT_SET_TYPE_SKIP_LIST_H
