/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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

TYPED_TEST_P( SkipListMap, random_level_generator )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;

    struct map_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::skip_list::stat<> stat;
        typedef cc::skip_list::xorshift random_level_generator;
        typedef cds::opt::v::rcu_assert_deadlock rcu_check_deadlock;
    };
    typedef cc::SkipListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m;
    this->test( m );
}


REGISTER_TYPED_TEST_CASE_P( SkipListMap,
    compare, less, cmpmix, item_counting, backoff, stat, random_level_generator
);


#endif // CDSUNIT_MAP_TEST_SKIPLIST_RCU_H

