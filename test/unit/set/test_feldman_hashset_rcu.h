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

#ifndef CDSUNIT_SET_TEST_FELDMAN_HASHSET_RCU_H
#define CDSUNIT_SET_TEST_FELDMAN_HASHSET_RCU_H

#include "test_feldman_hashset.h"

#include <cds/container/feldman_hashset_rcu.h>

namespace {

    namespace cc = cds::container;

    template <typename RCU>
    class FeldmanHashSet: public cds_test::feldman_hashset
    {
        typedef cds_test::feldman_hashset base_class;

    protected:
        typedef cds::urcu::gc<RCU> rcu_type;

        template <typename Set>
        void test( Set& s )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            base_class::test( s );

            typedef typename Set::value_type value_type;

            size_t const nSetSize = kSize;
            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key)));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end());

            for ( auto& i : data ) {
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            typedef typename Set::rcu_lock rcu_lock;

            // iterator test
            {
                rcu_lock l;

                for ( auto it = s.begin(); it != s.end(); ++it ) {
                    it->nFindCount = it->key() * 3;
                }

                for ( auto it = s.cbegin(); it != s.cend(); ++it ) {
                    EXPECT_EQ( it->nFindCount, static_cast<size_t>( it->key() * 3 ));
                }

                for ( auto it = s.rbegin(); it != s.rend(); ++it ) {
                    it->nFindCount = it->key() * 2;
                }

                for ( auto it = s.crbegin(); it != s.crend(); ++it ) {
                    EXPECT_EQ( it->nFindCount, static_cast<size_t>( it->key() * 2 ));
                }
            }

            typedef typename Set::exempt_ptr exempt_ptr;

            // get()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                {
                    rcu_lock l;
                    value_type * p = s.get( i.key());
                    ASSERT_TRUE( p != nullptr );
                    EXPECT_EQ( p->key(), i.key());
                    EXPECT_EQ( p->nFindCount, static_cast<size_t>( i.key() * 2 ));
                    p->nFindCount *= 2;
                }
            }

            // extract()
            exempt_ptr xp;
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !xp );
                xp = s.extract( i.key());
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->key(), i.key());
                EXPECT_EQ( xp->nFindCount, static_cast<size_t>( i.key() * 4 ));

                xp = s.extract( i.key());
                ASSERT_TRUE( !xp );

                {
                    rcu_lock l;
                    value_type * p = s.get( i.key());
                    EXPECT_TRUE( p == nullptr );
                }
            }

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

    TYPED_TEST_CASE_P( FeldmanHashSet );

    TYPED_TEST_P( FeldmanHashSet, defaulted )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::get_hash get_hash;

        typedef cc::FeldmanHashSet< rcu_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
            >::type
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( FeldmanHashSet, compare )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::get_hash get_hash;

        typedef cc::FeldmanHashSet< rcu_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                , cds::opt::compare<  typename TestFixture::cmp >
            >::type
        > set_type;

        set_type s( 4, 5 );
        this->test( s );
    }

    TYPED_TEST_P( FeldmanHashSet, less )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::get_hash get_hash;

        typedef cc::FeldmanHashSet< rcu_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                , cds::opt::less< std::less<int> >
            >::type
        > set_type;

        set_type s( 3, 2 );
        this->test( s );
    }

    TYPED_TEST_P( FeldmanHashSet, cmpmix )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::get_hash get_hash;

        typedef cc::FeldmanHashSet< rcu_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                , cds::opt::less< std::less<int> >
                , cds::opt::compare<  typename TestFixture::cmp >
            >::type
        > set_type;

        set_type s( 4, 4 );
        this->test( s );
    }

    TYPED_TEST_P( FeldmanHashSet, item_counting )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::get_hash get_hash;

        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef typename TestFixture::cmp compare;
            typedef std::less<int> less;
            typedef typename TestFixture::simple_item_counter item_counter;
        };
        typedef cc::FeldmanHashSet< rcu_type, int_item, set_traits > set_type;

        set_type s( 3, 3 );
        this->test( s );
    }

    TYPED_TEST_P( FeldmanHashSet, backoff )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::get_hash get_hash;

        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef typename TestFixture::cmp compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::FeldmanHashSet< rcu_type, int_item, set_traits > set_type;

        set_type s( 8, 2 );
        this->test( s );
    }

    TYPED_TEST_P( FeldmanHashSet, stat )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::get_hash get_hash;

        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef cds::backoff::yield back_off;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< rcu_type, int_item, set_traits > set_type;

        set_type s( 1, 1 );
        this->test( s );
    }

    TYPED_TEST_P( FeldmanHashSet, explicit_hash_size )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item2 int_item;
        typedef typename TestFixture::get_hash2 get_hash2;

        struct set_traits: public cc::feldman_hashset::traits
        {
            enum: size_t {
                hash_size = sizeof( std::declval<int_item>().nKey )
            };
            typedef get_hash2 hash_accessor;
            typedef typename TestFixture::cmp2 compare;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< rcu_type, int_item, set_traits > set_type;

        set_type s( 8, 4 );
        this->test( s );
    }

    // GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
    // "No test named <test_name> can be found in this test case"
    REGISTER_TYPED_TEST_CASE_P( FeldmanHashSet,
        defaulted, compare, less, cmpmix, item_counting, backoff, stat, explicit_hash_size
        );
} // namespace

#endif // CDSUNIT_SET_TEST_FELDMAN_HASHSET_RCU_H
