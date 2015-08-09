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
        test_hp<set_type>();

        typedef ci::MultiLevelHashSet< 
            gc_type, 
            Item<hash_type>, 
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
            >::type
        > set_type2;
        test_hp<set_type2>();
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
        test_hp<set_type>();

        typedef ci::MultiLevelHashSet< 
            gc_type, 
            Item<hash_type>, 
            typename ci::multilevel_hashset::make_traits<
                ci::multilevel_hashset::hash_accessor< get_hash<hash_type>>
                , ci::opt::disposer< item_disposer >
                ,co::stat< ci::multilevel_hashset::stat<>>
            >::type
        > set_type2;
        test_hp<set_type2>();
    }

} // namespace set

CPPUNIT_TEST_SUITE_REGISTRATION(set::IntrusiveMultiLevelHashSetHdrTest);
