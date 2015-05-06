//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_BRONSON_AVLTREE_H
#define CDSUNIT_MAP_TYPE_BRONSON_AVLTREE_H

#include "map2/map_type.h"

#include <cds/memory/vyukov_queue_pool.h>
#include <cds/sync/pool_monitor.h>
#include <cds/container/bronson_avltree_map_rcu.h>

#include "print_bronsonavltree_stat.h"
#include "print_sync_monitor_stat.h"

namespace map2 {

    template <typename Key, typename Value>
    struct map_type< cc::bronson_avltree::implementation_tag, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

        typedef cds::memory::vyukov_queue_pool< std::mutex > BronsonAVLTreeMap_simple_pool;
        typedef cds::memory::lazy_vyukov_queue_pool< std::mutex > BronsonAVLTreeMap_lazy_pool;
        typedef cds::memory::bounded_vyukov_queue_pool< std::mutex > BronsonAVLTreeMap_bounded_pool;

        struct BronsonAVLTreeMap_less: public 
            cc::bronson_avltree::make_traits<
                co::less< less >
                ,cc::bronson_avltree::relaxed_insert< false >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_gpi_less;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_gpb_less;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_gpt_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_shb_less;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_less > BronsonAVLTreeMap_rcu_sht_less;
#endif
        struct BronsonAVLTreeMap_cmp_stat: public 
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,cc::bronson_avltree::relaxed_insert< false >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::bronson_avltree::stat<>>
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_gpi_cmp_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_gpb_cmp_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_gpt_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_shb_cmp_stat;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_cmp_stat > BronsonAVLTreeMap_rcu_sht_cmp_stat;
#endif

        struct BronsonAVLTreeMap_less_pool_simple: public BronsonAVLTreeMap_less
        {
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_simple_pool> sync_monitor;
        };
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_gpi_less_pool_simple;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_gpb_less_pool_simple;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_gpt_less_pool_simple;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_shb_less_pool_simple;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_less_pool_simple > BronsonAVLTreeMap_rcu_sht_less_pool_simple;
#endif
        struct BronsonAVLTreeMap_less_pool_simple_stat : public BronsonAVLTreeMap_less
        {
            typedef cc::bronson_avltree::stat<> stat;
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_simple_pool, cds::opt::none, true > sync_monitor;
        };
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_gpi_less_pool_simple_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_gpb_less_pool_simple_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_gpt_less_pool_simple_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_shb_less_pool_simple_stat;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_less_pool_simple_stat > BronsonAVLTreeMap_rcu_sht_less_pool_simple_stat;
#endif
        struct BronsonAVLTreeMap_less_pool_lazy: public BronsonAVLTreeMap_less
        {
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_lazy_pool> sync_monitor;
            static CDS_CONSTEXPR bool const relaxed_insert = true;
        };
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_gpi_less_pool_lazy;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_gpb_less_pool_lazy;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_gpt_less_pool_lazy;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_shb_less_pool_lazy;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_less_pool_lazy > BronsonAVLTreeMap_rcu_sht_less_pool_lazy;
#endif
        struct BronsonAVLTreeMap_less_pool_lazy_stat : public BronsonAVLTreeMap_less
        {
            typedef cc::bronson_avltree::stat<> stat;
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_lazy_pool, cds::opt::none, true > sync_monitor;
            static CDS_CONSTEXPR bool const relaxed_insert = true;
        };
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_gpi_less_pool_lazy_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_gpb_less_pool_lazy_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_gpt_less_pool_lazy_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_shb_less_pool_lazy_stat;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_less_pool_lazy_stat > BronsonAVLTreeMap_rcu_sht_less_pool_lazy_stat;
#endif
        struct BronsonAVLTreeMap_less_pool_bounded: public BronsonAVLTreeMap_less
        {
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_bounded_pool> sync_monitor;
            static CDS_CONSTEXPR bool const relaxed_insert = true;
        };
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_gpi_less_pool_bounded;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_gpb_less_pool_bounded;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_gpt_less_pool_bounded;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_shb_less_pool_bounded;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_less_pool_bounded > BronsonAVLTreeMap_rcu_sht_less_pool_bounded;
#endif
        struct BronsonAVLTreeMap_less_pool_bounded_stat : public BronsonAVLTreeMap_less
        {
            typedef cc::bronson_avltree::stat<> stat;
            typedef cds::sync::pool_monitor<BronsonAVLTreeMap_bounded_pool, cds::opt::none, true > sync_monitor;
            static CDS_CONSTEXPR bool const relaxed_insert = true;
        };
        typedef cc::BronsonAVLTreeMap< rcu_gpi, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_gpi_less_pool_bounded_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpb, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_gpb_less_pool_bounded_stat;
        typedef cc::BronsonAVLTreeMap< rcu_gpt, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_gpt_less_pool_bounded_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::BronsonAVLTreeMap< rcu_shb, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_shb_less_pool_bounded_stat;
        typedef cc::BronsonAVLTreeMap< rcu_sht, Key, Value, BronsonAVLTreeMap_less_pool_bounded_stat > BronsonAVLTreeMap_rcu_sht_less_pool_bounded_stat;
#endif
    };

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cc::BronsonAVLTreeMap<GC, Key, T, Traits> const& m )
    {
        CPPUNIT_MSG( m.statistics() );
        CPPUNIT_MSG( m.monitor().statistics() );
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void check_before_cleanup( cc::BronsonAVLTreeMap<GC, Key, T, Traits>& m )
    {
        CPPUNIT_MSG( "  Check internal consistency (single-threaded)..." );
        bool bOk = m.check_consistency([]( size_t nLevel, size_t hLeft, size_t hRight )
            { 
                CPPUNIT_MSG( "Tree violation on level=" << nLevel << ": hLeft=" << hLeft << ", hRight=" << hRight ) 
            });
        CPPUNIT_CHECK_CURRENT_EX( bOk, "check_consistency failed");
    }

}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_BRONSON_AVLTREE_H
