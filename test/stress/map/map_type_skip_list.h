// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_SKIP_LIST_H
#define CDSUNIT_MAP_TYPE_SKIP_LIST_H

#include "map_type.h"

#include <cds/container/skip_list_map_hp.h>
#include <cds/container/skip_list_map_dhp.h>
#include <cds/container/skip_list_map_rcu.h>
#include <cds/container/skip_list_map_nogc.h>

#include <cds_test/stat_skiplist_out.h>

namespace map {

    template <class GC, typename Key, typename T, typename Traits = cc::skip_list::traits >
    class SkipListMap : public cc::SkipListMap< GC, Key, T, Traits >
    {
        typedef cc::SkipListMap< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        SkipListMap( Config const& /*cfg*/)
            : base_class()
        {}

        std::pair<Key, bool> extract_min_key()
        {
            auto xp = base_class::extract_min();
            if ( xp )
                return std::make_pair( xp->first, true );
            return std::make_pair( Key(), false );
        }

        std::pair<Key, bool> extract_max_key()
        {
            auto xp = base_class::extract_max();
            if ( xp )
                return std::make_pair( xp->first, true );
            return std::make_pair( Key(), false );
        }

        // for testing
        static constexpr bool const c_bExtractSupported = true;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = false;
    };

    struct tag_SkipListMap;

    template <typename Key, typename Value>
    struct map_type< tag_SkipListMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value >      base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;

        class traits_SkipListMap_less_turbo32: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_turbo32 > SkipListMap_hp_less_turbo32;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_turbo32 > SkipListMap_dhp_less_turbo32;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_turbo32 > SkipListMap_nogc_less_turbo32;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_turbo32 > SkipListMap_rcu_gpi_less_turbo32;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_turbo32 > SkipListMap_rcu_gpb_less_turbo32;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_turbo32 > SkipListMap_rcu_gpt_less_turbo32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_turbo32 > SkipListMap_rcu_shb_less_turbo32;
#endif

        class traits_SkipListMap_less_turbo24: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::turbo24 >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_turbo24 > SkipListMap_hp_less_turbo24;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_turbo24 > SkipListMap_dhp_less_turbo24;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_turbo24 > SkipListMap_nogc_less_turbo24;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_turbo24 > SkipListMap_rcu_gpi_less_turbo24;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_turbo24 > SkipListMap_rcu_gpb_less_turbo24;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_turbo24 > SkipListMap_rcu_gpt_less_turbo24;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_turbo24 > SkipListMap_rcu_shb_less_turbo24;
#endif

        class traits_SkipListMap_less_turbo16: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::turbo16 >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_turbo16 > SkipListMap_hp_less_turbo16;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_turbo16 > SkipListMap_dhp_less_turbo16;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_turbo16 > SkipListMap_nogc_less_turbo16;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_turbo16 > SkipListMap_rcu_gpi_less_turbo16;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_turbo16 > SkipListMap_rcu_gpb_less_turbo16;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_turbo16 > SkipListMap_rcu_gpt_less_turbo16;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_turbo16 > SkipListMap_rcu_shb_less_turbo16;
#endif

        class traits_SkipListMap_less_turbo32_seqcst: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_turbo32_seqcst > SkipListMap_hp_less_turbo32_seqcst;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_turbo32_seqcst > SkipListMap_dhp_less_turbo32_seqcst;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_turbo32_seqcst > SkipListMap_nogc_less_turbo32_seqcst;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_turbo32_seqcst > SkipListMap_rcu_gpi_less_turbo32_seqcst;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_turbo32_seqcst > SkipListMap_rcu_gpb_less_turbo32_seqcst;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_turbo32_seqcst > SkipListMap_rcu_gpt_less_turbo32_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_turbo32_seqcst > SkipListMap_rcu_shb_less_turbo32_seqcst;
#endif

        class traits_SkipListMap_less_turbo32_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_turbo32_stat > SkipListMap_hp_less_turbo32_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_turbo32_stat > SkipListMap_dhp_less_turbo32_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_turbo32_stat > SkipListMap_nogc_less_turbo32_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_turbo32_stat > SkipListMap_rcu_gpi_less_turbo32_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_turbo32_stat > SkipListMap_rcu_gpb_less_turbo32_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_turbo32_stat > SkipListMap_rcu_gpt_less_turbo32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_turbo32_stat > SkipListMap_rcu_shb_less_turbo32_stat;
#endif

