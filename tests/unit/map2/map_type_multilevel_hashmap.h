//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_MULTILEVEL_HASHMAP_H
#define CDSUNIT_MAP_TYPE_MULTILEVEL_HASHMAP_H

#include "map2/map_type.h"

#include <cds/container/multilevel_hashmap_hp.h>
#include <cds/container/multilevel_hashmap_dhp.h>

#include "print_multilevel_hashset_stat.h"
#include "hashing/hash_func.h"

namespace map2 {

    template <class GC, typename Key, typename T, typename Traits = cc::multilevel_hashmap::traits>
    class MultiLevelHashMap : public cc::MultiLevelHashMap< GC, Key, T, Traits >
    {
        typedef cc::MultiLevelHashMap< GC, Key, T, Traits > base_class;
    public:
        template <typename Config>
        MultiLevelHashMap( Config const& cfg)
            : base_class( cfg.c_nMultiLevelMap_HeadBits, cfg.c_nMultiLevelMap_ArrayBits )
        {}

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
    };

    struct tag_MultiLevelHashMap;

    template <typename Key, typename Value>
    struct map_type< tag_MultiLevelHashMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value > base_class;
        typedef typename base_class::compare    compare;
        typedef typename base_class::less       less;

        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value >    MultiLevelHashMap_hp_stdhash;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value >    MultiLevelHashMap_dhp_stdhash;

        struct traits_MultiLevelHashMap_stat: public cc::multilevel_hashmap::make_traits<
                co::stat< cc::multilevel_hashmap::stat<>>
            >::type
        {};

        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_stat >    MultiLevelHashMap_hp_stdhash_stat;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_stat >    MultiLevelHashMap_dhp_stdhash_stat;

        // SHA256
        struct traits_MultiLevelHashMap_sha256 : public cc::multilevel_hashmap::traits
        {
            typedef ::hashing::sha256 hash;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_sha256 >    MultiLevelHashMap_hp_sha256;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_sha256 >    MultiLevelHashMap_dhp_sha256;

        struct traits_MultiLevelHashMap_sha256_stat : public traits_MultiLevelHashMap_sha256
        {
            typedef cc::multilevel_hashmap::stat<> stat;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_sha256_stat >    MultiLevelHashMap_hp_sha256_stat;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_sha256_stat >    MultiLevelHashMap_dhp_sha256_stat;

        //MD5
        struct traits_MultiLevelHashMap_md5 : public cc::multilevel_hashmap::traits
        {
            typedef ::hashing::md5 hash;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_md5 >    MultiLevelHashMap_hp_md5;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_md5 >    MultiLevelHashMap_dhp_md5;

        struct traits_MultiLevelHashMap_md5_stat : public traits_MultiLevelHashMap_md5
        {
            typedef cc::multilevel_hashmap::stat<> stat;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_md5_stat >    MultiLevelHashMap_hp_md5_stat;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_md5_stat >    MultiLevelHashMap_dhp_md5_stat;

        // CityHash
#if CDS_BUILD_BITS == 64
        struct traits_MultiLevelHashMap_city64 : public cc::multilevel_hashmap::traits
        {
            typedef ::hashing::city64 hash;
            typedef ::hashing::city64::less less;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_city64 >    MultiLevelHashMap_hp_city64;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_city64 >    MultiLevelHashMap_dhp_city64;

        struct traits_MultiLevelHashMap_city64_stat : public traits_MultiLevelHashMap_city64
        {
            typedef cc::multilevel_hashmap::stat<> stat;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_city64_stat >    MultiLevelHashMap_hp_city64_stat;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_city64_stat >    MultiLevelHashMap_dhp_city64_stat;

        struct traits_MultiLevelHashMap_city128 : public cc::multilevel_hashmap::traits
        {
            typedef ::hashing::city128 hash;
            typedef ::hashing::city128::less less;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_city128 >    MultiLevelHashMap_hp_city128;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_city128 >    MultiLevelHashMap_dhp_city128;

        struct traits_MultiLevelHashMap_city128_stat : public traits_MultiLevelHashMap_city128
        {
            typedef cc::multilevel_hashmap::stat<> stat;
        };
        typedef MultiLevelHashMap< cds::gc::HP,  Key, Value, traits_MultiLevelHashMap_city128_stat >    MultiLevelHashMap_hp_city128_stat;
        typedef MultiLevelHashMap< cds::gc::DHP, Key, Value, traits_MultiLevelHashMap_city128_stat >    MultiLevelHashMap_dhp_city128_stat;
#endif // CDS_BUILD_BITS == 64
    };

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cc::MultiLevelHashMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() );
    }

}   // namespace map2

#endif // #ifndef CDSUNIT_MAP_TYPE_MULTILEVEL_HASHMAP_H
