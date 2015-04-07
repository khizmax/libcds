//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_ELLEN_BINTREE_H
#define CDSUNIT_MAP_TYPE_ELLEN_BINTREE_H

#include "map2/map_type.h"

#include <cds/container/ellen_bintree_map_rcu.h>
#include <cds/container/ellen_bintree_map_hp.h>
#include <cds/container/ellen_bintree_map_dhp.h>

#include "ellen_bintree_update_desc_pool.h"
#include "print_ellenbintree_stat.h"

namespace map2 {

    template <typename Key, typename Value>
    struct map_type< cc::ellen_bintree::implementation_tag, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

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
            struct sht {
                typedef cc::ellen_bintree::map_node<rcu_sht, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#endif
        };

        struct traits_EllenBinTreeMap: public cc::ellen_bintree::make_set_traits<
                co::less< less >
                ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        struct traits_EllenBinTreeMap_hp : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_hp >EllenBinTreeMap_hp;

        struct traits_EllenBinTreeMap_dhp : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::DHP, Key, Value, traits_EllenBinTreeMap_dhp >EllenBinTreeMap_dhp;

        struct traits_EllenBinTreeMap_gpi : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpi, Key, Value, traits_EllenBinTreeMap_gpi >EllenBinTreeMap_rcu_gpi;

        struct traits_EllenBinTreeMap_gpb : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_gpb >EllenBinTreeMap_rcu_gpb;

        struct traits_EllenBinTreeMap_gpt : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpt, Key, Value, traits_EllenBinTreeMap_gpt >EllenBinTreeMap_rcu_gpt;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeMap_shb : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_shb, Key, Value, traits_EllenBinTreeMap_shb >EllenBinTreeMap_rcu_shb;

        struct traits_EllenBinTreeMap_sht : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_sht, Key, Value, traits_EllenBinTreeMap_sht >EllenBinTreeMap_rcu_sht;
#endif

        struct traits_EllenBinTreeMap_yield : public traits_EllenBinTreeMap
        {
            typedef cds::backoff::yield back_off;
        };
        struct traits_EllenBinTreeMap_hp_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_hp_yield >EllenBinTreeMap_hp_yield;

        struct traits_EllenBinTreeMap_dhp_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::DHP, Key, Value, traits_EllenBinTreeMap_dhp_yield >EllenBinTreeMap_dhp_yield;

        struct traits_EllenBinTreeMap_gpb_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_gpb_yield >EllenBinTreeMap_rcu_gpb_yield;


        struct traits_EllenBinTreeMap_stat: public cc::ellen_bintree::make_set_traits<
                co::less< less >
                ,cc::ellen_bintree::update_desc_allocator<
                    cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
                >
                ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
                ,co::stat< cc::ellen_bintree::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};

        struct traits_EllenBinTreeMap_stat_hp : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_stat_hp > EllenBinTreeMap_hp_stat;

        struct traits_EllenBinTreeMap_stat_dhp : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_stat_dhp > EllenBinTreeMap_dhp_stat;

        struct traits_EllenBinTreeMap_stat_gpi : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpi, Key, Value, traits_EllenBinTreeMap_stat_gpi > EllenBinTreeMap_rcu_gpi_stat;

        struct traits_EllenBinTreeMap_stat_gpb : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_stat_gpb > EllenBinTreeMap_rcu_gpb_stat;

        struct traits_EllenBinTreeMap_stat_gpt : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpt, Key, Value, traits_EllenBinTreeMap_stat_gpt > EllenBinTreeMap_rcu_gpt_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeMap_stat_shb : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_shb, Key, Value, traits_EllenBinTreeMap_stat_shb > EllenBinTreeMap_rcu_shb_stat;

        struct traits_EllenBinTreeMap_stat_sht : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_sht, Key, Value, traits_EllenBinTreeMap_stat_sht > EllenBinTreeMap_rcu_sht_stat;
#endif
    };

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cc::EllenBinTreeMap<GC, Key, T, Traits> const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }
    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_cleanup( cc::EllenBinTreeMap<GC, Key, T, Traits>& /*s*/ )
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
            CPPUNIT_CHECK_CURRENT_EX( stat.m_nInternalNodeCreated == stat.m_nInternalNodeDeleted,
                "m_nInternalNodeCreated=" << stat.m_nInternalNodeCreated
                << " m_nInternalNodeDeleted=" << stat.m_nInternalNodeDeleted );
            CPPUNIT_CHECK_CURRENT_EX( stat.m_nUpdateDescCreated == stat.m_nUpdateDescDeleted,
                "m_nUpdateDescCreated=" << stat.m_nUpdateDescCreated
                << " m_nUpdateDescDeleted=" << stat.m_nUpdateDescDeleted );
            CPPUNIT_CHECK_CURRENT_EX( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == stat.m_nInternalNodeCreated,
                "allocated=" << ellen_bintree_pool::internal_node_counter::m_nAlloc.get()
                << "m_nInternalNodeCreated=" << stat.m_nInternalNodeCreated );
        }
    }   // namespace ellen_bintree_check
    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_check( cc::EllenBinTreeMap<GC, Key, T, Traits>& s )
    {
        GC::force_dispose();
        ellen_bintree_check::check_stat( s.statistics() );
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void check_before_cleanup( cc::EllenBinTreeMap<GC, Key, T, Traits>& m )
    {
        CPPUNIT_MSG( "  Check internal consistency (single-threaded)..." );
        CPPUNIT_CHECK_CURRENT( m.check_consistency() );
    }
}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_ELLEN_BINTREE_H
