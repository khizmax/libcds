//$$CDS-header$$

#include "ordered_list/hdr_michael.h"
#include <cds/container/michael_list_hrc.h>

namespace ordlist {
    namespace {
        struct HRC_cmp_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
        };
    }
    void MichaelListTestHeader::HRC_cmp()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::HRC, item, HRC_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::HRC, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HRC_less_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
        };
    }
    void MichaelListTestHeader::HRC_less()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::HRC, item, HRC_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::HRC, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HRC_cmpmix_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>  less;
        };
    }
    void MichaelListTestHeader::HRC_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::HRC, item, HRC_cmpmix_traits> list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::HRC, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct HRC_ic_traits: public cc::michael_list::type_traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelListTestHeader::HRC_ic()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::HRC, item, HRC_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::HRC, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

