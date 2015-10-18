//$$CDS-header$$

#include "set/hdr_intrusive_multilevel_hashset.h"
#include <cds/urcu/general_instant.h>
#include <cds/intrusive/multilevel_hashset_rcu.h>
#include "unit/print_multilevel_hashset_stat.h"

namespace set {
    namespace {
        typedef cds::urcu::gc<cds::urcu::general_instant<>> rcu_type;
    } // namespace

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_nohash()
    {
        typedef size_t key_type;

        struct traits : public ci::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef item_disposer disposer;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_rcu<set_type, nohash<key_type>>(4, 2);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_key<key_type>>
                , ci::opt::disposer< item_disposer >
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_stdhash()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(4, 2);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_hash128()
    {
        typedef hash128 hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef hash128::less less;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash128!!!" );
        test_rcu<set_type, hash128::make>(4, 2);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                , ci::opt::less< hash_type::less >
            >::type
        > set_type2;
        test_rcu<set_type2, hash128::make>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_nohash_stat()
    {
        typedef size_t key_type;

        struct traits : public ci::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef item_disposer disposer;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_rcu<set_type, nohash<key_type>>(4, 2);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename ci::multilevel_hashset::make_traits<
            ci::multilevel_hashset::hash_accessor< get_key<key_type>>
            , ci::opt::disposer< item_disposer >
            , co::stat< ci::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_stdhash_stat()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(4, 2);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_hash128_stat()
    {
        typedef hash128 hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef hash128::cmp  compare;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash_type::make>(4, 2);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
                ,co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_rcu<set_type2, hash_type::make>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_nohash_5_3()
    {
        typedef size_t key_type;

        struct traits : public ci::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef item_disposer disposer;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_rcu<set_type, nohash<key_type>>(5, 3);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename ci::multilevel_hashset::make_traits<
            ci::multilevel_hashset::hash_accessor< get_key<key_type>>
            , ci::opt::disposer< item_disposer >
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(5, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_stdhash_5_3()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(5, 3);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(5, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_hash128_4_3()
    {
        typedef hash128 hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash128::make >(4, 3);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        > set_type2;
        test_rcu<set_type2, hash128::make >(4, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_nohash_5_3_stat()
    {
        typedef size_t key_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef item_disposer disposer;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, nohash<key_type>>(5, 3);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<key_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_key<key_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, nohash<key_type>>(5, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_stdhash_5_3_stat()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_rcu<set_type, std::hash<hash_type>>(5, 3);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_rcu<set_type2, std::hash<hash_type>>(5, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::rcu_gpi_hash128_4_3_stat()
    {
        typedef hash128 hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef ci::multilevel_hashset::stat<> stat;
            typedef hash128::less less;
            typedef hash128::cmp compare;
        };
        typedef ci::MultiLevelHashSet< rcu_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_rcu<set_type, hash_type::make>(4, 3);

        typedef ci::MultiLevelHashSet<
            rcu_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                , co::stat< ci::multilevel_hashset::stat<>>
                , co::less< hash_type::less >
                , co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_rcu<set_type2, hash_type::make>(4, 3);
    }

} // namespace set
