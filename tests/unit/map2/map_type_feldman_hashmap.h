//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_FELDMAN_HASHMAP_H
#define CDSUNIT_MAP_TYPE_FELDMAN_HASHMAP_H

#include "map2/map_type.h"

#include <cds/container/feldman_hashmap_hp.h>
#include <cds/container/feldman_hashmap_dhp.h>
#include <cds/container/feldman_hashmap_rcu.h>

#include "print_feldman_hashset_stat.h"
#include "hashing/hash_func.h"

namespace map2 {

    template <class GC, typename Key, typename T, typename Traits = cc::feldman_hashmap::traits>
    class FeldmanHashMap : public cc::FeldmanHashMap< GC, Key, T, Traits >
    {
        typedef cc::FeldmanHashMap< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        FeldmanHashMap( Config const& cfg)
            : base_class( cfg.c_nFeldmanMap_HeadBits, cfg.c_nFeldmanMap_ArrayBits )
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        static CDS_CONSTEXPR bool const c_bEraseExactKey = true;
    };

    struct tag_FeldmanHashMap;

    template <typename Key, typename Value>
    struct map_type< tag_FeldmanHashMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

        struct traits_FeldmanHashMap_stdhash : public cc::feldman_hashmap::traits
        {
            typedef std::hash< Key > hash;
        };

        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_hp_stdhash;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_dhp_stdhash;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_gpi_stdhash;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_gpb_stdhash;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_gpt_stdhash;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_shb_stdhash;
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_stdhash >    FeldmanHashMap_rcu_sht_stdhash;
#endif

        struct traits_FeldmanHashMap_stdhash_stat: traits_FeldmanHashMap_stdhash
        {
            typedef cc::feldman_hashmap::stat<> stat;
        };

        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_hp_stdhash_stat;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_dhp_stdhash_stat;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_gpi_stdhash_stat;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_gpb_stdhash_stat;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_gpt_stdhash_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_shb_stdhash_stat;
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_stdhash_stat >    FeldmanHashMap_rcu_sht_stdhash_stat;
#endif

        //MD5
        struct traits_FeldmanHashMap_md5 : public cc::feldman_hashmap::traits
        {
            typedef ::hashing::md5 hash;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_md5 >    FeldmanHashMap_hp_md5;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_md5 >    FeldmanHashMap_dhp_md5;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_md5 >    FeldmanHashMap_rcu_gpi_md5;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_md5 >    FeldmanHashMap_rcu_gpb_md5;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_md5 >    FeldmanHashMap_rcu_gpt_md5;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_md5 >    FeldmanHashMap_rcu_shb_md5;
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_md5 >    FeldmanHashMap_rcu_sht_md5;
#endif

        struct traits_FeldmanHashMap_md5_stat : public traits_FeldmanHashMap_md5
        {
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_md5_stat >    FeldmanHashMap_hp_md5_stat;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_md5_stat >    FeldmanHashMap_dhp_md5_stat;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_md5_stat >    FeldmanHashMap_rcu_gpi_md5_stat;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_md5_stat >    FeldmanHashMap_rcu_gpb_md5_stat;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_md5_stat >    FeldmanHashMap_rcu_gpt_md5_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_md5_stat >    FeldmanHashMap_rcu_shb_md5_stat;
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_md5_stat >    FeldmanHashMap_rcu_sht_md5_stat;
#endif

        // CityHash
#if CDS_BUILD_BITS == 64
        struct traits_FeldmanHashMap_city64 : public cc::feldman_hashmap::traits
        {
            typedef ::hashing::city64 hash;
            typedef ::hashing::city64::less less;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_hp_city64;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_dhp_city64;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_gpi_city64;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_gpb_city64;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_gpt_city64;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_shb_city64;
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_city64 >    FeldmanHashMap_rcu_sht_city64;
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
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_city64_stat >    FeldmanHashMap_rcu_sht_city64_stat;
#endif

