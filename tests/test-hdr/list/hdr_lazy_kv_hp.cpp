//$$CDS-header$$

#include "list/hdr_lazy_kv.h"
#include <cds/container/lazy_kvlist_hp.h>

namespace ordlist {
    namespace {
        struct HP_cmp_traits: public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
        };

    }
    void LazyKVListTestHeader::HP_cmp()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::HP, key_type, value_type, HP_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::HP,
            key_type,
            value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_less_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
        };
    }
    void LazyKVListTestHeader::HP_less()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::HP, key_type, value_type, HP_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::HP, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_cmpmix_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>  less;
        };
    }
    void LazyKVListTestHeader::HP_cmpmix()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::HP, key_type, value_type, HP_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::HP, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_ic_traits : public cc::lazy_list::traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyKVListTestHeader::HP_ic()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::HP, key_type, value_type, HP_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::HP, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

CPPUNIT_TEST_SUITE_REGISTRATION(ordlist::LazyKVListTestHeader);
