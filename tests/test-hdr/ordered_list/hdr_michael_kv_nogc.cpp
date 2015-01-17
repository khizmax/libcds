/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "ordered_list/hdr_michael_kv.h"
#include <cds/container/michael_kvlist_nogc.h>

namespace ordlist {
    namespace {
        struct NOGC_cmp_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
        };

    }
    void MichaelKVListTestHeader::NOGC_cmp()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::nogc, key_type, value_type, NOGC_cmp_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::nogc,
            key_type,
            value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_less_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
        };
    }
    void MichaelKVListTestHeader::NOGC_less()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::nogc, key_type, value_type, NOGC_less_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::nogc, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_cmpmix_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::cmp<MichaelKVListTestHeader::key_type>   compare;
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>  less;
        };
    }
    void MichaelKVListTestHeader::NOGC_cmpmix()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::nogc, key_type, value_type, NOGC_cmpmix_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::nogc, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::compare< cmp<key_type> >
                ,cc::opt::less< lt<key_type> >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

    namespace {
        struct NOGC_ic_traits : public cc::michael_list::traits
        {
            typedef MichaelKVListTestHeader::lt<MichaelKVListTestHeader::key_type>   less;
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void MichaelKVListTestHeader::NOGC_ic()
    {
        // traits-based version
        typedef cc::MichaelKVList< cds::gc::nogc, key_type, value_type, NOGC_ic_traits > list;
        nogc_test< list >();

        // option-based version

        typedef cc::MichaelKVList< cds::gc::nogc, key_type, value_type,
            cc::michael_list::make_traits<
                cc::opt::less< lt<key_type> >
                ,cc::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > opt_list;
        nogc_test< opt_list >();
    }

}   // namespace ordlist

