//$$CDS-header$$

#include "list/hdr_lazy_kv.h"
#include <cds/container/lazy_kvlist_nogc.h>

namespace ordlist {
    namespace {
        struct NOGC_cmp_traits: public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
        };

    }
    void LazyKVListTestHeader::NOGC_cmp()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_cmp_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::nogc,
            key_type,
            value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_less_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
        };
    }
    void LazyKVListTestHeader::NOGC_less()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_less_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>  less;
        };
    }
    void LazyKVListTestHeader::NOGC_cmpmix()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_cmpmix_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_ic_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyKVListTestHeader::NOGC_ic()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type, NOGC_ic_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::nogc, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

}   // namespace ordlist

