// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TEST_ORDERED_SET_HP_H
#define CDSUNIT_SET_TEST_ORDERED_SET_HP_H

#include "test_set_hp.h"

namespace cds_test {

    class container_ordered_set_hp: public container_set_hp
    {
        typedef container_set_hp base_class;

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

#endif // CDSUNIT_SET_TEST_ORDERED_SET_HP_H
