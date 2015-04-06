//$$CDS-header$$

#ifndef CDSUNIT_SET_TYPE_ELLEN_BINTREE_H
#define CDSUNIT_SET_TYPE_ELLEN_BINTREE_H

#include "set2/set_type.h"

#include <cds/container/ellen_bintree_set_rcu.h>
#include <cds/container/ellen_bintree_set_hp.h>
#include <cds/container/ellen_bintree_set_dhp.h>

#include "print_ellenbintree_stat.h"

namespace set2 {

    template <typename Key, typename Val>
    struct set_type< cc::ellen_bintree::implementation_tag, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_type key_type;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::key_less key_less;

        struct ellen_bintree_props {
            struct key_extractor {
                void operator()( key_type& dest, key_val const& src ) const
                {
                    dest = src.key;
                }
            };

            struct less {
                bool operator()( key_val const& v1, key_val const& v2 ) const
                {
                    return key_less()( v1.key, v2.key );
                }
                bool operator()( key_type const& k, key_val const& v ) const
                {
                    return key_less()( k, v.key );
                }
                bool operator()( key_val const& v, key_type const& k ) const
                {
                    return key_less()( v.key, k );
                }
                bool operator()( key_type const& k1, key_type const& k2 ) const
                {
                    return key_less()( k1, k2 );
                }
            };

            struct hp_gc {
                typedef cc::ellen_bintree::node<cds::gc::HP, key_val>               leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };

            struct dhp_gc {
                typedef cc::ellen_bintree::node<cds::gc::DHP, key_val>              leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };

            struct gpi {
                typedef cc::ellen_bintree::node<rcu_gpi, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpb {
                typedef cc::ellen_bintree::node<rcu_gpb, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpt {
                typedef cc::ellen_bintree::node<rcu_gpt, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            struct shb {
                typedef cc::ellen_bintree::node<rcu_shb, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct sht {
                typedef cc::ellen_bintree::node<rcu_sht, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#endif
        };

        struct traits_EllenBinTreeSet: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};

        struct traits_EllenBinTreeSet_hp : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_hp > EllenBinTreeSet_hp;

        struct traits_EllenBinTreeSet_dhp : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_dhp > EllenBinTreeSet_dhp;

        struct traits_EllenBinTreeSet_gpi : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_gpi > EllenBinTreeSet_rcu_gpi;

        struct traits_EllenBinTreeSet_gpb : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_gpb > EllenBinTreeSet_rcu_gpb;

        struct traits_EllenBinTreeSet_gpt : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_gpt > EllenBinTreeSet_rcu_gpt;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeSet_shb : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_shb > EllenBinTreeSet_rcu_shb;

        struct traits_EllenBinTreeSet_sht : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_sht > EllenBinTreeSet_rcu_sht;
#endif

        //
        struct traits_EllenBinTreeSet_yield : public traits_EllenBinTreeSet
        {
            typedef cds::backoff::yield back_off;
        };

        struct traits_EllenBinTreeSet_yield_hp : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_yield_hp > EllenBinTreeSet_yield_hp;

        struct traits_EllenBinTreeSet_yield_dhp : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_yield_dhp > EllenBinTreeSet_yield_dhp;


        struct traits_EllenBinTreeSet_yield_gpb : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_yield_gpb > EllenBinTreeSet_yield_rcu_gpb;


        struct traits_EllenBinTreeSet_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};

        struct traits_EllenBinTreeSet_stat_hp : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_stat_hp > EllenBinTreeSet_hp_stat;

        struct traits_EllenBinTreeSet_stat_dhp : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_stat_dhp > EllenBinTreeSet_dhp_stat;

        struct traits_EllenBinTreeSet_stat_gpi : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_stat_gpi > EllenBinTreeSet_rcu_gpi_stat;

        struct traits_EllenBinTreeSet_stat_gpb : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_stat_gpb > EllenBinTreeSet_rcu_gpb_stat;

        struct traits_EllenBinTreeSet_stat_gpt : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_stat_gpt > EllenBinTreeSet_rcu_gpt_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeSet_stat_shb : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_stat_shb > EllenBinTreeSet_rcu_shb_stat;

        struct traits_EllenBinTreeSet_stat_sht : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_stat_sht > EllenBinTreeSet_rcu_sht_stat;
#endif

    };

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cc::EllenBinTreeSet<GC, Key, T, Traits> const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }

    namespace ellen_bintree_check {
        static inline void check_stat( cds::intrusive::ellen_bintree::empty_stat const& /*s*/ )
        {
            // Not true for threaded RCU
            /*
            CPPUNIT_CHECK_CURRENT_EX( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == ellen_bintree_pool::internal_node_counter::m_nFree.get(),
                "m_nAlloc=" << ellen_bintree_pool::internal_node_counter::m_nAlloc.get()
                << ", m_nFree=" << ellen_bintree_pool::internal_node_counter::m_nFree.get()
                );
            */
        }

        static inline void check_stat( cds::intrusive::ellen_bintree::stat<> const& stat )
        {
            CPPUNIT_CHECK_CURRENT( stat.m_nInternalNodeCreated == stat.m_nInternalNodeDeleted );
            CPPUNIT_CHECK_CURRENT( stat.m_nUpdateDescCreated == stat.m_nUpdateDescDeleted );
            //CPPUNIT_CHECK_CURRENT( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == ellen_bintree_pool::internal_node_counter::m_nFree.get() );
            CPPUNIT_CHECK_CURRENT( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == stat.m_nInternalNodeCreated );
            // true if RCU is not threaded
            //CPPUNIT_CHECK_CURRENT( stat.m_nInternalNodeDeleted == ellen_bintree_pool::internal_node_counter::m_nFree.get() );
        }
    }   // namespace ellen_bintree_check

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_check( cc::EllenBinTreeSet<GC, Key, T, Traits>& s )
    {
        GC::force_dispose();
        ellen_bintree_check::check_stat( s.statistics() );
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_cleanup( cc::EllenBinTreeSet<GC, Key, T, Traits>& /*s*/ )
    {
        ellen_bintree_pool::internal_node_counter::reset();
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void check_before_clear( cds::container::EllenBinTreeSet<GC, Key, T, Traits>& s )
    {
        CPPUNIT_CHECK_CURRENT( s.check_consistency() );
    }


} // namespace set2

#endif // #ifndef CDSUNIT_SET_TYPE_ELLEN_BINTREE_H
