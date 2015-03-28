//$$CDS-header$$

#include "set/hdr_set.h"
#include <cds/container/lazy_list_nogc.h>
#include <cds/container/michael_set_nogc.h>

namespace set {

    namespace {
        struct set_traits: public cc::michael_set::traits
        {
            typedef HashSetHdrTest::hash_int            hash;
            typedef HashSetHdrTest::simple_item_counter item_counter;
        };

        struct nogc_cmp_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
        };

        struct nogc_less_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };

        struct nogc_equal_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::equal<HashSetHdrTest::item>   equal_to;
            static const bool sort = false;
        };

        struct nogc_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef HashSetHdrTest::cmp<HashSetHdrTest::item>   compare;
            typedef HashSetHdrTest::less<HashSetHdrTest::item>   less;
        };
    }

    void HashSetHdrTest::Lazy_nogc_cmp()
    {
        typedef cc::LazyList< cds::gc::nogc, item, nogc_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list, set_traits > set;
        test_int_nogc< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int_nogc< opt_set >();
    }

    void HashSetHdrTest::Lazy_nogc_less()
    {
        typedef cc::LazyList< cds::gc::nogc, item, nogc_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list, set_traits > set;
        test_int_nogc< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int_nogc< opt_set >();
    }

    void HashSetHdrTest::Lazy_nogc_equal()
    {
        typedef cc::LazyList< cds::gc::nogc, item, nogc_equal_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list, set_traits > set;
        test_int_nogc_unordered< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int_nogc_unordered< opt_set >();
    }

    void HashSetHdrTest::Lazy_nogc_cmpmix()
    {
        typedef cc::LazyList< cds::gc::nogc, item, nogc_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list, set_traits > set;
        test_int_nogc< set >();

        // option-based version
        typedef cc::MichaelHashSet< cds::gc::nogc, list,
            cc::michael_set::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_set;
        test_int_nogc< opt_set >();
    }


} // namespace set
