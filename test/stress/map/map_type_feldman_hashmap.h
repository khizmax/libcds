// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_FELDMAN_HASHMAP_H
#define CDSUNIT_MAP_TYPE_FELDMAN_HASHMAP_H

#include "map_type.h"

#include <cds/container/feldman_hashmap_hp.h>
#include <cds/container/feldman_hashmap_dhp.h>
#include <cds/container/feldman_hashmap_rcu.h>

#include <cds_test/stat_feldman_hashset_out.h>
#include <cds_test/hash_func.h>

namespace map {

    template <class GC, typename Key, typename T, typename Traits = cc::feldman_hashmap::traits>
    class FeldmanHashMap : public cc::FeldmanHashMap< GC, Key, T, Traits >
    {
        typedef cc::FeldmanHashMap< GC, Key, T, Traits > base_class;
    public:

        template <typename OtherTraits>
        struct rebind_traits {
            typedef FeldmanHashMap<GC, Key, T, OtherTraits > result;
        };

        template <typename Config>
        FeldmanHashMap( Config const& cfg)
            : base_class( cfg.s_nFeldmanMap_HeadBits, cfg.s_nFeldmanMap_ArrayBits )
        {}

        template <typename Iterator>
        typename std::enable_if< std::is_same< Iterator, typename base_class::iterator>::value, Iterator>::type
        get_begin()
        {
            return base_class::begin();
        }

        template <typename Iterator>
        typename std::enable_if< std::is_same< Iterator, typename base_class::iterator>::value, Iterator>::type
        get_end()
        {
            return base_class::end();
        }

        template <typename Iterator>
        typename std::enable_if< std::is_same< Iterator, typename base_class::reverse_iterator>::value, Iterator>::type
            get_begin()
        {
            return base_class::rbegin();
        }

        template <typename Iterator>
        typename std::enable_if< std::is_same< Iterator, typename base_class::reverse_iterator>::value, Iterator>::type
            get_end()
        {
            return base_class::rend();
        }

        // for testing
        static constexpr bool const c_bExtractSupported = true;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = true;
    };

    struct tag_FeldmanHashMap;

    template <typename Key, typename Value>
    struct map_type< tag_FeldmanHashMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value >      base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;

        struct traits_FeldmanHashMap_stdhash : public cc::feldman_hashmap::traits
        {
            typedef std::hash< Key >    hash;
            typedef std::less<size_t>   less;
            typedef cds::atomicity::cache_friendly_item_counter item_counter;
        };

        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_hp_stdhash;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_dhp_stdhash;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_gpi_stdhash;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_gpb_stdhash;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_gpt_stdhash;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_shb_stdhash;
#endif

        struct traits_FeldmanHashMap_stdhash_stat: traits_FeldmanHashMap_stdhash
        {
            typedef cc::feldman_hashmap::stat<> stat;
            typedef std::less<size_t>   less;
        };

        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_hp_stdhash_stat;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_dhp_stdhash_stat;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_gpi_stdhash_stat;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_gpb_stdhash_stat;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_gpt_stdhash_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_shb_stdhash_stat;
#endif

        // CityHash
#if CDS_BUILD_BITS == 64
        struct traits_FeldmanHashMap_city64 : public cc::feldman_hashmap::traits
        {
            typedef ::cds_test::city64 hash;
            typedef ::cds_test::city64::less less;
            typedef cds::atomicity::cache_friendly_item_counter item_counter;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_hp_city64;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_dhp_city64;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_gpi_city64;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_gpb_city64;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_gpt_city64;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_shb_city64;
#endif

        struct traits_FeldmanHashMap_city64_stat : public traits_FeldmanHashMap_city64
        {
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_city64_stat >    FeldmanHashMap_hp_city64_stat;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_city64_stat >    FeldmanHashMap_dhp_city64_stat;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_city64_stat >    FeldmanHashMap_rcu_gpi_city64_stat;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_city64_stat >    FeldmanHashMap_rcu_gpb_city64_stat;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_city64_stat >    FeldmanHashMap_rcu_gpt_city64_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_city64_stat >    FeldmanHashMap_rcu_shb_city64_stat;
#endif

        struct traits_FeldmanHashMap_city128 : public cc::feldman_hashmap::traits
        {
            typedef ::cds_test::city128 hash;
            typedef ::cds_test::city128::less less;
            typedef cds::atomicity::cache_friendly_item_counter item_counter;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_hp_city128;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_dhp_city128;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_gpi_city128;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_gpb_city128;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_gpt_city128;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_shb_city128;
#endif

