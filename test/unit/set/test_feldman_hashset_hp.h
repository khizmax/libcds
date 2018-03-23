// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TEST_FELDMAN_HASHSET_HP_H
#define CDSUNIT_SET_TEST_FELDMAN_HASHSET_HP_H

#include "test_feldman_hashset.h"

namespace cds_test {

    class feldman_hashset_hp: public feldman_hashset
    {
        typedef feldman_hashset base_class;

    protected:
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

            // iterator test
            for ( auto it = s.begin(); it != s.end(); ++it ) {
                it->nFindCount = it->key() * 3;
                it.release();
            }

            for ( auto it = s.cbegin(); it != s.cend(); ++it ) {
                EXPECT_EQ( it->nFindCount, static_cast<size_t>( it->key() * 3 ));
            }

            // reverse iterator set
            for ( auto it = s.rbegin(); it != s.rend(); ++it ) {
                it->nFindCount = it->key() * 2;
            }

            for ( auto it = s.crbegin(); it != s.crend(); ++it ) {
                EXPECT_EQ( it->nFindCount, static_cast<size_t>( it->key() * 2 ));
            }

            typedef typename Set::guarded_ptr guarded_ptr;
            guarded_ptr gp;

            // get()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !gp );

                gp = s.get( i.key());
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->key(), i.key());
                EXPECT_EQ( gp->nFindCount, static_cast<size_t>( i.key() * 2 ));
                gp->nFindCount *= 2;

                gp.release();
            }

            // extract()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !gp );
                gp = s.extract( i.key());
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->key(), i.key());
                EXPECT_EQ( gp->nFindCount, static_cast<size_t>( i.key() * 4 ));

                gp = s.extract( i.key());
                ASSERT_TRUE( !gp );
            }

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            // erase_at()
            for ( auto& i : data ) {
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            for ( auto it = s.begin(); it != s.end(); ++it ) {
                int key = it->key();
                ASSERT_TRUE( s.erase_at( it ));
                EXPECT_EQ( it->key(), key );
                ASSERT_FALSE( s.erase_at( it ));
            }

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );
        }

    };
} // namespace cds_test

#endif // CDSUNIT_SET_TEST_FELDMAN_HASHSET_HP_H
