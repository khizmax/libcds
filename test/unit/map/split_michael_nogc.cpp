// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_map_nogc.h"

#include <cds/container/michael_list_nogc.h>
#include <cds/container/split_list_map_nogc.h>
#include <cds/intrusive/free_list.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class SplitListMichaelMap_NoGC : public cds_test::container_map_nogc
    {
    protected:
        typedef cds_test::container_map_nogc base_class;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( SplitListMichaelMap_NoGC, compare )
    {
        typedef cc::SplitListMap< gc_type, key_type, value_type,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::michael_list_tag >
                , cds::opt::hash< hash1 >
                , cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( SplitListMichaelMap_NoGC, less )
    {
        typedef cc::SplitListMap< gc_type, key_type, value_type,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::michael_list_tag >
                , cds::opt::hash< hash1 >
                , cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        cds::opt::less< less >
                    >::type
                >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( SplitListMichaelMap_NoGC, cmpmix )
    {
        typedef cc::SplitListMap< gc_type, key_type, value_type,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::michael_list_tag >
                , cds::opt::hash< hash1 >
                , cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        cds::opt::less< less >
                        , cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > map_type;

        map_type m( kSize, 3 );
        test( m );
    }

    TEST_F( SplitListMichaelMap_NoGC, item_counting )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef cmp compare;
                typedef base_class::less less;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( SplitListMichaelMap_NoGC, stat )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::split_list::stat<> stat;

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef base_class::less less;
                typedef cds::opt::v::sequential_consistent memory_model;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 5 );
        test( m );
    }

    TEST_F( SplitListMichaelMap_NoGC, back_off )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( SplitListMichaelMap_NoGC, free_list )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::FreeList free_list;

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    struct set_static_traits: public cc::split_list::traits
    {
        static bool const dynamic_bucket_table = false;
    };

    TEST_F( SplitListMichaelMap_NoGC, static_bucket_table )
    {
        struct map_traits: public set_static_traits
        {
            typedef cc::michael_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( SplitListMichaelMap_NoGC, static_bucket_table_free_list )
    {
        struct map_traits: public set_static_traits
        {
            typedef cc::michael_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::FreeList free_list;

            struct ordered_list_traits: public cc::michael_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

} // namespace
