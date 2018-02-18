// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_ELLEN_BINTREE_H
#define CDSUNIT_MAP_TYPE_ELLEN_BINTREE_H

#include "map_type.h"

#include <cds/container/ellen_bintree_map_rcu.h>
#include <cds/container/ellen_bintree_map_hp.h>
#include <cds/container/ellen_bintree_map_dhp.h>

#include <cds_test/stat_ellenbintree_out.h>
#include "framework/ellen_bintree_update_desc_pool.h"

namespace map {

    template <class GC, typename Key, typename T, typename Traits = cc::ellen_bintree::traits >
    class EllenBinTreeMap : public cc::EllenBinTreeMap< GC, Key, T, Traits >
    {
        typedef cc::EllenBinTreeMap< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        EllenBinTreeMap( Config const& /*cfg*/)
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

    struct tag_EllenBinTreeMap;

    template <typename Key, typename Value>
    struct map_type< tag_EllenBinTreeMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value >      base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;

        struct ellen_bintree_props {
            struct hp_gc {
                typedef cc::ellen_bintree::map_node<cds::gc::HP, Key, Value>        leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct dhp_gc {
                typedef cc::ellen_bintree::map_node<cds::gc::DHP, Key, Value>       leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpi {
                typedef cc::ellen_bintree::map_node<rcu_gpi, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpb {
                typedef cc::ellen_bintree::map_node<rcu_gpb, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpt {
                typedef cc::ellen_bintree::map_node<rcu_gpt, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            struct shb {
                typedef cc::ellen_bintree::map_node<rcu_shb, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#endif
        };

        struct traits_EllenBinTreeMap: public cc::ellen_bintree::make_set_traits<
                co::less< less >
                ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};
        struct traits_EllenBinTreeMap_hp : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_hp > EllenBinTreeMap_hp;

        struct traits_EllenBinTreeMap_dhp : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< cds::gc::DHP, Key, Value, traits_EllenBinTreeMap_dhp > EllenBinTreeMap_dhp;

        struct traits_EllenBinTreeMap_gpi : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_gpi, Key, Value, traits_EllenBinTreeMap_gpi > EllenBinTreeMap_rcu_gpi;

        struct traits_EllenBinTreeMap_gpb : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_gpb > EllenBinTreeMap_rcu_gpb;

        struct traits_EllenBinTreeMap_gpt : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_gpt, Key, Value, traits_EllenBinTreeMap_gpt > EllenBinTreeMap_rcu_gpt;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeMap_shb : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_shb, Key, Value, traits_EllenBinTreeMap_shb > EllenBinTreeMap_rcu_shb;
#endif

        struct traits_EllenBinTreeMap_yield : public traits_EllenBinTreeMap
        {
            typedef cds::backoff::yield back_off;
        };
        struct traits_EllenBinTreeMap_hp_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_hp_yield > EllenBinTreeMap_hp_yield;

        struct traits_EllenBinTreeMap_dhp_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< cds::gc::DHP, Key, Value, traits_EllenBinTreeMap_dhp_yield > EllenBinTreeMap_dhp_yield;

        struct traits_EllenBinTreeMap_gpb_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_gpb_yield > EllenBinTreeMap_rcu_gpb_yield;


        struct traits_EllenBinTreeMap_stat: public cc::ellen_bintree::make_set_traits<
                co::less< less >
                ,cc::ellen_bintree::update_desc_allocator<
                    cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
                >
                ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
                ,co::stat< cc::ellen_bintree::stat<> >
                ,co::item_counter< cds::atomicity::cache_friendly_item_counter >
            >::type
        {};

        struct traits_EllenBinTreeMap_stat_hp : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_stat_hp > EllenBinTreeMap_hp_stat;

        struct traits_EllenBinTreeMap_stat_dhp : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_stat_dhp > EllenBinTreeMap_dhp_stat;

        struct traits_EllenBinTreeMap_stat_gpi : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_gpi, Key, Value, traits_EllenBinTreeMap_stat_gpi > EllenBinTreeMap_rcu_gpi_stat;

        struct traits_EllenBinTreeMap_stat_gpb : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_stat_gpb > EllenBinTreeMap_rcu_gpb_stat;

        struct traits_EllenBinTreeMap_stat_gpt : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_gpt, Key, Value, traits_EllenBinTreeMap_stat_gpt > EllenBinTreeMap_rcu_gpt_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeMap_stat_shb : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeMap< rcu_shb, Key, Value, traits_EllenBinTreeMap_stat_shb > EllenBinTreeMap_rcu_shb_stat;
#endif
    };

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, EllenBinTreeMap<GC, Key, T, Traits> const& s )
    {
        o << s.statistics();
    }
    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_cleanup( EllenBinTreeMap<GC, Key, T, Traits>& /*s*/ )
    {
        ellen_bintree_pool::internal_node_counter::reset();
    }
    namespace ellen_bintree_check {
        static inline void check_stat( cds::intrusive::ellen_bintree::empty_stat const& /*s*/ )
        {
            // This check is not valid for thread-based RCU
            /*
            CPPUNIT_CHECK_CURRENT_EX( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == ellen_bintree_pool::internal_node_counter::m_nFree.get(),
                "m_nAlloc=" << ellen_bintree_pool::internal_node_counter::m_nAlloc.get()
                << ", m_nFree=" << ellen_bintree_pool::internal_node_counter::m_nFree.get()
                );
            */
        }

        static inline void check_stat( cds::intrusive::ellen_bintree::stat<> const& stat )
        {
            EXPECT_EQ( stat.m_nInternalNodeCreated, stat.m_nInternalNodeDeleted );
            EXPECT_EQ( stat.m_nUpdateDescCreated, stat.m_nUpdateDescDeleted );
            EXPECT_EQ( ellen_bintree_pool::internal_node_counter::m_nAlloc.get(), stat.m_nInternalNodeCreated );
        }
    }   // namespace ellen_bintree_check
    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_check( EllenBinTreeMap<GC, Key, T, Traits>& m )
    {
        GC::force_dispose();
        ellen_bintree_check::check_stat( m.statistics());
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void check_before_cleanup( EllenBinTreeMap<GC, Key, T, Traits>& m )
    {
        EXPECT_TRUE( m.check_consistency());
    }
}   // namespace map


#define CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, ellen_map_type, key_type, value_type ) \
    TEST_F( fixture, ellen_map_type ) \
    { \
        typedef map::map_type< tag_EllenBinTreeMap, key_type, value_type >::ellen_map_type map_type; \
        test_case<map_type>(); \
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSSTRESS_EllenBinTreeMap_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_shb,        key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_shb_stat,   key_type, value_type ) \

#else
#   define CDSSTRESS_EllenBinTreeMap_SHRCU( fixture, test_case, key_type, value_type )
#endif

#if defined(CDS_STRESS_TEST_LEVEL) && CDS_STRESS_TEST_LEVEL > 0
#   define CDSSTRESS_EllenBinTreeMap_HP_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_hp_yield,       key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_dhp_yield,      key_type, value_type ) \


#   define CDSSTRESS_EllenBinTreeMap_RCU_1( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_gpi,        key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_gpb_yield,  key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_gpi_stat,   key_type, value_type ) \
        CDSSTRESS_EllenBinTreeMap_SHRCU( fixture, test_case, key_type, value_type ) \

#else
#   define CDSSTRESS_EllenBinTreeMap_HP_1( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_EllenBinTreeMap_RCU_1( fixture, test_case, key_type, value_type )
#endif

#define CDSSTRESS_EllenBinTreeMap_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_hp,             key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_dhp,            key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_hp_stat,        key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_dhp_stat,       key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_HP_1( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_EllenBinTreeMap_RCU( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_gpb,        key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_gpt,        key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_gpb_stat,   key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_case( fixture, test_case, EllenBinTreeMap_rcu_gpt_stat,   key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_RCU_1( fixture, test_case, key_type, value_type ) \

#define CDSSTRESS_EllenBinTreeMap( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_EllenBinTreeMap_RCU( fixture, test_case, key_type, value_type ) \

#endif // ifndef CDSUNIT_MAP_TYPE_ELLEN_BINTREE_H
