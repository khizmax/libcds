//$$CDS-header$$

#include "list/hdr_lazy.h"
#include <cds/container/lazy_list_nogc.h>

namespace ordlist {
    namespace {
        struct NOGC_cmp_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
            static const bool sort = false;
        };
    }
    void LazyListTestHeader::NOGC_cmp_unord()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_cmp_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_less_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>   less;
            static const bool sort = false;
        };
    }
    void LazyListTestHeader::NOGC_less_unord()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_less_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_equal_to_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::equal_to<LazyListTestHeader::item> equal_to;
            static const bool sort = false;
        };
    }
    void LazyListTestHeader::NOGC_equal_to_unord()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_equal_to_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::equal_to< equal_to<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item>   compare;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item>  less;
            static const bool sort = false;
        };
    }
    void LazyListTestHeader::NOGC_cmpmix_unord()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_cmpmix_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_equal_to_mix_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::cmp<LazyListTestHeader::item> compare;
            typedef LazyListTestHeader::lt<LazyListTestHeader::item> less;
            typedef LazyListTestHeader::equal_to<LazyListTestHeader::item> equal_to;
            static const bool sort = false;
        };
    }
    void LazyListTestHeader::NOGC_equal_to_mix_unord()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_equal_to_mix_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
                ,cc::opt::equal_to< equal_to<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }
    namespace {
        struct NOGC_ic_traits : public cc::lazy_list::traits
        {
            typedef LazyListTestHeader::equal_to<LazyListTestHeader::item> equal_to;
            typedef cds::atomicity::item_counter item_counter;
            static const bool sort = false;
        };
    }
    void LazyListTestHeader::NOGC_ic_unord()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_ic_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::equal_to< equal_to<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

}   // namespace ordlist
