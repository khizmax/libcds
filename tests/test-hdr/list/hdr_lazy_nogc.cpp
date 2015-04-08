//$$CDS-header$$

#include "list/hdr_lazy.h"
#include <cds/container/lazy_list_nogc.h>

namespace ordlist {
    namespace {
        struct NOGC_cmp_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
        };
    }
    void LazyListTestHeader::NOGC_cmp()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_cmp_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_less_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
        };
    }
    void LazyListTestHeader::NOGC_less()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_less_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>  less;
        };
    }
    void LazyListTestHeader::NOGC_cmpmix()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_cmpmix_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_ic_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyListTestHeader::NOGC_ic()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_ic_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }
}   // namespace ordlist

