/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_RCU_H
#define CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_RCU_H

#include "test_intrusive_feldman_hashset.h"

#include <cds/intrusive/feldman_hashset_rcu.h>

namespace ci = cds::intrusive;
namespace co = cds::opt;

template <class RCU>
class IntrusiveFeldmanHashSet : public cds_test::intrusive_feldman_hashset
{
    typedef cds_test::intrusive_feldman_hashset base_class;

protected:
    typedef cds::urcu::gc<RCU> rcu_type;

    template <class Set>
    void test( Set& s )
    {
        // Precondition: set is empty
        // Postcondition: set is empty

        base_class::test( s );

        ASSERT_TRUE( s.empty() );
        ASSERT_CONTAINER_SIZE( s, 0 );

        typedef typename Set::value_type value_type;
        size_t const nSetSize = std::max( s.head_size() * 2, static_cast<size_t>(100) );

        std::vector< value_type > data;
        std::vector< size_t> indices;
        data.reserve( nSetSize );
        indices.reserve( nSetSize );
        for ( size_t key = 0; key < nSetSize; ++key ) {
            data.push_back( value_type( static_cast<int>(key) ) );
            indices.push_back( key );
        }
        shuffle( indices.begin(), indices.end() );

        typename Set::exempt_ptr xp;
        value_type * rp;
        typedef typename Set::rcu_lock rcu_lock;

        // get/extract from empty set
        for ( auto idx : indices ) {
            auto& i = data[idx];

            {
                rcu_lock l;
                rp = s.get( i.key() );
                ASSERT_TRUE( !rp );
            }

            xp = s.extract( i.key() );
            ASSERT_TRUE( !xp );
        }

        // fill set
        for ( auto& i : data ) {
            i.nDisposeCount = 0;
            ASSERT_TRUE( s.insert( i ) );
        }

        // get/extract
        for ( auto idx : indices ) {
            auto& i = data[idx];

            {
                rcu_lock l;
                EXPECT_EQ( i.nFindCount, 0 );
                rp = s.get( i.key() );
                ASSERT_FALSE( !rp );
                ++rp->nFindCount;
                EXPECT_EQ( i.nFindCount, 1 );
            }

            EXPECT_EQ( i.nEraseCount, 0 );
            xp = s.extract( i.key());
            ASSERT_FALSE( !xp );
            ++xp->nEraseCount;
            EXPECT_EQ( i.nEraseCount, 1 );

            xp = s.extract( i.key() );
            ASSERT_TRUE( !xp );
        }

        ASSERT_TRUE( s.empty() );
        ASSERT_CONTAINER_SIZE( s, 0 );

        // Force retiring cycle
        Set::gc::force_dispose();
        for ( auto& i : data ) {
            EXPECT_EQ( i.nDisposeCount, 1 );
        }
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

TYPED_TEST_CASE_P( IntrusiveFeldmanHashSet );

TYPED_TEST_P( IntrusiveFeldmanHashSet, compare )
{
    typedef typename TestFixture::rcu_type rcu_type;

    struct traits : public ci::feldman_hashset::traits
    {
        typedef typename TestFixture::hash_accessor hash_accessor;
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::mock_disposer disposer;
    };

    typedef ci::FeldmanHashSet< rcu_type, typename TestFixture::int_item, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveFeldmanHashSet, less )
{
    typedef ci::FeldmanHashSet< typename TestFixture::rcu_type, typename TestFixture::int_item,
        typename ci::feldman_hashset::make_traits<
            ci::feldman_hashset::hash_accessor< typename TestFixture::hash_accessor >
            , ci::opt::less< std::less<int>>
            , ci::opt::disposer< typename TestFixture::mock_disposer>
        >::type
    > set_type;

    set_type s( 5, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveFeldmanHashSet, cmpmix )
{
    struct traits : public ci::feldman_hashset::traits
    {
        typedef typename TestFixture::hash_accessor hash_accessor;
        typedef typename TestFixture::cmp compare;
        typedef std::less<int> less;
        typedef typename TestFixture::mock_disposer disposer;
        typedef typename TestFixture::simple_item_counter item_counter;
    };

    typedef ci::FeldmanHashSet< typename TestFixture::rcu_type, typename TestFixture::int_item, traits > set_type;

    set_type s( 3, 4 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveFeldmanHashSet, backoff )
{
    struct traits : public ci::feldman_hashset::traits
    {
        typedef typename TestFixture::hash_accessor hash_accessor;
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::mock_disposer disposer;
        typedef cds::backoff::empty back_off;
        typedef ci::opt::v::sequential_consistent memory_model;
    };

    typedef ci::FeldmanHashSet< typename TestFixture::rcu_type, typename TestFixture::int_item, traits > set_type;

    set_type s( 8, 3 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveFeldmanHashSet, stat )
{
    struct traits : public ci::feldman_hashset::traits
    {
        typedef typename TestFixture::hash_accessor hash_accessor;
        typedef typename TestFixture::cmp compare;
        typedef typename TestFixture::mock_disposer disposer;
        typedef ci::feldman_hashset::stat<> stat;
    };

    typedef ci::FeldmanHashSet< typename TestFixture::rcu_type, typename TestFixture::int_item, traits > set_type;

    set_type s( 8, 3 );
    this->test( s );
}

// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( IntrusiveFeldmanHashSet,
    compare, less, cmpmix, backoff, stat
    );


#endif // #ifndef CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_RCU_H
