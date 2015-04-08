//$$CDS-header$$

#include "list/hdr_michael_kv.h"
#include <cds/container/michael_kvlist_dhp.h>

namespace ordlist {
    namespace {
        struct DHP_cmp_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
        };
    }
    void MichaelKVListTestHeader::DHP_cmp()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type, DHP_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_less_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
        };
    }
    void MichaelKVListTestHeader::DHP_less()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type, DHP_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_cmpmix_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>  less;
        };
    }
    void MichaelKVListTestHeader::DHP_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type, DHP_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_ic_traits: public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelKVListTestHeader::DHP_ic()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type, DHP_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::DHP, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