        struct traits_FeldmanHashMap_city128_stat : public traits_FeldmanHashMap_city128
        {
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_city128_stat >    FeldmanHashMap_hp_city128_stat;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_city128_stat >    FeldmanHashMap_dhp_city128_stat;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_city128_stat >    FeldmanHashMap_rcu_gpi_city128_stat;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_city128_stat >    FeldmanHashMap_rcu_gpb_city128_stat;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_city128_stat >    FeldmanHashMap_rcu_gpt_city128_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_city128_stat >    FeldmanHashMap_rcu_shb_city128_stat;
#endif
#endif // CDS_BUILD_BITS == 64


        // for fixed-sized keys - no hash functor required
        struct traits_FeldmanHashMap_fixed: public cc::feldman_hashmap::traits
        {
            typedef map::cmp<Key>    compare;
            typedef cds::atomicity::cache_friendly_item_counter item_counter;
        };

        typedef FeldmanHashMap< cds::gc::HP, Key, Value,  traits_FeldmanHashMap_fixed >    FeldmanHashMap_hp_fixed;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_fixed >   FeldmanHashMap_dhp_fixed;
        typedef FeldmanHashMap< rcu_gpi, Key, Value,      traits_FeldmanHashMap_fixed >    FeldmanHashMap_rcu_gpi_fixed;
        typedef FeldmanHashMap< rcu_gpb, Key, Value,      traits_FeldmanHashMap_fixed >    FeldmanHashMap_rcu_gpb_fixed;
        typedef FeldmanHashMap< rcu_gpt, Key, Value,      traits_FeldmanHashMap_fixed >    FeldmanHashMap_rcu_gpt_fixed;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value,      traits_FeldmanHashMap_fixed >    FeldmanHashMap_rcu_shb_fixed;
#endif

        struct traits_FeldmanHashMap_fixed_stat : public traits_FeldmanHashMap_fixed
        {
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef FeldmanHashMap< cds::gc::HP, Key, Value,    traits_FeldmanHashMap_fixed_stat >  FeldmanHashMap_hp_fixed_stat;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value,   traits_FeldmanHashMap_fixed_stat >  FeldmanHashMap_dhp_fixed_stat;
        typedef FeldmanHashMap< rcu_gpi, Key, Value,        traits_FeldmanHashMap_fixed_stat >  FeldmanHashMap_rcu_gpi_fixed_stat;
        typedef FeldmanHashMap< rcu_gpb, Key, Value,        traits_FeldmanHashMap_fixed_stat >  FeldmanHashMap_rcu_gpb_fixed_stat;
        typedef FeldmanHashMap< rcu_gpt, Key, Value,        traits_FeldmanHashMap_fixed_stat >  FeldmanHashMap_rcu_gpt_fixed_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_fixed_stat > FeldmanHashMap_rcu_shb_fixed_stat;
#endif

    };

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cds_test::property_stream& o, FeldmanHashMap< GC, K, T, Traits > const& m )
    {
        std::vector< cds::intrusive::feldman_hashset::level_statistics > level_stat;
        m.get_level_statistics( level_stat );

        o << m.statistics()
          << level_stat;
    }

#define CDSSTRESS_FeldmanHashMap_case( fixture, test_case, feldman_map_type, key_type, value_type ) \
    TEST_F( fixture, feldman_map_type ) \
    { \
        typedef map::map_type< tag_FeldmanHashMap, key_type, value_type >::feldman_map_type map_type; \
        test_case<map_type>(); \
    }


#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSSTRESS_FeldmanHashMap_fixed_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_fixed, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_fixed_stat, key_type, value_type ) \

#   define CDSSTRESS_FeldmanHashMap_stdhash_SHRCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_stdhash, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_stdhash_stat, key_type, value_type ) \

#   if CDS_BUILD_BITS == 64
#       define CDSSTRESS_FeldmanHashMap_city64_SHRCU( fixture, test_case, key_type, value_type ) \
            CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_city64, key_type, value_type ) \
            CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_city64_stat, key_type, value_type ) \

#       define CDSSTRESS_FeldmanHashMap_city128_SHRCU( fixture, test_case, key_type, value_type ) \
            CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_city64, key_type, value_type ) \
            CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_shb_city64_stat, key_type, value_type ) \

