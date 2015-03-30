//$$CDS-header$$

#include "list/hdr_michael_kv.h"
#include <cds/container/michael_kvlist_hp.h>

namespace ordlist {
    namespace {
        struct HP_cmp_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
        };

    }
    void MichaelKVListTestHeader::HP_cmp()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::HP, key_type, value_type, HP_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::HP,
            key_type,
            value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_less_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
        };
    }
    void MichaelKVListTestHeader::HP_less()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::HP, key_type, value_type, HP_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::HP, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_cmpmix_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>  less;
        };
    }
    void MichaelKVListTestHeader::HP_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::HP, key_type, value_type, HP_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::HP, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HP_ic_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelKVListTestHeader::HP_ic()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::HP, key_type, value_type, HP_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::HP, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

CPPUNIT_TEST_SUITE_REGISTRATION(ordlist::MichaelKVListTestHeader);
