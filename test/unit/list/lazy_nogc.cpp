// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_list_nogc.h"
#include <cds/container/lazy_list_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class LazyList_NOGC : public cds_test::list_nogc
    {};

    TEST_F( LazyList_NOGC, less_ordered )
    {
        typedef cc::LazyList< gc_type, item,
            typename cc::lazy_list::make_traits<
                cds::opt::less< lt<item> >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, compare_ordered )
    {
        typedef cc::LazyList< gc_type, item,
            typename cc::lazy_list::make_traits<
                cds::opt::compare< cmp<item> >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, mix_ordered )
    {
        typedef cc::LazyList< gc_type, item,
            typename cc::lazy_list::make_traits<
                cds::opt::compare< cmp<item> >
                ,cds::opt::less< lt<item> >
            >::type
        > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, item_counting )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::LazyList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, backoff )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::LazyList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, seq_cst )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::LazyList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, mutex )
    {
        struct traits : public cc::lazy_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef std::mutex lock_type;
        };
        typedef cc::LazyList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, stat )
    {
        struct traits: public cc::lazy_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::lazy_list::stat<> stat;
        };
        typedef cc::LazyList<gc_type, item, traits > list_type;

        list_type l;
        test( l );
        test_ordered_iterator( l );
    }

    TEST_F( LazyList_NOGC, wrapped_stat )
    {
        struct traits: public cc::lazy_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::lazy_list::wrapped_stat<> stat;
        };
        typedef cc::LazyList<gc_type, item, traits > list_type;

        cds::container::lazy_list::stat<> st;
        list_type l( st );
        test( l );
        test_ordered_iterator( l );
    }

} // namespace
