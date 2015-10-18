//$$CDS-header$$

#ifndef CDSUNIT_SET_TYPE_MICHAEL_H
#define CDSUNIT_SET_TYPE_MICHAEL_H

#include "set2/set_type.h"

#include <cds/container/multilevel_hashset_hp.h>
#include <cds/container/multilevel_hashset_dhp.h>
#include <cds/container/multilevel_hashset_rcu.h>

#include "print_multilevel_hashset_stat.h"
#include "hashing/hash_func.h"

namespace set2 {

    template <class GC, typename T, typename Traits = cc::multilevel_hashset::traits>
    class MultiLevelHashSet : public cc::MultiLevelHashSet< GC, T, Traits >
    {
        typedef cc::MultiLevelHashSet< GC, T, Traits > base_class;

        template <typename G>
        struct get_extracted_ptr
        {
            typedef typename base_class::guarded_ptr extracted_ptr;
        };

        template <typename RCU>
        struct get_extracted_ptr<cds::urcu::gc<RCU>>
        {
            typedef typename base_class::exempt_ptr extracted_ptr;
        };

    public:
        typedef typename T::hasher hasher ;
        typedef typename get_extracted_ptr<GC>::extracted_ptr extracted_ptr;

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
        extracted_ptr extract(Q const& key)
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
        static CDS_CONSTEXPR bool const c_bEraseExactKey = true;
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
        typedef MultiLevelHashSet< rcu_gpi, key_val<std::hash<key_type>>, default_traits >    MultiLevelHashSet_rcu_gpi_stdhash;
        typedef MultiLevelHashSet< rcu_gpb, key_val<std::hash<key_type>>, default_traits >    MultiLevelHashSet_rcu_gpb_stdhash;
        typedef MultiLevelHashSet< rcu_gpt, key_val<std::hash<key_type>>, default_traits >    MultiLevelHashSet_rcu_gpt_stdhash;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<std::hash<key_type>>, default_traits >    MultiLevelHashSet_rcu_shb_stdhash;
        typedef MultiLevelHashSet< rcu_sht, key_val<std::hash<key_type>>, default_traits >    MultiLevelHashSet_rcu_sht_stdhash;
#endif

        struct traits_MultiLevelHashSet_stat: public cc::multilevel_hashset::make_traits<
                co::type_traits< default_traits >,
                co::stat< cc::multilevel_hashset::stat<>>
            >::type
        {};

        typedef MultiLevelHashSet< cds::gc::HP,  key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_hp_stdhash_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_dhp_stdhash_stat;
        typedef MultiLevelHashSet< rcu_gpi, key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_rcu_gpi_stdhash_stat;
        typedef MultiLevelHashSet< rcu_gpb, key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_rcu_gpb_stdhash_stat;
        typedef MultiLevelHashSet< rcu_gpt, key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_rcu_gpt_stdhash_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_rcu_shb_stdhash_stat;
        typedef MultiLevelHashSet< rcu_sht, key_val<std::hash<key_type>>, traits_MultiLevelHashSet_stat >    MultiLevelHashSet_rcu_sht_stdhash_stat;
#endif

        // SHA256
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_hp_sha256;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_dhp_sha256;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_rcu_gpi_sha256;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_rcu_gpb_sha256;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_rcu_gpt_sha256;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_rcu_shb_sha256;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::sha256>, default_traits >    MultiLevelHashSet_rcu_sht_sha256;
#endif

        struct traits_MultiLevelHashSet_sha256_stat : public default_traits
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_hp_sha256_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_dhp_sha256_stat;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_rcu_gpi_sha256_stat;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_rcu_gpb_sha256_stat;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_rcu_gpt_sha256_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_rcu_shb_sha256_stat;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::sha256>, traits_MultiLevelHashSet_sha256_stat >    MultiLevelHashSet_rcu_sht_sha256_stat;
#endif

        //MD5
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_hp_md5;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_dhp_md5;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_rcu_gpi_md5;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_rcu_gpb_md5;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_rcu_gpt_md5;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_rcu_shb_md5;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::md5>, default_traits >    MultiLevelHashSet_rcu_sht_md5;
#endif

        struct traits_MultiLevelHashSet_md5_stat : public default_traits
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_hp_md5_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_dhp_md5_stat;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_rcu_gpi_md5_stat;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_rcu_gpb_md5_stat;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_rcu_gpt_md5_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_rcu_shb_md5_stat;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::md5>, traits_MultiLevelHashSet_md5_stat >    MultiLevelHashSet_rcu_sht_md5_stat;
#endif

        // CityHash
#if CDS_BUILD_BITS == 64
        struct traits_MultiLevelHashSet_city64 : public default_traits
        {
            typedef ::hashing::city64::less less;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_hp_city64;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_dhp_city64;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_rcu_gpi_city64;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_rcu_gpb_city64;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_rcu_gpt_city64;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_rcu_shb_city64;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64 >    MultiLevelHashSet_rcu_sht_city64;
#endif

        struct traits_MultiLevelHashSet_city64_stat : public traits_MultiLevelHashSet_city64
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_hp_city64_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_dhp_city64_stat;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_rcu_gpi_city64_stat;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_rcu_gpb_city64_stat;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_rcu_gpt_city64_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_rcu_shb_city64_stat;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::city64>, traits_MultiLevelHashSet_city64_stat >    MultiLevelHashSet_rcu_sht_city64_stat;
#endif

