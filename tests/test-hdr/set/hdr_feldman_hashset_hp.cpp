//$$CDS-header$$

#include "set/hdr_feldman_hashset.h"
#include <cds/container/feldman_hashset_hp.h>
#include "unit/print_feldman_hashset_stat.h"

namespace set {
    namespace {
        typedef cds::gc::HP gc_type;
    } // namespace

    void FeldmanHashSetHdrTest::hp_nohash()
    {
        typedef size_t key_type;

        struct traits : public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_hp<set_type, nohash<key_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
            cc::feldman_hashset::hash_accessor< get_key<key_type>>
            >::type
        > set_type2;
        test_hp<set_type2, nohash<key_type>>(4, 2);
    }

    void FeldmanHashSetHdrTest::hp_stdhash()
    {
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(4, 2);
    }

    void FeldmanHashSetHdrTest::hp_hash128()
    {
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef hash128::less less;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash128!!!" );
        test_hp<set_type, hash128::make>(4, 2);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                , co::less< hash_type::less >
            >::type
        > set_type2;
        test_hp<set_type2, hash128::make>(4, 2);
    }

    void FeldmanHashSetHdrTest::hp_nohash_stat()
    {
        typedef size_t key_type;

        struct traits : public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!");
        test_hp<set_type, nohash<key_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
            cc::feldman_hashset::hash_accessor< get_key<key_type>>
            , co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_hp<set_type2, nohash<key_type>>(4, 2);
    }

    void FeldmanHashSetHdrTest::hp_stdhash_stat()
    {
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(4, 2);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(4, 2);
    }

    void FeldmanHashSetHdrTest::hp_hash128_stat()
    {
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef hash128::cmp  compare;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_hp<set_type, hash_type::make>(4, 2);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
                ,co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_hp<set_type2, hash_type::make>(4, 2);
    }

    void FeldmanHashSetHdrTest::hp_nohash_5_3()
    {
        typedef size_t key_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, nohash<key_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_key<key_type>>
            >::type
        > set_type2;
        test_hp<set_type2, nohash<key_type>>(5, 3);
    }

    void FeldmanHashSetHdrTest::hp_stdhash_5_3()
    {
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(5, 3);
    }

    void FeldmanHashSetHdrTest::hp_hash128_4_3()
    {
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_hp<set_type, hash128::make >(4, 3);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        > set_type2;
        test_hp<set_type2, hash128::make >(4, 3);
    }

    void FeldmanHashSetHdrTest::hp_nohash_5_3_stat()
    {
        typedef size_t key_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_key<key_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<key_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, nohash<key_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<key_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_key<key_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_hp<set_type2, nohash<key_type>>(5, 3);
    }

    void FeldmanHashSetHdrTest::hp_stdhash_5_3_stat()
    {
        typedef size_t hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(5, 3);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                ,co::stat< cc::feldman_hashset::stat<>>
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(5, 3);
    }

    void FeldmanHashSetHdrTest::hp_hash128_4_3_stat()
    {
        typedef hash128 hash_type;

        struct traits: public cc::feldman_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef cc::feldman_hashset::stat<> stat;
            typedef hash128::less less;
            typedef hash128::cmp compare;
        };
        typedef cc::FeldmanHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_hp<set_type, hash_type::make>(4, 3);

        typedef cc::FeldmanHashSet<
            gc_type,
            Item<hash_type>,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash<hash_type>>
                , co::stat< cc::feldman_hashset::stat<>>
                , co::less< hash_type::less >
                , co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_hp<set_type2, hash_type::make>(4, 3);
    }


} // namespace set

CPPUNIT_TEST_SUITE_REGISTRATION(set::FeldmanHashSetHdrTest);
