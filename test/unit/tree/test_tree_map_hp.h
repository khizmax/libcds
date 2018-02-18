// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_TREE_TEST_TREE_MAP_HP_H
#define CDSUNIT_TREE_TEST_TREE_MAP_HP_H

#include "test_tree_map.h"

namespace cds_test {

    class container_tree_map_hp: public container_tree_map
    {
        typedef container_tree_map base_class;

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

            // get/extract
            typedef typename Map::guarded_ptr guarded_ptr;
            guarded_ptr gp;

            for ( auto const& i : arrKeys ) {
                value_type const& val = arrVals.at( i.nKey );

                gp = m.get( i.nKey );
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );
                gp.release();
                gp = m.get( i );
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );
                gp.release();
                gp = m.get_with( other_item( i.nKey ), other_less());
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );

                switch ( i.nKey % 4 ) {
                case 0:
                    gp = m.extract( i.nKey );
                    break;
                case 1:
                    gp = m.extract( i );
                    break;
                case 2:
                    gp = m.extract( val.strVal );
                    break;
                case 3:
                    gp = m.extract_with( other_item( i.nKey ), other_less());
                    break;
                }
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );
                gp.release();

                gp = m.get( i.nKey );
                ASSERT_TRUE( !gp );
                gp = m.get( i );
                ASSERT_TRUE( !gp );
                gp = m.get_with( other_item( i.nKey ), other_less());
                ASSERT_TRUE( !gp );
            }
            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            gp = m.extract_min();
            EXPECT_TRUE( !gp );
            gp = m.extract_max();
            EXPECT_TRUE( !gp );

            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ));
            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );

            // extract_min
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

            ASSERT_TRUE( m.check_consistency());
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_TREE_TEST_TREE_MAP_HP_H
