// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_BRONSON_AVLTREE_H
#define CDSUNIT_MAP_TYPE_BRONSON_AVLTREE_H

#include "map_type.h"

#include <cds/memory/vyukov_queue_pool.h>
#include <cds/sync/pool_monitor.h>
#include <cds/container/bronson_avltree_map_rcu.h>

#include <cds_test/stat_bronson_avltree_out.h>
#include <cds_test/stat_sync_monitor_out.h>

namespace map {

    template <class GC, typename Key, typename T, typename Traits = cc::bronson_avltree::traits >
    class BronsonAVLTreeMap : public cc::BronsonAVLTreeMap< GC, Key, T, Traits >
    {
        typedef cc::BronsonAVLTreeMap< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        BronsonAVLTreeMap( Config const& /*cfg*/ )
            : base_class()
        {}

        std::pair<Key, bool> extract_min_key()
        {
            Key key;
            typename base_class::exempt_ptr xp = base_class::extract_min( [&key]( Key const& k ) { key = k; } );
            if ( xp )
                return std::make_pair( key, true );
            return std::make_pair( key, false );
        }

        std::pair<Key, bool> extract_max_key()
        {
            Key key;
            typename base_class::exempt_ptr xp = base_class::extract_max( [&key]( Key const& k ) { key = k; } );
            if ( xp )
                return std::make_pair( key, true );
            return std::make_pair( key, false );
        }

        // for testing
        static constexpr bool const c_bExtractSupported = true;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = false;
    };

    struct tag_BronsonAVLTreeMap;

    template <typename Key, typename Value>
    struct map_type< tag_BronsonAVLTreeMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value >      base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;

        typedef cds::memory::vyukov_queue_pool< std::mutex >         BronsonAVLTreeMap_simple_pool;
        typedef cds::memory::lazy_vyukov_queue_pool< std::mutex >    BronsonAVLTreeMap_lazy_pool;
        typedef cds::memory::bounded_vyukov_queue_pool< std::mutex > BronsonAVLTreeMap_bounded_pool;

        struct BronsonAVLTreeMap_less: public
            cc::bronson_avltree::make_traits<
                co::less< less >
                ,cc::bronson_avltree::relaxed_insert< false >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_gpi_less;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_gpb_less;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_gpt_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_shb_less;
#endif
        struct BronsonAVLTreeMap_cmp_stat: public
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,cc::bronson_avltree::relaxed_insert< false >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
                ,co::stat< cc::bronson_avltree::stat<>>
            >::type
        {};
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_gpi_cmp_stat;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_gpb_cmp_stat;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_gpt_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_shb_cmp_stat;
#endif