        class traits_SkipListMap_less_turbo24_stat: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::turbo24 >
            , co::stat< cc::skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_turbo24_stat > SkipListMap_hp_less_turbo24_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_turbo24_stat > SkipListMap_dhp_less_turbo24_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_turbo24_stat > SkipListMap_nogc_less_turbo24_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_turbo24_stat > SkipListMap_rcu_gpi_less_turbo24_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_turbo24_stat > SkipListMap_rcu_gpb_less_turbo24_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_turbo24_stat > SkipListMap_rcu_gpt_less_turbo24_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_turbo24_stat > SkipListMap_rcu_shb_less_turbo24_stat;
#endif

        class traits_SkipListMap_less_turbo16_stat: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::turbo16 >
            , co::stat< cc::skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_turbo16_stat > SkipListMap_hp_less_turbo16_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_turbo16_stat > SkipListMap_dhp_less_turbo16_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_turbo16_stat > SkipListMap_nogc_less_turbo16_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_turbo16_stat > SkipListMap_rcu_gpi_less_turbo16_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_turbo16_stat > SkipListMap_rcu_gpb_less_turbo16_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_turbo16_stat > SkipListMap_rcu_gpt_less_turbo16_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_turbo16_stat > SkipListMap_rcu_shb_less_turbo16_stat;
#endif

        class traits_SkipListMap_cmp_turbo32: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_turbo32 > SkipListMap_hp_cmp_turbo32;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_turbo32 > SkipListMap_dhp_cmp_turbo32;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_turbo32 > SkipListMap_nogc_cmp_turbo32;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_turbo32 > SkipListMap_rcu_gpi_cmp_turbo32;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_turbo32 > SkipListMap_rcu_gpb_cmp_turbo32;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_turbo32 > SkipListMap_rcu_gpt_cmp_turbo32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_turbo32 > SkipListMap_rcu_shb_cmp_turbo32;
#endif

        class traits_SkipListMap_cmp_turbo32_stat: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo32 >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_turbo32_stat > SkipListMap_hp_cmp_turbo32_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_turbo32_stat > SkipListMap_dhp_cmp_turbo32_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_turbo32_stat > SkipListMap_nogc_cmp_turbo32_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_turbo32_stat > SkipListMap_rcu_gpi_cmp_turbo32_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_turbo32_stat > SkipListMap_rcu_gpb_cmp_turbo32_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_turbo32_stat > SkipListMap_rcu_gpt_cmp_turbo32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_turbo32_stat > SkipListMap_rcu_shb_cmp_turbo32_stat;
#endif

        class traits_SkipListMap_less_xorshift32: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift32 > SkipListMap_hp_less_xorshift32;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift32 > SkipListMap_dhp_less_xorshift32;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift32 > SkipListMap_nogc_less_xorshift32;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift32 > SkipListMap_rcu_gpi_less_xorshift32;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift32 > SkipListMap_rcu_gpb_less_xorshift32;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift32 > SkipListMap_rcu_gpt_less_xorshift32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift32 > SkipListMap_rcu_shb_less_xorshift32;
#endif

        class traits_SkipListMap_less_xorshift24: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::xorshift24 >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift24 > SkipListMap_hp_less_xorshift24;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift24 > SkipListMap_dhp_less_xorshift24;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift24 > SkipListMap_nogc_less_xorshift24;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift24 > SkipListMap_rcu_gpi_less_xorshift24;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift24 > SkipListMap_rcu_gpb_less_xorshift24;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift24 > SkipListMap_rcu_gpt_less_xorshift24;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift24 > SkipListMap_rcu_shb_less_xorshift24;
#endif

        class traits_SkipListMap_less_xorshift16: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::xorshift16 >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift16 > SkipListMap_hp_less_xorshift16;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift16 > SkipListMap_dhp_less_xorshift16;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift16 > SkipListMap_nogc_less_xorshift16;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift16 > SkipListMap_rcu_gpi_less_xorshift16;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift16 > SkipListMap_rcu_gpb_less_xorshift16;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift16 > SkipListMap_rcu_gpt_less_xorshift16;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift16 > SkipListMap_rcu_shb_less_xorshift16;
#endif

