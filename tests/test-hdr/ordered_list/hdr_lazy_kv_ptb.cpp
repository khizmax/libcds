//$$CDS-header$$

#include "ordered_list/hdr_lazy_kv.h"
#include <cds/container/lazy_kvlist_ptb.h>

namespace ordlist {
    namespace {
        struct PTB_cmp_traits: public cc::lazy_list::type_traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
        };
    }
    void LazyKVListTestHeader::PTB_cmp()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type, PTB_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct PTB_less_traits: public cc::lazy_list::type_traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
        };
    }
    void LazyKVListTestHeader::PTB_less()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type, PTB_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct PTB_cmpmix_traits: public cc::lazy_list::type_traits
        {
            typedef LazyKVListTestHeader::cmp<LazyKVListTestHeader::key_type>   compare;
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>  less;
        };
    }
    void LazyKVListTestHeader::PTB_cmpmix()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type, PTB_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct PTB_ic_traits: public cc::lazy_list::type_traits
        {
            typedef LazyKVListTestHeader::lt<LazyKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void LazyKVListTestHeader::PTB_ic()
    {
        // traits-based version
        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type, PTB_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::LazyKVList< cds::gc::PTB, key_type, value_type,
            cc::lazy_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

