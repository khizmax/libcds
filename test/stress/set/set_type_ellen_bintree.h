/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSUNIT_SET_TYPE_ELLEN_BINTREE_H
#define CDSUNIT_SET_TYPE_ELLEN_BINTREE_H

#include "set_type.h"

#include <cds/container/ellen_bintree_set_rcu.h>
#include <cds/container/ellen_bintree_set_hp.h>
#include <cds/container/ellen_bintree_set_dhp.h>

#include <cds_test/stat_ellenbintree_out.h>
#include "framework/ellen_bintree_update_desc_pool.h"

namespace set {

    template <class GC, typename Key, typename T, typename Traits = cc::ellen_bintree::traits >
    class EllenBinTreeSet : public cc::EllenBinTreeSet< GC, Key, T, Traits >
    {
        typedef cc::EllenBinTreeSet< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        EllenBinTreeSet( Config const& /*cfg*/ )
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
    };

    struct tag_EllenBinTreeSet;

    template <typename Key, typename Val>
    struct set_type< tag_EllenBinTreeSet, Key, Val >: public set_type_base< Key, Val >
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
        typedef EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_hp > EllenBinTreeSet_hp;

        struct traits_EllenBinTreeSet_dhp : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_dhp > EllenBinTreeSet_dhp;

        struct traits_EllenBinTreeSet_gpi : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_gpi > EllenBinTreeSet_rcu_gpi;

        struct traits_EllenBinTreeSet_gpb : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_gpb > EllenBinTreeSet_rcu_gpb;

        struct traits_EllenBinTreeSet_gpt : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_gpt > EllenBinTreeSet_rcu_gpt;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeSet_shb : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_shb > EllenBinTreeSet_rcu_shb;

        struct traits_EllenBinTreeSet_sht : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_sht > EllenBinTreeSet_rcu_sht;
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
        typedef EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_yield_hp > EllenBinTreeSet_yield_hp;

        struct traits_EllenBinTreeSet_yield_dhp : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_yield_dhp > EllenBinTreeSet_yield_dhp;


        struct traits_EllenBinTreeSet_yield_gpb : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_yield_gpb > EllenBinTreeSet_yield_rcu_gpb;


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
        typedef EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_stat_hp > EllenBinTreeSet_hp_stat;

        struct traits_EllenBinTreeSet_stat_dhp : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_stat_dhp > EllenBinTreeSet_dhp_stat;

        struct traits_EllenBinTreeSet_stat_gpi : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_stat_gpi > EllenBinTreeSet_rcu_gpi_stat;

        struct traits_EllenBinTreeSet_stat_gpb : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_stat_gpb > EllenBinTreeSet_rcu_gpb_stat;

        struct traits_EllenBinTreeSet_stat_gpt : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_stat_gpt > EllenBinTreeSet_rcu_gpt_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeSet_stat_shb : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_stat_shb > EllenBinTreeSet_rcu_shb_stat;

        struct traits_EllenBinTreeSet_stat_sht : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_stat_sht > EllenBinTreeSet_rcu_sht_stat;
#endif

    };

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cds_test::property_stream& o, EllenBinTreeSet<GC, Key, T, Traits> const& s )
    {
        o << s.statistics();
    }

    namespace ellen_bintree_check {
        static inline void check_stat( cds::intrusive::ellen_bintree::empty_stat const& /*s*/ )
        {
            // Not true for threaded RCU
            /*
            EXPECT_EQ( ellen_bintree_pool::internal_node_counter::m_nAlloc.get(), ellen_bintree_pool::internal_node_counter::m_nFree.get());
            */
        }
        static inline void check_stat( cds::intrusive::ellen_bintree::stat<> const& stat )
        {
            EXPECT_EQ( stat.m_nInternalNodeCreated, stat.m_nInternalNodeDeleted );
            EXPECT_EQ( stat.m_nUpdateDescCreated, stat.m_nUpdateDescDeleted );
            //EXPECT_EQ( ellen_bintree_pool::internal_node_counter::m_nAlloc.get(), ellen_bintree_pool::internal_node_counter::m_nFree.get() );
            EXPECT_EQ( ellen_bintree_pool::internal_node_counter::m_nAlloc.get(), stat.m_nInternalNodeCreated );
            // true if RCU is not threaded
            //EXPECT_EQ( stat.m_nInternalNodeDeleted, ellen_bintree_pool::internal_node_counter::m_nFree.get() );
        }
    }   // namespace ellen_bintree_check

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_check( EllenBinTreeSet<GC, Key, T, Traits>& s )
    {
        typedef EllenBinTreeSet<GC, Key, T, Traits> set_type;
        GC::force_dispose();
        ellen_bintree_check::check_stat( s.statistics());
/*
        bool const threaded_rcu = std::is_same<typename set_type::rcu_tag, cds::urcu::general_threaded_tag >::value
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            || std::is_same<typename set_type::rcu_tag, signal_threaded_tag >::value
#endif
        ;
        if ( !threaded_rcu ) {
            EXPECT_EQ( ellen_bintree_pool::internal_node_counter::m_nAlloc.get(), ellen_bintree_pool::internal_node_counter::m_nFree.get());
        }
*/
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_cleanup( EllenBinTreeSet<GC, Key, T, Traits>& /*s*/ )
    {
        ellen_bintree_pool::internal_node_counter::reset();
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void check_before_clear( cds::container::EllenBinTreeSet<GC, Key, T, Traits>& s )
    {
        EXPECT_TRUE( s.check_consistency() );
    }
} // namespace set

#define CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, ellen_set_type, key_type, value_type ) \
    TEST_F( fixture, ellen_set_type ) \
    { \
        typedef set::set_type< tag_EllenBinTreeSet, key_type, value_type >::ellen_set_type set_type; \
        test_case<set_type>(); \
    }

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSSTRESS_EllenBinTreeSet_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_shb,      key_type, value_type ) \
        CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_sht,      key_type, value_type ) \
        CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_shb_stat, key_type, value_type ) \
        CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_sht_stat, key_type, value_type )
#else
#   define CDSSTRESS_EllenBinTreeSet_SHRCU( fixture, test_case, key_type, value_type )
#endif


#define CDSSTRESS_EllenBinTreeSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_hp,             key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_dhp,            key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_gpi,        key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_gpb,        key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_gpt,        key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_yield_hp,       key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_yield_dhp,      key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_yield_rcu_gpb,  key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_hp_stat,        key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_dhp_stat,       key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_gpi_stat,   key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_gpb_stat,   key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_case( fixture, test_case, EllenBinTreeSet_rcu_gpt_stat,   key_type, value_type ) \
    CDSSTRESS_EllenBinTreeSet_SHRCU( fixture, test_case, key_type, value_type )

#endif // #ifndef CDSUNIT_SET_TYPE_ELLEN_BINTREE_H
