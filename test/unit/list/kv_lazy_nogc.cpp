// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_kv_list_nogc.h"
#include <cds/container/lazy_kvlist_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class LazyKVList_NOGC : public cds_test::kv_list_nogc
    {};

    TEST_F( LazyKVList_NOGC, less_ordered )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
                cds::opt::less< lt >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, compare_ordered )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, mix_ordered )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
                cds::opt::compare< cmp >
                ,cds::opt::less< lt >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, item_counting )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::LazyKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, backoff )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::LazyKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, seq_cst )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::LazyKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, mutex )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef std::mutex lock_type;
        };
        typedef cc::LazyKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, stat )
    {
        struct traits: public cc::lazy_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::lazy_list::stat<> stat;
        };
        typedef cc::LazyKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyKVList_NOGC, wrapped_stat )
    {
        struct traits: public cc::lazy_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::lazy_list::wrapped_stat<> stat;
        };
        typedef cc::LazyKVList<gc_type, key_type, value_type, traits > list_type;

        cds::container::lazy_list::stat<> st;
        list_type l( st );
        test( l );
        test_ordered_iterator( l );
    }

} // namespace
