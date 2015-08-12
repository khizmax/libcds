//$$CDS-header$$

#include "set/hdr_intrusive_multilevel_hashset.h"
#include <cds/intrusive/multilevel_hashset_hp.h>
#include "unit/print_multilevel_hashset_stat.h"

namespace set {
    namespace {
        typedef cds::gc::HP gc_type;
    } // namespace

    void IntrusiveMultiLevelHashSetHdrTest::hp_stdhash()
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

    void IntrusiveMultiLevelHashSetHdrTest::hp_stdhash_stat()
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

    void IntrusiveMultiLevelHashSetHdrTest::hp_stdhash_5_3()
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

    void IntrusiveMultiLevelHashSetHdrTest::hp_stdhash_5_3_stat()
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

} // namespace set

CPPUNIT_TEST_SUITE_REGISTRATION(set::IntrusiveMultiLevelHashSetHdrTest);
