//$$CDS-header$$

#include "list/hdr_lazy.h"
#include <cds/container/lazy_list_hp.h>

namespace ordlist {
    namespace {
        struct HP_cmp_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item> compare;
        };

    }
    void LazyListTestHeader::HP_cmp()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::HP, item, HP_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::HP, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_less_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
        };
    }
    void LazyListTestHeader::HP_less()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::HP, item, HP_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::HP, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>  less;
        };
    }
    void LazyListTestHeader::HP_cmpmix()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::HP, item, HP_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::HP, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_ic_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyListTestHeader::HP_ic()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::HP, item, HP_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::HP, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

CPPUNIT_TEST_SUITE_REGISTRATION(ordlist::LazyListTestHeader);
