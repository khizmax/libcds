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
#ifndef CDSUNIT_SET_TEST_SKIPLIST_RCU_H
#define CDSUNIT_SET_TEST_SKIPLIST_RCU_H

#include "test_set_rcu.h"
#include <cds/container/skip_list_set_rcu.h>

namespace cc = cds::container;

template <class RCU>
class SkipListSet: public cds_test::container_set_rcu
{
    typedef cds_test::container_set_rcu base_class;
public:
    typedef cds::urcu::gc<RCU> rcu_type;

protected:
    template <typename Set>
    void test( Set& s )
    {
        // Precondition: set is empty
        // Postcondition: set is empty

        base_class::test( s );

        ASSERT_TRUE( s.empty());
        ASSERT_CONTAINER_SIZE( s, 0 );

        typedef typename Set::value_type value_type;

        size_t const nSetSize = base_class::kSize;
        std::vector< value_type > data;
        std::vector< size_t> indices;
        data.reserve( nSetSize );
        indices.reserve( nSetSize );
        for ( size_t key = 0; key < nSetSize; ++key ) {
            data.push_back( value_type( static_cast<int>(key)));
            indices.push_back( key );
        }
        shuffle( indices.begin(), indices.end());

        for ( auto i : indices ) {
            ASSERT_TRUE( s.insert( data[i] ));
        }
        ASSERT_FALSE( s.empty());
        ASSERT_CONTAINER_SIZE( s, nSetSize );

        typedef typename Set::exempt_ptr exempt_ptr;
        exempt_ptr xp;

        // extract_min
        size_t nCount = 0;
        int nKey = -1;
        while ( !s.empty()) {
            xp = s.extract_min();
            ASSERT_FALSE( !xp );
            EXPECT_EQ( nKey + 1, xp->key());
            ++nCount;
            nKey = xp->key();
        }
        xp.release();
        EXPECT_EQ( nCount, nSetSize );

        ASSERT_TRUE( s.empty());
        ASSERT_CONTAINER_SIZE( s, 0 );

        // extract_max
        for ( auto i : indices ) {
            ASSERT_TRUE( s.insert( data[i] ));
        }
        ASSERT_FALSE( s.empty());
        ASSERT_CONTAINER_SIZE( s, nSetSize );

        nCount = 0;
        nKey = nSetSize;
        while ( !s.empty()) {
            xp = s.extract_max();
            ASSERT_FALSE( !xp );
            EXPECT_EQ( nKey - 1, xp->key());
            ++nCount;
            nKey = xp->key();
        }
        xp.release();
        EXPECT_EQ( nCount, nSetSize );

        ASSERT_TRUE( s.empty());
        ASSERT_CONTAINER_SIZE( s, 0 );
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

TYPED_TEST_CASE_P( SkipListSet );

//TYPED_TEST_P( SkipListSet, compare )
TYPED_TEST_P( SkipListSet, compare )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    typedef cc::SkipListSet< rcu_type, int_item,
        typename cc::skip_list::make_traits<
            cds::opt::compare< typename TestFixture::cmp >
        >::type
    > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( SkipListSet, less )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    typedef cc::SkipListSet< rcu_type, int_item,
        typename cc::skip_list::make_traits<
            cds::opt::less< typename TestFixture::less >
        >::type
    > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( SkipListSet, cmpmix )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    typedef cc::SkipListSet< rcu_type, int_item,
        typename cc::skip_list::make_traits<
            cds::opt::less< typename TestFixture::less >
            ,cds::opt::compare< typename TestFixture::cmp >
        >::type
    > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( SkipListSet, item_counting )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct set_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::SkipListSet< rcu_type, int_item, set_traits >set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( SkipListSet, backoff )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct set_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
    };
    typedef cc::SkipListSet< rcu_type, int_item, set_traits >set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( SkipListSet, stat )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct set_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
        typedef cc::skip_list::stat<> stat;
    };
    typedef cc::SkipListSet< rcu_type, int_item, set_traits >set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( SkipListSet, random_level_generator )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct set_traits: public cc::skip_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::less less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::skip_list::stat<> stat;
        typedef cc::skip_list::xorshift random_level_generator;
        typedef cds::opt::v::rcu_assert_deadlock rcu_check_deadlock;
    };
    typedef cc::SkipListSet< rcu_type, int_item, set_traits >set_type;

    set_type s;
    this->test( s );
}


// GCC 5: All this->test names should be written on single line, otherwise a runtime error will be encountered like as
// "No this->test named <test_name> can be found in this this->test case"
REGISTER_TYPED_TEST_CASE_P( SkipListSet,
    compare, less, cmpmix, item_counting, backoff, stat, random_level_generator
);


#endif // CDSUNIT_SET_TEST_SKIPLIST_RCU_H

