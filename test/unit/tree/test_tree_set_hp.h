// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TEST_TREE_SET_HP_H
#define CDSUNIT_SET_TEST_TREE_SET_HP_H

#include "test_tree_set.h"

namespace cds_test {

    class container_tree_set_hp: public container_tree_set
    {
        typedef container_tree_set base_class;

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

            for ( auto i : indices ) {
                ASSERT_TRUE( s.insert( data[i] ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            typedef typename Set::guarded_ptr guarded_ptr;
            guarded_ptr gp;

            // get()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !gp );
                switch ( idx % 3 ) {
                case 0:
                    gp = s.get( i.key());
                    ASSERT_FALSE( !gp );
                    break;
                case 1:
                    gp = s.get( i );
                    ASSERT_FALSE( !gp );
                    break;
                case 2:
                    gp = s.get_with( other_item( i.key()), other_less());
                    ASSERT_FALSE( !gp );
                }
                EXPECT_EQ( gp->key(), i.key());
                gp->nFindCount = gp->key() * 3;

                gp.release();
            }

            // extract()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !gp );
                switch ( idx % 3 ) {
                case 0:
                    gp = s.extract( i.key());
                    ASSERT_FALSE( !gp );
                    break;
                case 1:
                    gp = s.extract( i );
                    ASSERT_FALSE( !gp );
                    break;
                case 2:
                    gp = s.extract_with( other_item( i.key()), other_less());
                    ASSERT_FALSE( !gp );
                    break;
                }
                EXPECT_EQ( gp->key(), i.key());
                EXPECT_EQ( gp->nFindCount, static_cast<size_t>( i.key() * 3 ));

                switch ( idx % 3 ) {
                case 0:
                    gp = s.extract( i.key());
                    break;
                case 1:
                    gp = s.extract( i );
                    break;
                case 2:
                    gp = s.extract_with( other_item( i.key()), other_less());
                    break;
                }
                ASSERT_TRUE( !gp );
            }

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            for ( auto i : indices ) {
                ASSERT_TRUE( s.insert( data[i] ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            // extract_min
            size_t nCount = 0;
            int nKey = -1;
            while ( !s.empty()) {
                gp = s.extract_min();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( nKey + 1, gp->key());
                ++nCount;
                nKey = gp->key();
            }
            gp.release();
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
                gp = s.extract_max();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( nKey - 1, gp->key());
                ++nCount;
                nKey = gp->key();
            }
            gp.release();
            EXPECT_EQ( nCount, nSetSize );

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

        }

    };
} // namespace cds_test

#endif // CDSUNIT_SET_TEST_TREE_SET_HP_H
