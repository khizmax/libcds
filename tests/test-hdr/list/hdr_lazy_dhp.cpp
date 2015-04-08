//$$CDS-header$$

#include "list/hdr_lazy.h"
#include <cds/container/lazy_list_dhp.h>

namespace ordlist {
    namespace {
        struct DHP_cmp_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
        };
    }
    void LazyListTestHeader::DHP_cmp()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::DHP, item, DHP_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::DHP, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_less_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
        };
    }
    void LazyListTestHeader::DHP_less()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::DHP, item, DHP_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::DHP, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_cmpmix_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>  less;
        };
    }
    void LazyListTestHeader::DHP_cmpmix()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::DHP, item, DHP_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::DHP, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_ic_traits: public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyListTestHeader::DHP_ic()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::DHP, item, DHP_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyList< cds::gc::DHP, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

