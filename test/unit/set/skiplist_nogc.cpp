// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_set_nogc.h"

#include <cds/container/skip_list_set_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class SkipListSet_NoGC : public cds_test::container_set_nogc
    {
    protected:
        typedef cds_test::container_set_nogc base_class;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( SkipListSet_NoGC, compare )
    {
        typedef cc::SkipListSet< gc_type, int_item,
            typename cc::skip_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, less )
    {
        typedef cc::SkipListSet< gc_type, int_item,
            typename cc::skip_list::make_traits<
                cds::opt::less< base_class::less >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, cmpmix )
    {
        typedef cc::SkipListSet< gc_type, int_item,
            typename cc::skip_list::make_traits<
                cds::opt::less< base_class::less >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, item_counting )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, backoff )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, stat )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::skip_list::stat<> stat;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, xorshift32 )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::xorshift32 random_level_generator;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, xorshift24 )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::xorshift24 random_level_generator;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, xorshift16 )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::xorshift16 random_level_generator;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, turbo32 )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::turbo32 random_level_generator;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, turbo24 )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::turbo24 random_level_generator;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

    TEST_F( SkipListSet_NoGC, turbo16 )
    {
        struct set_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::turbo16 random_level_generator;
        };
        typedef cc::SkipListSet< gc_type, int_item, set_traits >set_type;

        set_type s;
        test( s );
    }

} // namespace
