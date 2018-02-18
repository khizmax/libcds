// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TEST_SKIPLIST_HP_H
#define CDSUNIT_MAP_TEST_SKIPLIST_HP_H

#include "test_map_hp.h"

namespace cds_test {

    class skiplist_map_hp: public container_map_hp
    {
        typedef container_map_hp base_class;

    protected:
        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            base_class::test( m );

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            size_t const kkSize = base_class::kSize;

            std::vector<key_type> arrKeys;
            for ( int i = 0; i < static_cast<int>(kkSize); ++i )
                arrKeys.push_back( key_type( i ));
            shuffle( arrKeys.begin(), arrKeys.end());

            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ));
            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );

            // extract_min
            typedef typename Map::guarded_ptr guarded_ptr;
            guarded_ptr gp;

            int nKey = -1;
            size_t nCount = 0;
            while ( !m.empty()) {
                gp = m.extract_min();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, nKey + 1 );
                nKey = gp->first.nKey;
                ++nCount;
            }
            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );
            EXPECT_EQ( nCount, kkSize );

            // extract_max
            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ));
            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );

            nKey = kkSize;
            nCount = 0;
            while ( !m.empty()) {
                gp = m.extract_max();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, nKey - 1 );
                nKey = gp->first.nKey;
                ++nCount;
            }
            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );
            EXPECT_EQ( nCount, kkSize );
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_MAP_TEST_SKIPLIST_HP_H
