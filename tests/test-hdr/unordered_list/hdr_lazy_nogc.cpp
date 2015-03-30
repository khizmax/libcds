//$$CDS-header$$

#include "unordered_list/hdr_lazy.h"
#include <cds/container/lazy_list_nogc.h>

namespace unordlist {
    namespace {
        struct NOGC_cmp_traits : public cc::lazy_list::traits
        {
            typedef UnorderedLazyListTestHeader::cmp<UnorderedLazyListTestHeader::item>   compare;
            static const bool sort = false;
        };
    }
    void UnorderedLazyListTestHeader::NOGC_cmp()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_cmp_traits > list;
        nogc_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_less_traits : public cc::lazy_list::traits
        {
            typedef UnorderedLazyListTestHeader::lt<UnorderedLazyListTestHeader::item>   less;
            static const bool sort = false;
        };
    }
    void UnorderedLazyListTestHeader::NOGC_less()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_less_traits > list;
        nogc_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_equal_to_traits : public cc::lazy_list::traits
        {
            typedef UnorderedLazyListTestHeader::equal_to<UnorderedLazyListTestHeader::item> equal_to;
            static const bool sort = false;
        };
    }
    void UnorderedLazyListTestHeader::NOGC_equal_to()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_equal_to_traits > list;
        nogc_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::equal_to< equal_to<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef UnorderedLazyListTestHeader::cmp<UnorderedLazyListTestHeader::item>   compare;
            typedef UnorderedLazyListTestHeader::lt<UnorderedLazyListTestHeader::item>  less;
            static const bool sort = false;
        };
    }
    void UnorderedLazyListTestHeader::NOGC_cmpmix()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_cmpmix_traits > list;
        nogc_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_equal_to_mix_traits : public cc::lazy_list::traits
        {
            typedef UnorderedLazyListTestHeader::cmp<UnorderedLazyListTestHeader::item> compare;
            typedef UnorderedLazyListTestHeader::lt<UnorderedLazyListTestHeader::item> less;
            typedef UnorderedLazyListTestHeader::equal_to<UnorderedLazyListTestHeader::item> equal_to;
            static const bool sort = false;
        };
    }
    void UnorderedLazyListTestHeader::NOGC_equal_to_mix()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_equal_to_mix_traits > list;
        nogc_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
                ,cc::opt::equal_to< equal_to<item> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }
    namespace {
        struct NOGC_ic_traits : public cc::lazy_list::traits
        {
            typedef UnorderedLazyListTestHeader::equal_to<UnorderedLazyListTestHeader::item> equal_to;
            typedef cds::atomicity::item_counter item_counter;
            static const bool sort = false;
        };
    }
    void UnorderedLazyListTestHeader::NOGC_ic()
    {
        // traits-based version
        typedef cc::LazyList< cds::gc::nogc, item, NOGC_ic_traits > list;
        nogc_test< list >();

        // option-based version
        typedef cc::LazyList< cds::gc::nogc, item,
            cc::lazy_list::make_traits<
                cc::opt::equal_to< equal_to<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

}   // namespace unordlist
CPPUNIT_TEST_SUITE_REGISTRATION(unordlist::UnorderedLazyListTestHeader);
