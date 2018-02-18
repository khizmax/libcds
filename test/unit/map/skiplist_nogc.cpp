// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_map_nogc.h"

#include <cds/container/skip_list_map_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class SkipListMap_NoGC : public cds_test::container_map_nogc
    {
    protected:
        typedef cds_test::container_map_nogc base_class;

        template <typename Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            base_class::test( m );

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::value_type map_pair;
            size_t const kkSize = base_class::kSize;

            // get_min
            for ( int i = static_cast<int>( kkSize ); i > 0; --i ) {
                ASSERT_TRUE( m.insert( i ) != m.end());

                map_pair * p = m.get_min();
                ASSERT_TRUE( p != nullptr );
                EXPECT_EQ( p->first.nKey, i );

                p = m.get_max();
                ASSERT_TRUE( p != nullptr );
                EXPECT_EQ( p->first.nKey, static_cast<int>( kkSize ));
            }

            m.clear();
            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            // get_max
            for ( int i = 0; i < static_cast<int>( kkSize ); ++i ) {
                ASSERT_TRUE( m.insert( i ) != m.end());

                map_pair * p = m.get_max();
                ASSERT_TRUE( p != nullptr );
                EXPECT_EQ( p->first.nKey, i );

                p = m.get_min();
                ASSERT_TRUE( p != nullptr );
                EXPECT_EQ( p->first.nKey, 0 );
            }

            m.clear();
        }

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( SkipListMap_NoGC, compare )
    {
        typedef cc::SkipListMap< gc_type, key_type, value_type,
            typename cc::skip_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, less )
    {
        typedef cc::SkipListMap< gc_type, key_type, value_type,
            typename cc::skip_list::make_traits<
                cds::opt::less< base_class::less >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, cmpmix )
    {
        typedef cc::SkipListMap< gc_type, key_type, value_type,
            typename cc::skip_list::make_traits<
                cds::opt::less< base_class::less >
                ,cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, item_counting )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, backoff )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, stat )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::skip_list::stat<> stat;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, xorshift32 )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::xorshift32 random_level_generator;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, xorshift24 )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::xorshift24 random_level_generator;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, xorshift16 )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::xorshift16 random_level_generator;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, turbo32 )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::turbo32 random_level_generator;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, turbo24 )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::turbo24 random_level_generator;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( SkipListMap_NoGC, turbo16 )
    {
        struct map_traits: public cc::skip_list::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::skip_list::stat<> stat;
            typedef cc::skip_list::turbo16 random_level_generator;
        };
        typedef cc::SkipListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

} // namespace
