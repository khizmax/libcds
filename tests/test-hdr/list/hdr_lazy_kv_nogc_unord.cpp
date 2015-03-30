//$$CDS-header$$

#include "list/hdr_lazy_kv.h"
#include <cds/container/lazy_kvlist_nogc.h>

namespace ordlist {
    namespace {
        struct NOGC_cmp_traits: public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
            static const bool sort = false;
        };

    }
    void LazyKVListTestHeader::NOGC_cmp_unord()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_cmp_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyKVList< cds::gc::nogc,
            key_type,
            value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_less_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
            static const bool sort = false;
        };
    }
    void LazyKVListTestHeader::NOGC_less_unord()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_less_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_equal_to_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::eq<LazyKVListTestHeader::key_type>   equal_to;
            static const bool sort = false;
        };
    }
    void LazyKVListTestHeader::NOGC_equal_to_unord()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_equal_to_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::equal_to< eq<key_type> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>  less;
            typedef LazyKVListTestHeader::eq<LazyKVListTestHeader::key_type>   equal_to;
            static const bool sort = false;
        };
    }
    void LazyKVListTestHeader::NOGC_cmpmix_unord()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_cmpmix_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
                ,cc::opt::equal_to< eq<key_type> >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

    namespace {
        struct NOGC_ic_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::eq<LazyKVListTestHeader::key_type>   equal_to;
            typedef cds::atomicity::item_counter item_counter;
            static const bool sort = false;
        };
    }
    void LazyKVListTestHeader::NOGC_ic_unord()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_ic_traits > list;
        nogc_unord_test< list >();

        // option-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::equal_to< eq<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
                ,cc::opt::sort<false>
            >::type
        > opt_list;
        nogc_unord_test< opt_list >();
    }

}   // namespace ordlist
