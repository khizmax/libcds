//$$CDS-header$$

#include "set/hdr_intrusive_multilevel_hashset.h"
#include <cds/intrusive/multilevel_hashset_dhp.h>
#include "unit/print_multilevel_hashset_stat.h"

namespace set {
    namespace {
        typedef cds::gc::DHP gc_type;
    } // namespace

    void IntrusiveMultiLevelHashSetHdrTest::dhp_stdhash()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
        };
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(4, 2);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::dhp_hash128()
    {
        typedef hash128 hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef hash128::less less;
        };
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash128!!!" );
        test_hp<set_type, hash128::make>(4, 2);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                , ci::opt::less< hash_type::less >
            >::type
        > set_type2;
        test_hp<set_type2, hash128::make>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::dhp_stdhash_stat()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(4, 2);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::dhp_hash128_stat()
    {
        typedef hash128 hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef hash128::cmp  compare;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_hp<set_type, hash_type::make>(4, 2);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
                ,co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_hp<set_type2, hash_type::make>(4, 2);
    }

    void IntrusiveMultiLevelHashSetHdrTest::dhp_stdhash_5_3()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
        };
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(5, 3);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(5, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::dhp_hash128_4_3()
    {
        typedef hash128 hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_hp<set_type, hash128::make >(4, 3);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        > set_type2;
        test_hp<set_type2, hash128::make >(4, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::dhp_stdhash_5_3_stat()
    {
        typedef size_t hash_type;

        struct traits: public ci::multilevel_hashset::traits
        {
            typedef get_hash<hash_type> hash_accessor;
            typedef item_disposer disposer;
            typedef ci::multilevel_hashset::stat<> stat;
        };
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, size_t>::value, "set::hash_type != size_t!!!" );
        test_hp<set_type, std::hash<hash_type>>(5, 3);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_hp<set_type2, std::hash<hash_type>>(5, 3);
    }

    void IntrusiveMultiLevelHashSetHdrTest::dhp_hash128_4_3_stat()
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
        typedef ci::MultiLevelHashSet< gc_type, Item<hash_type>, traits > set_type;
        static_assert(std::is_same< typename set_type::hash_type, hash_type>::value, "set::hash_type != hash_type!!!" );
        test_hp<set_type, hash_type::make>(4, 3);

        typedef ci::MultiLevelHashSet<
            gc_type,
            Item<hash_type>,
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                , co::stat< ci::multilevel_hashset::stat<>>
                , co::less< hash_type::less >
                , co::compare< hash128::cmp >
            >::type
        > set_type2;
        test_hp<set_type2, hash_type::make>(4, 3);
    }

} // namespace set
