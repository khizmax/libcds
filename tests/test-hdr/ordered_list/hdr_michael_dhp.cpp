//$$CDS-header$$

#include "ordered_list/hdr_michael.h"
#include <cds/container/michael_list_dhp.h>

namespace ordlist {
    namespace {
        struct PTB_cmp_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
        };
    }
    void MichaelListTestHeader::PTB_cmp()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::PTB, item, PTB_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::PTB, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct PTB_less_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
        };
    }
    void MichaelListTestHeader::PTB_less()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::PTB, item, PTB_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::PTB, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct PTB_cmpmix_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>  less;
        };
    }
    void MichaelListTestHeader::PTB_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::PTB, item, PTB_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::PTB, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct PTB_ic_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelListTestHeader::PTB_ic()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::PTB, item, PTB_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::PTB, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

