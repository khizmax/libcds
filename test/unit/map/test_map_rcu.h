// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TEST_MAP_RCU_H
#define CDSUNIT_MAP_TEST_MAP_RCU_H

#include "test_map.h"

namespace cds_test {

    class container_map_rcu: public container_map
    {
        typedef container_map base_class;

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

            // iterators
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

            typedef typename Map::exempt_ptr exempt_ptr;
            typedef typename Map::raw_ptr    raw_ptr;
            typedef typename Map::rcu_lock   rcu_lock;

            // get/extract
            shuffle( arrKeys.begin(), arrKeys.end());

            for ( auto const& i : arrKeys ) {
                value_type const& val = arrVals.at( i.nKey );

                {
                    rcu_lock l;
                    raw_ptr rp;

                    rp = m.get( i.nKey );
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->first.nKey, i.nKey );
                    rp->second.nVal = rp->first.nKey * 2;

                    rp = m.get( i );
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->first.nKey, i.nKey );
                    EXPECT_EQ( rp->second.nVal, rp->first.nKey * 2 );
                    rp->second.nVal = rp->first.nKey * 3;

                    rp = m.get_with( other_item( i.nKey ), other_less());
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->first.nKey, i.nKey );
                    EXPECT_EQ( rp->second.nVal, rp->first.nKey * 3 );
                    rp->second.nVal = rp->first.nKey;
                }

                exempt_ptr xp;
                if ( Map::c_bExtractLockExternal ) {
                    {
                        rcu_lock l;

                        switch ( i.nKey % 4 ) {
                        case 0:
                            xp = m.extract( i.nKey );
                            break;
                        case 1:
                            xp = m.extract( i );
                            break;
                        case 2:
                            xp = m.extract( val.strVal );
                            break;
                        case 3:
                            xp = m.extract_with( other_item( i.nKey ), other_less());
                            break;
                        }
                        ASSERT_FALSE( !xp );
                        EXPECT_EQ( xp->first.nKey, i.nKey );
                        EXPECT_EQ( xp->second.nVal, xp->first.nKey );
                    }
                    xp.release();

                    {
                        rcu_lock l;

                        switch ( i.nKey % 4 ) {
                        case 0:
                            xp = m.extract( i.nKey );
                            break;
                        case 1:
                            xp = m.extract( i );
                            break;
                        case 2:
                            xp = m.extract( val.strVal );
                            break;
                        case 3:
                            xp = m.extract_with( other_item( i.nKey ), other_less());
                            break;
                        }
                        EXPECT_TRUE( !xp );
                    }
                }
                else {
                    switch ( i.nKey % 4 ) {
                    case 0:
                        xp = m.extract( i.nKey );
                        break;
                    case 1:
                        xp = m.extract( i );
                        break;
                    case 2:
                        xp = m.extract( val.strVal );
                        break;
                    case 3:
                        xp = m.extract_with( other_item( i.nKey ), other_less());
                        break;
                    }
                    ASSERT_FALSE( !xp );
                    EXPECT_EQ( xp->first.nKey, i.nKey );
                    EXPECT_EQ( xp->second.nVal, xp->first.nKey );

                    switch ( i.nKey % 4 ) {
                    case 0:
                        xp = m.extract( i.nKey );
                        break;
                    case 1:
                        xp = m.extract( i );
                        break;
                    case 2:
                        xp = m.extract( val.strVal );
                        break;
                    case 3:
                        xp = m.extract_with( other_item( i.nKey ), other_less());
                        break;
                    }
                    EXPECT_TRUE( !xp );
                }

                {
                    rcu_lock l;
                    raw_ptr rp;

                    rp = m.get( i.nKey );
                    ASSERT_TRUE( !rp );
                    rp = m.get( i );
                    ASSERT_TRUE( !rp );
                    rp = m.get_with( other_item( i.nKey ), other_less());
                    ASSERT_TRUE( !rp );
                }
            }

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_MAP_TEST_MAP_H
