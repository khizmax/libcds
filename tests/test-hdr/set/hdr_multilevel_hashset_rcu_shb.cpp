//$$CDS-header$$

#include "set/hdr_multilevel_hashset.h"
#include <cds/urcu/signal_buffered.h>
#include <cds/container/multilevel_hashset_rcu.h>
#include "unit/print_multilevel_hashset_stat.h"

namespace set {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace {
        typedef cds::urcu::gc<cds::urcu::signal_buffered<>> rcu_type;
    } // namespace
#endif

    void MultiLevelHashSetHdrTest::rcu_shb_nohash()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits : public cc::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_rcu<set_type, nohash<key_type>>(4, 2);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::multilevel_hashset::make_traits<
            cc::multilevel_hashset::hash_accessor< get_key<key_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(4, 2);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_stdhash()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(4, 2);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(4, 2);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_hash128()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef hash128::less less;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash128!!!" );
        test_rcu<set_type, hash128::make>(4, 2);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , co::less< hash_type::less >
            >::type
        > set_type2;
        test_rcu<set_type2, hash128::make>(4, 2);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_nohash_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits : public cc::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_rcu<set_type, nohash<key_type>>(4, 2);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::multilevel_hashset::make_traits<
            cc::multilevel_hashset::hash_accessor< get_key<key_type>>
            , co::stat< cc::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(4, 2);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_stdhash_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(4, 2);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(4, 2);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_hash128_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef hash128::cmp  compare;
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash_type::make>(4, 2);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::multilevel_hashset::stat<>>
                ,co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_rcu<set_type2, hash_type::make>(4, 2);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_nohash_5_3()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, nohash<key_type>>(5, 3);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_key<key_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(5, 3);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_stdhash_5_3()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(5, 3);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(5, 3);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_hash128_4_3()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash128::make >(4, 3);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        > set_type2;
        test_rcu<set_type2, hash128::make >(4, 3);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_nohash_5_3_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t key_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, nohash<key_type>>(5, 3);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_key<key_type>>
                ,co::stat< cc::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(5, 3);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_stdhash_5_3_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef size_t hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::multilevel_hashset::stat<> stat;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(5, 3);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(5, 3);
#endif
    }

    void MultiLevelHashSetHdrTest::rcu_shb_hash128_4_3_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef hash128 hash_type;

        struct traits: public cc::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::multilevel_hashset::stat<> stat;
            typedef hash128::less less;
            typedef hash128::cmp compare;
        };
        typedef cc::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash_type::make>(4, 3);

        typedef cc::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename cc::multilevel_hashset::make_traits<
                cc::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , co::stat< cc::multilevel_hashset::stat<>>
                , co::less< hash_type::less >
                , co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_rcu<set_type2, hash_type::make>(4, 3);
#endif
    }
} // namespace set