        class traits_SkipListMap_less_xorshift32_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift32_stat > SkipListMap_hp_less_xorshift32_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift32_stat > SkipListMap_dhp_less_xorshift32_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift32_stat > SkipListMap_nogc_less_xorshift32_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift32_stat > SkipListMap_rcu_gpi_less_xorshift32_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift32_stat > SkipListMap_rcu_gpb_less_xorshift32_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift32_stat > SkipListMap_rcu_gpt_less_xorshift32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift32_stat > SkipListMap_rcu_shb_less_xorshift32_stat;
#endif

        class traits_SkipListMap_less_xorshift24_stat: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::xorshift24 >
            , co::stat< cc::skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift24_stat > SkipListMap_hp_less_xorshift24_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift24_stat > SkipListMap_dhp_less_xorshift24_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift24_stat > SkipListMap_nogc_less_xorshift24_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift24_stat > SkipListMap_rcu_gpi_less_xorshift24_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift24_stat > SkipListMap_rcu_gpb_less_xorshift24_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift24_stat > SkipListMap_rcu_gpt_less_xorshift24_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift24_stat > SkipListMap_rcu_shb_less_xorshift24_stat;
#endif

        class traits_SkipListMap_less_xorshift16_stat: public cc::skip_list::make_traits <
            co::less< less >
            , cc::skip_list::random_level_generator< cc::skip_list::xorshift16 >
            , co::stat< cc::skip_list::stat<> >
            , co::item_counter< cds::atomicity::cache_friendly_item_counter >
        >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift16_stat > SkipListMap_hp_less_xorshift16_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift16_stat > SkipListMap_dhp_less_xorshift16_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift16_stat > SkipListMap_nogc_less_xorshift16_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift16_stat > SkipListMap_rcu_gpi_less_xorshift16_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift16_stat > SkipListMap_rcu_gpb_less_xorshift16_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift16_stat > SkipListMap_rcu_gpt_less_xorshift16_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift16_stat > SkipListMap_rcu_shb_less_xorshift16_stat;
#endif

        class traits_SkipListMap_cmp_xorshift32: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_xorshift32 > SkipListMap_hp_cmp_xorshift32;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_xorshift32 > SkipListMap_dhp_cmp_xorshift32;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_xorshift32 > SkipListMap_nogc_cmp_xorshift32;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_xorshift32 > SkipListMap_rcu_gpi_cmp_xorshift32;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_xorshift32 > SkipListMap_rcu_gpb_cmp_xorshift32;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_xorshift32 > SkipListMap_rcu_gpt_cmp_xorshift32;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_xorshift32 > SkipListMap_rcu_shb_cmp_xorshift32;
#endif

        class traits_SkipListMap_cmp_xorshift32_stat: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift32 >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_xorshift32_stat > SkipListMap_hp_cmp_xorshift32_stat;
        typedef SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_xorshift32_stat > SkipListMap_dhp_cmp_xorshift32_stat;
        typedef SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_xorshift32_stat > SkipListMap_nogc_cmp_xorshift32_stat;
        typedef SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_xorshift32_stat > SkipListMap_rcu_gpi_cmp_xorshift32_stat;
        typedef SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_xorshift32_stat > SkipListMap_rcu_gpb_cmp_xorshift32_stat;
        typedef SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_xorshift32_stat > SkipListMap_rcu_gpt_cmp_xorshift32_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_xorshift32_stat > SkipListMap_rcu_shb_cmp_xorshift32_stat;
#endif

    };

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cds_test::property_stream& o, SkipListMap< GC, K, T, Traits > const& m )
    {
        o << m.statistics();
    }

}   // namespace map

