/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "ordered_list/hdr_michael.h"
#include <cds/container/michael_list_dhp.h>

namespace ordlist {
    namespace {
        struct DHP_cmp_traits: public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
        };
    }
    void MichaelListTestHeader::DHP_cmp()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::DHP, item, DHP_cmp_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::DHP, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_less_traits: public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
        };
    }
    void MichaelListTestHeader::DHP_less()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::DHP, item, DHP_less_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::DHP, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_cmpmix_traits: public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::cmp<MichaelListTestHeader::item>   compare;
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>  less;
        };
    }
    void MichaelListTestHeader::DHP_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::DHP, item, DHP_cmpmix_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::DHP, item,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<item> >
                ,cc::opt::less< lt<item> >
            >::type
        > opt_list;
        test< opt_list >();
    }

    namespace {
        struct DHP_ic_traits: public cc::michael_list::traits
        {
            typedef MichaelListTestHeader::lt<MichaelListTestHeader::item>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelListTestHeader::DHP_ic()
    {
        // traits-based version
        typedef cc::MichaelList< cds::gc::DHP, item, DHP_ic_traits > list;
        test< list >();

        // option-based version

        typedef cc::MichaelList< cds::gc::DHP, item,
            cc::michael_list::make_traits<
                cc::opt::less< lt<item> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        test< opt_list >();
    }

}   // namespace ordlist

