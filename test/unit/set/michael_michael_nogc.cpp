// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_set_nogc.h"

#include <cds/container/michael_list_nogc.h>
#include <cds/container/michael_set_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class MichaelSet_NoGC : public cds_test::container_set_nogc
    {
    protected:
        typedef cds_test::container_set_nogc base_class;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( MichaelSet_NoGC, compare )
    {
        typedef cc::MichaelList< gc_type, int_item,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > list_type;

        typedef cc::MichaelHashSet< gc_type, list_type,
            typename cc::michael_set::make_traits<
                cds::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( MichaelSet_NoGC, less )
    {
        typedef cc::MichaelList< gc_type, int_item,
            typename cc::michael_list::make_traits<
                cds::opt::less< base_class::less >
            >::type
        > list_type;

        typedef cc::MichaelHashSet< gc_type, list_type,
            typename cc::michael_set::make_traits<
                cds::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( MichaelSet_NoGC, cmpmix )
    {
        struct list_traits : public cc::michael_list::traits
        {
            typedef base_class::less less;
            typedef cmp compare;
        };
        typedef cc::MichaelList< gc_type, int_item, list_traits > list_type;

        typedef cc::MichaelHashSet< gc_type, list_type,
            typename cc::michael_set::make_traits<
                cds::opt::hash< hash_int >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( MichaelSet_NoGC, item_counting )
    {
        struct list_traits : public cc::michael_list::traits
        {
            typedef cmp compare;
        };
        typedef cc::MichaelList< gc_type, int_item, list_traits > list_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef simple_item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 3 );
        test( s );
    }

    TEST_F( MichaelSet_NoGC, backoff )
    {
        struct list_traits : public cc::michael_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::make_exponential_t<cds::backoff::pause, cds::backoff::yield> back_off;
        };
        typedef cc::MichaelList< gc_type, int_item, list_traits > list_type;

        struct set_traits : public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( MichaelSet_NoGC, seq_cst )
    {
        struct list_traits : public cc::michael_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelList< gc_type, int_item, list_traits > list_type;

        struct set_traits : public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( MichaelSet_NoGC, stat )
    {
        struct list_traits: public cc::michael_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::michael_list::stat<> stat;
        };
        typedef cc::MichaelList< gc_type, int_item, list_traits > list_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
        EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( MichaelSet_NoGC, wrapped_stat )
    {
        struct list_traits: public cc::michael_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::michael_list::wrapped_stat<> stat;
        };
        typedef cc::MichaelList< gc_type, int_item, list_traits > list_type;

        struct set_traits: public cc::michael_set::traits
        {
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashSet< gc_type, list_type, set_traits >set_type;

        set_type s( kSize, 4 );
        test( s );
        EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
    }

} // namespace
