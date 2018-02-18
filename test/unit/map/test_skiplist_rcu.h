// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_MAP_TEST_SKIPLIST_RCU_H
#define CDSUNIT_MAP_TEST_SKIPLIST_RCU_H

#include "test_map_rcu.h"
#include <cds/container/skip_list_map_rcu.h>

namespace cc = cds::container;

template <class RCU>
class SkipListMap: public cds_test::container_map_rcu
{
    typedef cds_test::container_map_rcu base_class;
public:
    typedef cds::urcu::gc<RCU> rcu_type;

protected:
    template <typename Map>
    void test( Map& m )
    {
        // Precondition: map is empty
        // Postcondition: map is empty

        base_class::test( m );

        ASSERT_TRUE( m.empty());
        ASSERT_CONTAINER_SIZE( m, 0 );

        typedef typename Map::exempt_ptr exempt_ptr;
        size_t const kkSize = base_class::kSize;

        // get_min
        for ( int i = static_cast<int>(kkSize); i > 0; --i )
            ASSERT_TRUE( m.insert( i ));

        exempt_ptr xp;

        size_t nCount = 0;
        int nKey = 0;
        while ( !m.empty()) {
            xp = m.extract_min();
            ASSERT_FALSE( !xp );
            EXPECT_EQ( xp->first.nKey, nKey + 1 );
            nKey = xp->first.nKey;
            ++nCount;
        }
        xp = m.extract_min();
        ASSERT_TRUE( !xp );
        xp = m.extract_max();
        ASSERT_TRUE( !xp );
        EXPECT_EQ( kkSize, nCount );
        ASSERT_TRUE( m.empty());
        ASSERT_CONTAINER_SIZE( m, 0 );

        // get_max
        for ( int i = 0; i < static_cast<int>(kkSize); ++i )
            ASSERT_TRUE( m.insert( i ));

        nKey = kkSize;
        nCount = 0;
        while ( !m.empty()) {
            xp = m.extract_max();
            ASSERT_FALSE( !xp );
            EXPECT_EQ( xp->first.nKey, nKey - 1 );
            nKey = xp->first.nKey;
            ++nCount;
        }
        xp = m.extract_min();
        ASSERT_TRUE( !xp );
        xp = m.extract_max();
        ASSERT_TRUE( !xp );
        EXPECT_EQ( kkSize, nCount );
        ASSERT_TRUE( m.empty());
        ASSERT_CONTAINER_SIZE( m, 0 );
    }

    void SetUp()
    {
        RCU::Construct();
        cds::threading::Manager::attachThread();
    }

    void TearDown()
    {
        cds::threading::Manager::detachThread();
        RCU::Destruct();
    }
};

TYPED_TEST_CASE_P( SkipListMap );

TYPED_TEST_P( SkipListMap, compare )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    typedef cc::SkipListMap< rcu_type, key_type, value_type,
        typename cc::skip_list::make_traits<
            cds::opt::compare< typename TestFixture::cmp >
        >::type
    > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, less )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    typedef cc::SkipListMap< rcu_type, key_type, value_type,
        typename cc::skip_list::make_traits<
            cds::opt::less< typename TestFixture::less >
        >::type
    > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, cmpmix )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    typedef cc::SkipListMap< rcu_type, key_type, value_type,
        typename cc::skip_list::make_traits<
            cds::opt::less< typename TestFixture::less >
            ,cds::opt::compare< typename TestFixture::cmp >
        >::type
    > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, item_counting )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, backoff )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, stat )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
        typedef cc::skip_list::stat<> stat;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, xorshift32 )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cc::skip_list::xorshift32 random_level_generator;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, xorshift24 )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cc::skip_list::xorshift24 random_level_generator;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, xorshift16 )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cc::skip_list::xorshift16 random_level_generator;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, turbo32 )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cc::skip_list::turbo32 random_level_generator;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, turbo24 )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cc::skip_list::turbo24 random_level_generator;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

TYPED_TEST_P( SkipListMap, turbo16 )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cc::skip_list::turbo16 random_level_generator;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}

REGISTER_TYPED_TEST_CASE_P( SkipListMap,
    compare, less, cmpmix, item_counting, backoff, stat, xorshift32, xorshift24, xorshift16, turbo32, turbo24, turbo16
);


#endif // CDSUNIT_MAP_TEST_SKIPLIST_RCU_H

