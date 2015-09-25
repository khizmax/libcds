//$$CDS-header$$

#ifndef CDSUNIT_SET_TYPE_MICHAEL_H
#define CDSUNIT_SET_TYPE_MICHAEL_H

#include <cds/container/multilevel_hashset_hp.h>
#include <cds/container/multilevel_hashset_dhp.h>

#include "print_multilevel_hashset_stat.h"
#include "hashing/hash_func.h"

namespace set2 {

    template <class GC, typename T, typename Traits = cc::multilevel_hashset::traits>
    class MultiLevelHashSet : public cc::MultiLevelHashSet< GC, T, Traits >
    {
        typedef cc::MultiLevelHashSet< GC, T, Traits > base_class;
    public:
        typedef typename T::hasher hasher ;
        typedef typename base_class::guarded_ptr guarded_ptr;

        template <class Config>
        MultiLevelHashSet( Config const& cfg )
            : base_class( cfg.c_nMultiLevelSet_HeadBits, cfg.c_nMultiLevelSet_ArrayBits )
        {}

        template <typename Q>
        bool erase( Q const& key )
        {
            return base_class::erase( hasher()( key ));
        }

        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return base_class::erase( hasher()( key ), f );
        }

        template <typename Q>
        guarded_ptr extract(Q const& key)
        {
            return base_class::extract( hasher()(key) );
        }

        template <typename Q>
        bool contains( Q const& key )
        {
            return base_class::contains( hasher()(key) );
        }

        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = true;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
    };

    struct tag_MultiLevelHashSet;

    template <typename Key, typename Val>
    struct set_type< tag_MultiLevelHashSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::compare compare;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;
        typedef typename base_class::key_type   key_type;
        typedef typename base_class::value_type value_type;

        template <typename Hasher>
        struct hash_type
        {
            typedef Hasher hasher;
            typedef typename hasher::hash_type type;
        };

        template <typename TH>
        struct hash_type<std::hash<TH>>
        {
            typedef std::hash<TH> hasher;
            typedef size_t type;
        };

        template <typename Hasher>
        struct key_val: base_class::key_val
        {
            typedef typename base_class::key_val base;
            typedef Hasher hasher;
            typedef typename hash_type<hasher>::type hash_type;

            hash_type hash;

            /*explicit*/ key_val( key_type const& k ): base(k), hash( hasher()( k )) {}
            key_val( key_type const& k, value_type const& v ): base(k, v), hash( hasher()( k )) {}

            template <typename K>
            /*explicit*/ key_val( K const& k ): base(k), hash( hasher()( k )) {}

            template <typename K, typename T>
            key_val( K const& k, T const& v ): base(k, v), hash( hasher()( k )) {}
        };

        struct default_traits : public cc::multilevel_hashset::traits
        {
            struct hash_accessor {
                template <typename Hasher>
                typename key_val<Hasher>::hash_type const& operator()( key_val<Hasher> const& kv )
                {
                    return kv.hash;
                }
            };
        };

        typedef MultiLevelHashSet< cds::gc::HP,  key_val<std::hash<key_type>>, default_traits >    MultiLevelHashSet_hp_stdhash;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<std::hash<key_type>>, default_traits >    MultiLevelHashSet_dhp_stdhash;

        struct traits_MultiLevelHashSet_stat: public cc::multilevel_hashset::make_traits<
                co::type_traits< default_traits >,
                co::stat< cc::multilevel_hashset::stat<>>
            >::type
        {};

        typedef MultiLevelHashSet< cds::gc::HP,  key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_hp_stdhash_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_dhp_stdhash_stat;

        // SHA256
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_hp_sha256;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_dhp_sha256;

        struct traits_MultiLevelHashSet_sha256_stat : public default_traits
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_hp_sha256_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_dhp_sha256_stat;

        //MD5
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_hp_md5;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_dhp_md5;

        struct traits_MultiLevelHashSet_md5_stat : public default_traits
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_hp_md5_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_dhp_md5_stat;

        // CityHash
#if CDS_BUILD_BITS == 64
        struct traits_MultiLevelHashSet_city64 : public default_traits
        {
            typedef ::hashing::city64::less less;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_hp_city64;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_dhp_city64;

        struct traits_MultiLevelHashSet_city64_stat : public traits_MultiLevelHashSet_city64
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_hp_city64_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_dhp_city64_stat;

        struct traits_MultiLevelHashSet_city128 : public default_traits
        {
            typedef ::hashing::city128::less less;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_hp_city128;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_dhp_city128;

        struct traits_MultiLevelHashSet_city128_stat : public traits_MultiLevelHashSet_city128
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_hp_city128_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_dhp_city128_stat;
#endif // #if CDS_BUILD_BITS == 64
    };

    template <typename GC, typename T, typename Traits >
    static inline void print_stat( cc::MultiLevelHashSet< GC, T, Traits > const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }

} // namespace set2

#endif // #ifndef CDSUNIT_SET_TYPE_MICHAEL_H