#   else
#       define CDSSTRESS_FeldmanHashMap_city64_SHRCU( fixture, test_case, key_type, value_type )
#       define CDSSTRESS_FeldmanHashMap_city128_SHRCU( fixture, test_case, key_type, value_type )
#   endif

#else
#   define CDSSTRESS_FeldmanHashMap_fixed_SHRCU( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_stdhash_SHRCU( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city64_SHRCU( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city128_SHRCU( fixture, test_case, key_type, value_type )
#endif

#define CDSSTRESS_FeldmanHashMap_fixed_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_fixed,         key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_fixed,        key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_fixed_stat,    key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_fixed_stat,   key_type, value_type ) \

#define CDSSTRESS_FeldmanHashMap_fixed_RCU( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_fixed,        key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_fixed,        key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_fixed,        key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_fixed_stat,   key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_fixed_stat,   key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_fixed_stat,   key_type, value_type ) \

    //CDSSTRESS_FeldmanHashMap_fixed_SHRCU( fixture, test_case, key_type, value_type )

#define CDSSTRESS_FeldmanHashMap_fixed( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_fixed_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_fixed_RCU( fixture, test_case, key_type, value_type ) \


#define CDSSTRESS_FeldmanHashMap_stdhash_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_stdhash,       key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_stdhash,      key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_stdhash_stat,  key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_stdhash_stat, key_type, value_type ) \

#define CDSSTRESS_FeldmanHashMap_stdhash_RCU( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_stdhash,      key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_stdhash,      key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_stdhash,      key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_stdhash_stat, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_stdhash_stat, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_stdhash_stat, key_type, value_type ) \

    //CDSSTRESS_FeldmanHashMap_stdhash_SHRCU( fixture, test_case, key_type, value_type )

#define CDSSTRESS_FeldmanHashMap_stdhash( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_stdhash_HP( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_FeldmanHashMap_stdhash_RCU( fixture, test_case, key_type, value_type ) \


#if CDS_BUILD_BITS == 64
#   define CDSSTRESS_FeldmanHashMap_city64_HP( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_city64, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_city64, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_city64_stat, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_city64_stat, key_type, value_type ) \

#   define CDSSTRESS_FeldmanHashMap_city64_RCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_city64, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_city64, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_city64, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_city64_stat, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_city64_stat, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_city64_stat, key_type, value_type ) \

        //CDSSTRESS_FeldmanHashMap_city64_SHRCU( fixture, test_case, key_type, value_type )

#   define CDSSTRESS_FeldmanHashMap_city64( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city64_HP( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city64_RCU( fixture, test_case, key_type, value_type ) \


#   define CDSSTRESS_FeldmanHashMap_city128_HP( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_city128, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_city128, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_hp_city128_stat, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_dhp_city128_stat, key_type, value_type ) \

#   define CDSSTRESS_FeldmanHashMap_city128_RCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_city128, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_city128, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_city128, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpi_city128_stat, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpb_city128_stat, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_case( fixture, test_case, FeldmanHashMap_rcu_gpt_city128_stat, key_type, value_type ) \

        //CDSSTRESS_FeldmanHashMap_city128_SHRCU( fixture, test_case, key_type, value_type )

#   define CDSSTRESS_FeldmanHashMap_city128( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city128_HP( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city128_RCU( fixture, test_case, key_type, value_type ) \

#   define CDSSTRESS_FeldmanHashMap_city_HP( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city64_HP( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city128_HP( fixture, test_case, key_type, value_type )

#   define CDSSTRESS_FeldmanHashMap_city_RCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city64_RCU( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city128_RCU( fixture, test_case, key_type, value_type )

#   define CDSSTRESS_FeldmanHashMap_city( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city_HP( fixture, test_case, key_type, value_type ) \
        CDSSTRESS_FeldmanHashMap_city_RCU( fixture, test_case, key_type, value_type )

#else
#   define CDSSTRESS_FeldmanHashMap_city64_HP( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city64_RCU( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city64( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city128_HP( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city128_RCU( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city128( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city_HP( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city_RCU( fixture, test_case, key_type, value_type )
#   define CDSSTRESS_FeldmanHashMap_city( fixture, test_case, key_type, value_type )
#endif


}   // namespace map

#endif // #ifndef CDSUNIT_MAP_TYPE_FELDMAN_HASHMAP_H