        struct BronsonAVLTreeMap_less_pool_simple: public BronsonAVLTreeMap_less
        {
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_simple_pool> sync_monitor;
        };
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_gpi_less_pool_simple;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_gpb_less_pool_simple;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_gpt_less_pool_simple;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_shb_less_pool_simple;
#endif
        struct BronsonAVLTreeMap_less_pool_simple_stat : public BronsonAVLTreeMap_less
        {
            typedef cc::bronson_avltree::stat<> stat;
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_simple_pool, cds::opt::none, true > sync_monitor;
        };
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_gpi_less_pool_simple_stat;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_gpb_less_pool_simple_stat;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_gpt_less_pool_simple_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_shb_less_pool_simple_stat;
#endif
        struct BronsonAVLTreeMap_less_pool_lazy: public BronsonAVLTreeMap_less
        {
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_lazy_pool> sync_monitor;
            static constexpr bool const relaxed_insert = false; // relaxed insert can lead to test assert triggering
        };
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_gpi_less_pool_lazy;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_gpb_less_pool_lazy;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_gpt_less_pool_lazy;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_shb_less_pool_lazy;
#endif
        struct BronsonAVLTreeMap_less_pool_lazy_stat : public BronsonAVLTreeMap_less
        {
            typedef cc::bronson_avltree::stat<> stat;
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_lazy_pool, cds::opt::none, true > sync_monitor;
            static constexpr bool const relaxed_insert = false; // relaxed insert can lead to test assert triggering
        };
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_gpi_less_pool_lazy_stat;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_gpb_less_pool_lazy_stat;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_gpt_less_pool_lazy_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_shb_less_pool_lazy_stat;
#endif
        struct BronsonAVLTreeMap_less_pool_bounded: public BronsonAVLTreeMap_less
        {
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_bounded_pool> sync_monitor;
            static constexpr bool const relaxed_insert = false; // relaxed insert can lead to test assert triggering
        };
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_gpi_less_pool_bounded;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_gpb_less_pool_bounded;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_gpt_less_pool_bounded;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_shb_less_pool_bounded;
#endif
        struct BronsonAVLTreeMap_less_pool_bounded_stat : public BronsonAVLTreeMap_less
        {
            typedef cc::bronson_avltree::stat<> stat;
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_bounded_pool, cds::opt::none, true > sync_monitor;
            static constexpr bool const relaxed_insert = false; // relaxed insert can lead to test assert triggering
        };
        typedef BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_gpi_less_pool_bounded_stat;
        typedef BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_gpb_less_pool_bounded_stat;
        typedef BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_gpt_less_pool_bounded_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_shb_less_pool_bounded_stat;
#endif
    };

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, BronsonAVLTreeMap<GC, Key, T, Traits> const& m )
    {
        o << m.statistics()
          << m.monitor().statistics();
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void check_before_cleanup( BronsonAVLTreeMap<GC, Key, T, Traits>& m )
    {
        bool check_consistency_result = m.check_consistency([]( size_t nLevel, size_t hLeft, size_t hRight )
            {
                EXPECT_TRUE( false ) << "Tree violation on level=" << nLevel << ": hLeft=" << hLeft << ", hRight=" << hRight;
            });
        EXPECT_TRUE( check_consistency_result ) << "Internal tree structure violation";
    }


#define CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, bronson_map_type, key_type, value_type ) \
    TEST_F( fixture, bronson_map_type ) \
    { \
        typedef map::map_type< tag_BronsonAVLTreeMap, key_type, value_type >::bronson_map_type map_type; \
        test_case<map_type>(); \
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSSTRESS_BronsonAVLTreeMap_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_shb_less,                   key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_shb_cmp_stat,               key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_shb_less_pool_simple,       key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_shb_less_pool_simple_stat,  key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_shb_less_pool_lazy,         key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_shb_less_pool_lazy_stat,    key_type, value_type ) \

#else
#   define CDSSTRESS_BronsonAVLTreeMap_SHRCU( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 0
#   define CDSSTRESS_BronsonAVLTreeMap_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpi_cmp_stat,               key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpi_less_pool_simple,       key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpi_less_pool_simple_stat,  key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpi_less_pool_lazy,         key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpi_less_pool_lazy_stat,    key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpt_less_pool_simple,       key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpb_less_pool_simple_stat,  key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpb_less_pool_lazy,         key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpt_less_pool_lazy_stat,    key_type, value_type ) \
        CDSSTRESS_BronsonAVLTreeMap_SHRCU( fixture, test_case, key_type, value_type )

#else
#   define CDSSTRESS_BronsonAVLTreeMap_1( fixture, test_case, key_type, value_type )
#endif

#define CDSSTRESS_BronsonAVLTreeMap( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpi_less,                   key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpb_less,                   key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpt_less,                   key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpb_cmp_stat,               key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpt_cmp_stat,               key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpb_less_pool_simple,       key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpt_less_pool_simple_stat,  key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpt_less_pool_lazy,         key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_case( fixture, test_case, BronsonAVLTreeMap_rcu_gpb_less_pool_lazy_stat,    key_type, value_type ) \
    CDSSTRESS_BronsonAVLTreeMap_1( fixture, test_case, key_type, value_type ) \

}   // namespace map

#endif // ifndef CDSUNIT_MAP_TYPE_BRONSON_AVLTREE_H