#define CDSSTRESS_SkipListMap_case( fixture, test_case, skiplist_map_type, key_type, value_type ) \
    TEST_F( fixture, skiplist_map_type ) \
    { \
        typedef map::map_type< tag_SkipListMap, key_type, value_type >::skiplist_map_type map_type; \
        test_case<map_type>(); \
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 1
#   define CDSSTRESS_SkipListMap_SHRCU_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_less_turbo32_seqcst, key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListMap_SHRCU_2( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL == 1
#   define CDSSTRESS_SkipListMap_SHRCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_less_turbo32_stat,   key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_cmp_turbo32,         key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_cmp_turbo32_stat,    key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_less_xorshift32_stat, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_cmp_xorshift32,       key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListMap_SHRCU_1( fixture, test_case, key_type, value_type )
#endif


#   define CDSSTRESS_SkipListMap_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_less_turbo32,        key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_less_xorshift32,      key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_shb_cmp_xorshift32_stat,  key_type, value_type ) \
        CDSSTRESS_SkipListMap_SHRCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_SHRCU_2( fixture, test_case, key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListMap_SHRCU( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 1
#   define CDSSTRESS_SkipListMap_HP_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_turbo32_seqcst,      key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_turbo32_seqcst,     key_type, value_type ) \

#   define CDSSTRESS_SkipListMap_RCU_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_less_turbo32_seqcst, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_turbo32_seqcst, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_less_turbo32_seqcst, key_type, value_type ) \

#   define CDSSTRESS_SkipListMap_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_HP_2( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_RCU_2( fixture, test_case, key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListMap_HP_2( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SkipListMap_RCU_2( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SkipListMap_2( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL == 1
#   define CDSSTRESS_SkipListMap_HP_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_turbo32,            key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_turbo32_stat,        key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_cmp_turbo32,              key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_cmp_turbo32_stat,        key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_xorshift32,          key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_xorshift32_stat,      key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_cmp_xorshift32,            key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_cmp_xorshift32_stat,      key_type, value_type ) \

#   define CDSSTRESS_SkipListMap_RCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_turbo32,        key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_less_turbo32_stat,   key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_less_turbo32_stat,   key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_cmp_turbo32,         key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_cmp_turbo32,         key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_cmp_turbo32_stat,    key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_xorshift32,      key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_less_xorshift32_stat, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_less_xorshift32_stat, key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_cmp_xorshift32,       key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_cmp_xorshift32,       key_type, value_type ) \
        CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_cmp_xorshift32_stat,  key_type, value_type ) \
        CDSSTRESS_SkipListMap_SHRCU( fixture, test_case, key_type, value_type )

#   define CDSSTRESS_SkipListMap_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_HP_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_SkipListMap_RCU_1( fixture, test_case, key_type, value_type ) \

#else
#   define CDSSTRESS_SkipListMap_HP_1( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SkipListMap_RCU_1( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_SkipListMap_1( fixture, test_case, key_type, value_type )
#endif


#define CDSSTRESS_SkipListMap_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_turbo32,             key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_turbo24,             key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_turbo16,             key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_turbo32_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_turbo24_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_turbo16_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_cmp_turbo32,             key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_cmp_turbo32_stat,         key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_xorshift32,           key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_xorshift24,           key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_less_xorshift16,           key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_xorshift32_stat,     key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_xorshift24_stat,     key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_less_xorshift16_stat,     key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_dhp_cmp_xorshift32,           key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_hp_cmp_xorshift32_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListMap_HP_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListMap_HP_2( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_SkipListMap_RCU( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_less_turbo32,        key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_less_turbo32,        key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_turbo32_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_turbo24_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_turbo16_stat,   key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_cmp_turbo32,         key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_cmp_turbo32_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_cmp_turbo32_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_less_xorshift32,      key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_less_xorshift32,      key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_xorshift32_stat, key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_xorshift24_stat, key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_less_xorshift16_stat, key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpb_cmp_xorshift32,       key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpi_cmp_xorshift32_stat,  key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_rcu_gpt_cmp_xorshift32_stat,  key_type, value_type ) \
    CDSSTRESS_SkipListMap_RCU_1( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListMap_RCU_2( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_SkipListMap( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListMap_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListMap_RCU( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_SkipListMap_nogc( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_turbo32,           key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_turbo24,           key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_turbo16,           key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_turbo32_seqcst,    key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_turbo32_stat,      key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_turbo24_stat,      key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_turbo16_stat,      key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_cmp_turbo32,            key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_cmp_turbo32_stat,       key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_xorshift32,         key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_xorshift24,         key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_xorshift16,         key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_xorshift32_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_xorshift24_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_less_xorshift16_stat,    key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_cmp_xorshift32,          key_type, value_type ) \
    CDSSTRESS_SkipListMap_case( fixture, test_case, SkipListMap_nogc_cmp_xorshift32_stat,     key_type, value_type ) \

#endif // ifndef CDSUNIT_MAP_TYPE_SKIP_LIST_H
