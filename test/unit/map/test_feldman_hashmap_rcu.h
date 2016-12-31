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

#ifndef CDSUNIT_MAP_TEST_FELDMAN_HASHMAP_RCU_H
#define CDSUNIT_MAP_TEST_FELDMAN_HASHMAP_RCU_H

#include "test_feldman_hashmap.h"
#include <cds/container/feldman_hashmap_rcu.h>

namespace {

    template <typename RCU>
    class FeldmanHashMap: public cds_test::feldman_hashmap
    {
        typedef cds_test::feldman_hashmap base_class;

    protected:
        typedef cds::urcu::gc<RCU> rcu_type;

        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            base_class::test( m );

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::key_type key_type;
            typedef typename Map::mapped_type value_type;
            typedef typename Map::value_type map_pair;
            typedef typename Map::rcu_lock   rcu_lock;
            typedef typename Map::exempt_ptr exempt_ptr;

            size_t const kkSize = base_class::kSize;

            std::vector<key_type> arrKeys;
            for ( int i = 0; i < static_cast<int>(kkSize); ++i )
                arrKeys.push_back( key_type( i ));
            shuffle( arrKeys.begin(), arrKeys.end());

            std::vector< value_type > arrVals;
            for ( size_t i = 0; i < kkSize; ++i ) {
                value_type val;
                val.nVal = static_cast<int>( i );
                val.strVal = std::to_string( i );
                arrVals.push_back( val );
            }

            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ));
            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );

            // iterators
            {
                rcu_lock l;
                size_t nCount = 0;
                for ( auto it = m.begin(); it != m.end(); ++it ) {
                    EXPECT_EQ( it->second.nVal, 0 );
                    it->second.nVal = it->first.nKey * 2;
                    ++nCount;
                }
                EXPECT_EQ( nCount, kkSize );

                nCount = 0;
                for ( auto it = m.cbegin(); it != m.cend(); ++it ) {
                    EXPECT_EQ( it->second.nVal, it->first.nKey * 2 );
                    ++nCount;
                }
                EXPECT_EQ( nCount, kkSize );

                nCount = 0;
                for ( auto it = m.rbegin(); it != m.rend(); ++it ) {
                    EXPECT_EQ( it->second.nVal, it->first.nKey * 2 );
                    it->second.nVal = it->first.nKey * 4;
                    ++nCount;
                }
                EXPECT_EQ( nCount, kkSize );

                nCount = 0;
                for ( auto it = m.crbegin(); it != m.crend(); ++it ) {
                    EXPECT_EQ( it->second.nVal, it->first.nKey * 4 );
                    ++nCount;
                }
                EXPECT_EQ( nCount, kkSize );
            }

            // get/extract
            exempt_ptr xp;

            for ( auto const& i : arrKeys ) {
                value_type const& val = arrVals.at( i.nKey );

                {
                    rcu_lock l;
                    map_pair * p = m.get( i.nKey );
                    ASSERT_FALSE( p == nullptr );
                    EXPECT_EQ( p->first.nKey, i.nKey );

                    p = m.get( i );
                    ASSERT_FALSE( p == nullptr );
                    EXPECT_EQ( p->first.nKey, i.nKey );

                    p = m.get( val.strVal );
                    ASSERT_FALSE( p == nullptr );
                    EXPECT_EQ( p->first.nKey, i.nKey );
                }

                switch ( i.nKey % 3 ) {
                case 0:
                    xp = m.extract( i.nKey );
                    break;
                case 1:
                    xp = m.extract( i );
                    break;
                case 2:
                    xp = m.extract( val.strVal );
                    break;
                }
                ASSERT_FALSE( !xp );
                ASSERT_EQ( xp->first.nKey, i.nKey );

                {
                    rcu_lock l;

                    map_pair * p = m.get( i.nKey );
                    EXPECT_TRUE( p == nullptr );
                    p = m.get( i );
                    EXPECT_TRUE( p == nullptr );
                }
            }
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

    TYPED_TEST_CASE_P( FeldmanHashMap );

    TYPED_TEST_P( FeldmanHashMap, defaulted )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::FeldmanHashMap< rcu_type, key_type, value_type > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( FeldmanHashMap, compare )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::FeldmanHashMap< rcu_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::compare< typename TestFixture::cmp >
            >::type
        > map_type;

        map_type m( 4, 5 );
        this->test( m );
    }

    TYPED_TEST_P( FeldmanHashMap, less )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::FeldmanHashMap< rcu_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::less< typename TestFixture::less >
            >::type
        > map_type;

        map_type m( 3, 2 );
        this->test( m );
    }

    TYPED_TEST_P( FeldmanHashMap, cmpmix )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::FeldmanHashMap< rcu_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::less< typename TestFixture::less >
                , cds::opt::compare<  typename TestFixture::cmp >
            >::type
        > map_type;

        map_type m( 4, 4 );
        this->test( m );
    }

    TYPED_TEST_P( FeldmanHashMap, backoff )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::feldman_hashmap::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::FeldmanHashMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m( 8, 2 );
        this->test( m );
    }

    TYPED_TEST_P( FeldmanHashMap, stat )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::feldman_hashmap::traits
        {
            typedef cds::backoff::yield back_off;
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef cc::FeldmanHashMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m( 1, 1 );
        this->test( m );
    }

    TYPED_TEST_P( FeldmanHashMap, explicit_key_size )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type2  key_type2;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::feldman_hashmap::traits
        {
            enum: size_t {
                hash_size = sizeof( int ) + sizeof( uint16_t )
            };
            typedef typename TestFixture::hash2 hash;
            typedef typename TestFixture::less2 less;
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef cc::FeldmanHashMap< rcu_type, key_type2, value_type, map_traits > map_type;

        map_type m( 5, 3 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>(1 << 6));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>(1 << 3));
        this->test( m );
    }

    // GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
    // "No test named <test_name> can be found in this test case"
    REGISTER_TYPED_TEST_CASE_P( FeldmanHashMap,
        defaulted, compare, less, cmpmix, backoff, stat, explicit_key_size
        );
} // namespace

#endif // #ifndef CDSUNIT_MAP_TEST_FELDMAN_HASHMAP_RCU_H
