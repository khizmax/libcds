// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_map_nogc.h"

#include <cds/container/lazy_kvlist_nogc.h>
#include <cds/container/michael_map_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class MichaelLazyMap_NoGC : public cds_test::container_map_nogc
    {
    protected:
        typedef cds_test::container_map_nogc base_class;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( MichaelLazyMap_NoGC, compare )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > list_type;

        typedef cc::MichaelHashMap< gc_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< hash1 >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( MichaelLazyMap_NoGC, less )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
                cds::opt::less< base_class::less >
            >::type
        > list_type;

        typedef cc::MichaelHashMap< gc_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< hash1 >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( MichaelLazyMap_NoGC, cmpmix )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cmp compare;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        typedef cc::MichaelHashMap< gc_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< hash1 >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( MichaelLazyMap_NoGC, backoff )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::make_exponential_t<cds::backoff::pause, cds::backoff::yield> back_off;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits : public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits >map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( MichaelLazyMap_NoGC, seq_cst )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits : public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits >map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( MichaelLazyMap_NoGC, mutex )
    {
        struct list_traits : public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef std::mutex lock_type;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits : public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( MichaelLazyMap_NoGC, stat )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::lazy_list::stat<> stat;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
        EXPECT_GE( m.statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( MichaelLazyMap_NoGC, wrapped_stat )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::lazy_list::wrapped_stat<> stat;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
        EXPECT_GE( m.statistics().m_nInsertSuccess, 0u );
    }

} // namespace