        struct traits_FeldmanHashMap_city128 : public cc::feldman_hashmap::traits
        {
            typedef ::hashing::city128 hash;
            typedef ::hashing::city128::less less;
        };
        typedef FeldmanHashMap< cds::gc::HP,  Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_hp_city128;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_dhp_city128;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_gpi_city128;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_gpb_city128;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_gpt_city128;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_shb_city128;
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_city128 >    FeldmanHashMap_rcu_sht_city128;
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
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_city128_stat >    FeldmanHashMap_rcu_sht_city128_stat;
#endif
#endif // CDS_BUILD_BITS == 64


        // for fixed-sized keys - no hash functor required
        typedef FeldmanHashMap< cds::gc::HP, Key, Value >    FeldmanHashMap_hp_fixed;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value >   FeldmanHashMap_dhp_fixed;
        typedef FeldmanHashMap< rcu_gpi, Key, Value >    FeldmanHashMap_rcu_gpi_fixed;
        typedef FeldmanHashMap< rcu_gpb, Key, Value >    FeldmanHashMap_rcu_gpb_fixed;
        typedef FeldmanHashMap< rcu_gpt, Key, Value >    FeldmanHashMap_rcu_gpt_fixed;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value >    FeldmanHashMap_rcu_shb_fixed;
        typedef FeldmanHashMap< rcu_sht, Key, Value >    FeldmanHashMap_rcu_sht_fixed;
#endif

        struct traits_FeldmanHashMap_stat : public cc::feldman_hashmap::traits
        {
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef FeldmanHashMap< cds::gc::HP, Key, Value, traits_FeldmanHashMap_stat >    FeldmanHashMap_hp_fixed_stat;
        typedef FeldmanHashMap< cds::gc::DHP, Key, Value, traits_FeldmanHashMap_stat >   FeldmanHashMap_dhp_fixed_stat;
        typedef FeldmanHashMap< rcu_gpi, Key, Value, traits_FeldmanHashMap_stat >    FeldmanHashMap_rcu_gpi_fixed_stat;
        typedef FeldmanHashMap< rcu_gpb, Key, Value, traits_FeldmanHashMap_stat >    FeldmanHashMap_rcu_gpb_fixed_stat;
        typedef FeldmanHashMap< rcu_gpt, Key, Value, traits_FeldmanHashMap_stat >    FeldmanHashMap_rcu_gpt_fixed_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef FeldmanHashMap< rcu_shb, Key, Value, traits_FeldmanHashMap_stat >    FeldmanHashMap_rcu_shb_fixed_stat;
        typedef FeldmanHashMap< rcu_sht, Key, Value, traits_FeldmanHashMap_stat >    FeldmanHashMap_rcu_sht_fixed_stat;
#endif

    };

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( FeldmanHashMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() );

        std::vector< cds::intrusive::feldman_hashset::level_statistics > level_stat;
        m.get_level_statistics( level_stat );
        CPPUNIT_MSG( "Level statistics, height=" << level_stat.size());
        size_t i = 0;
        CPPUNIT_MSG( "  i   node_count capacity    data_cell   array_cell   empty_cell" );
        for (auto it = level_stat.begin(); it != level_stat.end(); ++it, ++i) {
            CPPUNIT_MSG( std::setw(3)  << i                    << std::setw(0) << " "
                      << std::setw(12) << it->array_node_count << std::setw(0) << " "
                      << std::setw(8)  << it->node_capacity    << std::setw(0) << " "
                      << std::setw(12) << it->data_cell_count  << std::setw(0) << " "
                      << std::setw(12) << it->array_cell_count << std::setw(0) << " "
                      << std::setw(12) << it->empty_cell_count << std::setw(0)
                      );
        }
    }

}   // namespace map2

#endif // #ifndef CDSUNIT_MAP_TYPE_FELDMAN_HASHMAP_H
