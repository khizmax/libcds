// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TEST_MAP_NOGC_H
#define CDSUNIT_MAP_TEST_MAP_NOGC_H

#include "test_map_data.h"

// forward declaration
namespace cds { namespace container {} }

namespace cds_test {

    class container_map_nogc: public map_fixture
    {
    public:
        static size_t const kSize = 1000;

    protected:
        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::value_type map_pair;
            typedef typename Map::iterator   iterator;
            size_t const kkSize = kSize;

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

            // insert/find
            for ( auto const& i : arrKeys ) {
                value_type const& val( arrVals.at( i.nKey ));

                ASSERT_FALSE( m.contains( i.nKey ) != m.end());
                ASSERT_FALSE( m.contains( i ) != m.end());
                ASSERT_FALSE( m.contains( other_item( i.nKey ), other_less()) != m.end());

                std::pair< iterator, bool > updResult;
                iterator it;
                switch ( i.nKey % 16 ) {
                case 0:
                    it = m.insert( i );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert( i ) == m.end());
                    it->second.nVal = it->first.nKey;
                    it->second.strVal = std::to_string( it->first.nKey );
                    break;
                case 1:
                    it = m.insert( i.nKey );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert( i.nKey ) == m.end());
                    it->second.nVal = it->first.nKey;
                    it->second.strVal = std::to_string( it->first.nKey );
                    break;
                case 2:
                    it = m.insert( std::to_string( i.nKey ));
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert( std::to_string( i.nKey )) == m.end());
                    it->second.nVal = it->first.nKey;
                    it->second.strVal = std::to_string( it->first.nKey );
                    break;
                case 3:
                    it = m.insert( i, val );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert( i, val ) == m.end());
                    break;
                case 4:
                    it = m.insert( i.nKey, val.strVal );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert( i.nKey, val.strVal ) == m.end());
                    break;
                case 5:
                    it = m.insert( val.strVal, i.nKey );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert( val.strVal, i.nKey ) == m.end());
                    break;
                case 6:
                    it = m.insert_with( i, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert_with( i, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ) == m.end());
                    break;
                case 7:
                    it = m.insert_with( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert_with( i.nKey, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ) == m.end());
                    break;
                case 8:
                    it = m.insert_with( val.strVal, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } );
                    ASSERT_FALSE( it == m.end());
                    ASSERT_TRUE( m.insert_with( val.strVal, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ) == m.end());
                    break;
                case 9:
                    updResult = m.update( i.nKey, false );
                    ASSERT_TRUE( updResult.first == m.end());
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i.nKey );
                    ASSERT_TRUE( updResult.first != m.end());
                    ASSERT_TRUE( updResult.second );
                    updResult.first->second.nVal = updResult.first->first.nKey;

                    updResult = m.update( i.nKey, false );
                    ASSERT_TRUE( updResult.first != m.end());
                    ASSERT_FALSE( updResult.second );
                    EXPECT_EQ( updResult.first->first.nKey, updResult.first->second.nVal );
                    updResult.first->second.strVal = std::to_string( updResult.first->second.nVal );
                    break;
                case 10:
                    updResult = m.update( i, false );
                    ASSERT_TRUE( updResult.first == m.end());
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i );
                    ASSERT_TRUE( updResult.first != m.end());
                    ASSERT_TRUE( updResult.second );
                    updResult.first->second.nVal = updResult.first->first.nKey;

                    updResult = m.update( i );
                    ASSERT_TRUE( updResult.first != m.end());
                    ASSERT_FALSE( updResult.second );
                    EXPECT_EQ( updResult.first->first.nKey, updResult.first->second.nVal );
                    updResult.first->second.strVal = std::to_string( updResult.first->second.nVal );
                    break;
                case 11:
                    updResult = m.update( val.strVal, false );
                    ASSERT_TRUE( updResult.first == m.end());
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( val.strVal );
                    ASSERT_TRUE( updResult.first != m.end());
                    ASSERT_TRUE( updResult.second );
                    updResult.first->second.nVal = updResult.first->first.nKey;

                    updResult = m.update( val.strVal, false );
                    ASSERT_TRUE( updResult.first != m.end());
                    ASSERT_FALSE( updResult.second );
                    EXPECT_EQ( updResult.first->first.nKey, updResult.first->second.nVal );
                    updResult.first->second.strVal = std::to_string( updResult.first->second.nVal );
                    break;
                case 12:
                    it = m.emplace( i.nKey );
                    ASSERT_TRUE( it != m.end());
                    ASSERT_FALSE( m.emplace( i.nKey ) != m.end());
                    it->second.nVal = it->first.nKey;
                    it->second.strVal = std::to_string( it->first.nKey );
                    break;
                case 13:
                    it = m.emplace( i, i.nKey );
                    ASSERT_TRUE( it != m.end());
                    ASSERT_FALSE( m.emplace( i, i.nKey ) != m.end());
                    break;
                case 14:
                    {
                        std::string str = val.strVal;
                        it = m.emplace( i, std::move( str ));
                        ASSERT_TRUE( it != m.end());
                        ASSERT_TRUE( str.empty());
                        str = val.strVal;
                        ASSERT_FALSE( m.emplace( i, std::move( str )) != m.end());
                        ASSERT_TRUE( str.empty());
                    }
                    break;
                case 15:
                    {
                        std::string str = val.strVal;
                        it = m.emplace( i, i.nKey, std::move( str ));
                        ASSERT_TRUE( it != m.end());
                        ASSERT_TRUE( str.empty());
                        str = val.strVal;
                        ASSERT_FALSE( m.emplace( i, i.nKey, std::move( str )) != m.end());
                        ASSERT_TRUE( str.empty());
                    }
                    break;
                }

                it = m.contains( i.nKey );
                ASSERT_TRUE( it != m.end());
                ASSERT_TRUE( m.contains( i ) == it );
                ASSERT_TRUE( m.contains( other_item( i.nKey ), other_less()) == it );
                EXPECT_EQ( it->first.nKey, it->second.nVal );
                EXPECT_EQ( std::to_string( it->first.nKey ), it->second.strVal );
            }
            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );
            ASSERT_FALSE( m.begin() == m.end());
            ASSERT_FALSE( m.cbegin() == m.cend());

            // clear

            m.clear();

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );
            ASSERT_TRUE( m.begin() == m.end());
            ASSERT_TRUE( m.cbegin() == m.cend());
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_MAP_TEST_MAP_NOGC_H
