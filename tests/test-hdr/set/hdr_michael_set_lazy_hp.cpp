//$$CDS-header$$

#include "set/hdr_set.h"
#include <cds/container/lazy_list_hp.h>
#include <cds/container/michael_set.h>

namespace set {

    namespace {
        struct set_traits: public cc::michael_set::traits
        {
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
        };

        struct HP_cmp_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
        };

        struct HP_less_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };

        struct HP_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };
    }

    void HashSetHdrTest::Lazy_HP_cmp()
    {
        typedef cc::LazyList< cds::gc::HP, item, HP_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list, set_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int< opt_set >();
    }

    void HashSetHdrTest::Lazy_HP_less()
    {
        typedef cc::LazyList< cds::gc::HP, item, HP_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list, set_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int< opt_set >();
    }

    void HashSetHdrTest::Lazy_HP_cmpmix()
    {
        typedef cc::LazyList< cds::gc::HP, item, HP_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list, set_traits > set;
        test_int< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::HP, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int< opt_set >();
    }


} // namespace set

