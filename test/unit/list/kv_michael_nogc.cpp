// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_kv_list_nogc.h"
#include <cds/container/michael_kvlist_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class MichaelKVList_NOGC : public cds_test::kv_list_nogc
    {};

    TEST_F( MichaelKVList_NOGC, less_ordered )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::less< lt >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelKVList_NOGC, compare_ordered )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelKVList_NOGC, mix_ordered )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp >
                ,cds::opt::less< lt >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelKVList_NOGC, item_counting )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelKVList_NOGC, backoff )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelKVList_NOGC, seq_cst )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelKVList_NOGC, stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::stat<> stat;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( MichaelKVList_NOGC, wrapped_stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::wrapped_stat<> stat;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        cds::container::michael_list::stat<> st;
        list_type l( st );
        test( l );
        test_ordered_iterator( l );
    }

} // namespace