        struct traits_MultiLevelHashSet_city128 : public default_traits
        {
            typedef ::hashing::city128::less less;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_hp_city128;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_dhp_city128;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_rcu_gpi_city128;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_rcu_gpb_city128;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_rcu_gpt_city128;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_rcu_shb_city128;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128 >    MultiLevelHashSet_rcu_sht_city128;
#endif

        struct traits_MultiLevelHashSet_city128_stat : public traits_MultiLevelHashSet_city128
        {
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef MultiLevelHashSet< cds::gc::HP,  key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_hp_city128_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_dhp_city128_stat;
        typedef MultiLevelHashSet< rcu_gpi, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_rcu_gpi_city128_stat;
        typedef MultiLevelHashSet< rcu_gpb, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_rcu_gpb_city128_stat;
        typedef MultiLevelHashSet< rcu_gpt, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_rcu_gpt_city128_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_rcu_shb_city128_stat;
        typedef MultiLevelHashSet< rcu_sht, key_val<::hashing::city128>, traits_MultiLevelHashSet_city128_stat >    MultiLevelHashSet_rcu_sht_city128_stat;
#endif

#endif // #if CDS_BUILD_BITS == 64


        // for fixed-sized key
        // No hash function is necessary

        struct fixed_sized_key
        {
            typedef typename set_type_base< Key, Val >::key_type key_type;
            struct key_val : public set_type_base< Key, Val >::key_val
            {
                typedef typename set_type_base< Key, Val >::key_val base_class;

                /*explicit*/ key_val(key_type const& k) : base_class(k) {}
                key_val(key_type const& k, value_type const& v) : base_class(k, v) {}

                template <typename K>
                /*explicit*/ key_val(K const& k) : base_class(k) {}

                template <typename K, typename T>
                key_val(K const& k, T const& v) : base_class(k, v) {}

                // mock hasher
                struct hasher {
                template <typename Q>
                    key_type operator()( Q const& k ) const
                    {
                        return key_type( k );
                    }
                };
            };

            struct traits : public cc::multilevel_hashset::traits
            {
                struct hash_accessor {
                    key_type operator()(key_val const& kv)
                    {
                        return kv.key;
                    }
                };
            };

            struct traits_stat : public traits
            {
                typedef cc::multilevel_hashset::stat<> stat;
            };
        };

        typedef MultiLevelHashSet< cds::gc::HP, typename fixed_sized_key::key_val, typename fixed_sized_key::traits >    MultiLevelHashSet_hp_fixed;
        typedef MultiLevelHashSet< cds::gc::DHP, typename fixed_sized_key::key_val, typename fixed_sized_key::traits >    MultiLevelHashSet_dhp_fixed;
        typedef MultiLevelHashSet< rcu_gpi, typename fixed_sized_key::key_val, typename fixed_sized_key::traits >    MultiLevelHashSet_rcu_gpi_fixed;
        typedef MultiLevelHashSet< rcu_gpb, typename fixed_sized_key::key_val, typename fixed_sized_key::traits >    MultiLevelHashSet_rcu_gpb_fixed;
        typedef MultiLevelHashSet< rcu_gpt, typename fixed_sized_key::key_val, typename fixed_sized_key::traits >    MultiLevelHashSet_rcu_gpt_fixed;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, typename fixed_sized_key::key_val, typename fixed_sized_key::traits >    MultiLevelHashSet_rcu_shb_fixed;
        typedef MultiLevelHashSet< rcu_sht, typename fixed_sized_key::key_val, typename fixed_sized_key::traits >    MultiLevelHashSet_rcu_sht_fixed;
#endif

        typedef MultiLevelHashSet< cds::gc::HP, typename fixed_sized_key::key_val, typename fixed_sized_key::traits_stat >    MultiLevelHashSet_hp_fixed_stat;
        typedef MultiLevelHashSet< cds::gc::DHP, typename fixed_sized_key::key_val, typename fixed_sized_key::traits_stat >    MultiLevelHashSet_dhp_fixed_stat;
        typedef MultiLevelHashSet< rcu_gpi, typename fixed_sized_key::key_val, typename fixed_sized_key::traits_stat >    MultiLevelHashSet_rcu_gpi_fixed_stat;
        typedef MultiLevelHashSet< rcu_gpb, typename fixed_sized_key::key_val, typename fixed_sized_key::traits_stat >    MultiLevelHashSet_rcu_gpb_fixed_stat;
        typedef MultiLevelHashSet< rcu_gpt, typename fixed_sized_key::key_val, typename fixed_sized_key::traits_stat >    MultiLevelHashSet_rcu_gpt_fixed_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef MultiLevelHashSet< rcu_shb, typename fixed_sized_key::key_val, typename fixed_sized_key::traits_stat >    MultiLevelHashSet_rcu_shb_fixed_stat;
        typedef MultiLevelHashSet< rcu_sht, typename fixed_sized_key::key_val, typename fixed_sized_key::traits_stat >    MultiLevelHashSet_rcu_sht_fixed_stat;
#endif

    };

    template <typename GC, typename T, typename Traits >
    static inline void print_stat( MultiLevelHashSet< GC, T, Traits > const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }

} // namespace set2

#endif // #ifndef CDSUNIT_SET_TYPE_MICHAEL_H
