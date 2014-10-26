//$$CDS-header$$

#include "set/hdr_set.h"
#include <cds/container/lazy_list_dhp.h>
#include <cds/container/michael_set.h>

namespace set {

    namespace {
        struct set_traits: public cc::michael_set::traits
        {
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
        };

        struct DHP_cmp_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
        };

        struct DHP_less_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };

        struct DHP_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };
    }

    void HashSetHdrTest::Lazy_DHP_cmp()
    {
        typedef cc::LazyList< cds::gc::DHP, item, DHP_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::DHP, list, set_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::DHP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int< opt_set >();
    }

    void HashSetHdrTest::Lazy_DHP_less()
    {
        typedef cc::LazyList< cds::gc::DHP, item, DHP_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::DHP, list, set_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::DHP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int< opt_set >();
    }

    void HashSetHdrTest::Lazy_DHP_cmpmix()
    {
        typedef cc::LazyList< cds::gc::DHP, item, DHP_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::DHP, list, set_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::DHP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int< opt_set >();
    }


} // namespace set
