// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_list_nogc.h"
#include <cds/container/michael_list_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class MichaelList_NOGC : public cds_test::list_nogc
    {};

    TEST_F( MichaelList_NOGC, less_ordered )
    {
        typedef cc::MichaelList< gc_type, item,
            typename cc::michael_list::make_traits<
                cds::opt::less< lt<item> >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelList_NOGC, compare_ordered )
    {
        typedef cc::MichaelList< gc_type, item,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp<item> >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelList_NOGC, mix_ordered )
    {
        typedef cc::MichaelList< gc_type, item,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp<item> >
                ,cds::opt::less< lt<item> >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelList_NOGC, item_counting )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelList_NOGC, backoff )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelList_NOGC, seq_cst )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelList_NOGC, stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::stat<> stat;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelList_NOGC, wrapped_stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::wrapped_stat<> stat;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        cds::container::michael_list::stat<> st;
        list_type l{ st };
        test( l );
        test_ordered_iterator( l );
    }

} // namespace
