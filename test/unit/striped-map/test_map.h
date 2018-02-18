// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_STRIPED_MAP_TEST_MAP_H
#define CDSUNIT_STRIPED_MAP_TEST_MAP_H

#include "test_map_data.h"

// forward declaration
namespace cds { namespace container {} }

namespace cds_test {

    class container_map: public striped_map_fixture
    {
    public:
        static size_t const kSize = 1000;

    protected:
        template <class Map>
        void test( Map& m )
        {
            test_< true >( m );
        }

        template <bool Sorted, class Map>
        void test_( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::value_type map_pair;
            typedef typename std::conditional< Sorted, other_less, other_equal_to >::type other_predicate;

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

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.contains( other_item( i.nKey ), other_predicate()));
                ASSERT_FALSE( m.find( i, []( map_pair const& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find_with( other_item( i.nKey ), other_predicate(), []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));

                std::pair< bool, bool > updResult;

                switch ( i.nKey % 16 ) {
                case 0:
                    ASSERT_TRUE( m.insert( i ));
                    ASSERT_FALSE( m.insert( i ));
                    ASSERT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 1:
                    ASSERT_TRUE( m.insert( i.nKey ));
                    ASSERT_FALSE( m.insert( i.nKey ));
                    ASSERT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 2:
                    ASSERT_TRUE( m.insert( std::to_string( i.nKey )));
                    ASSERT_FALSE( m.insert( std::to_string( i.nKey )));
                    ASSERT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 3:
                    ASSERT_TRUE( m.insert( i, val ));
                    ASSERT_FALSE( m.insert( i, val ));
                    break;
                case 4:
                    ASSERT_TRUE( m.insert( i.nKey, val.strVal ));
                    ASSERT_FALSE( m.insert( i.nKey, val.strVal ));
                    break;
                case 5:
                    ASSERT_TRUE( m.insert( val.strVal, i.nKey ));
                    ASSERT_FALSE( m.insert( val.strVal, i.nKey ));
                    break;
                case 6:
                    ASSERT_TRUE( m.insert_with( i, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( i, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 7:
                    ASSERT_TRUE( m.insert_with( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( i.nKey, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 8:
                    ASSERT_TRUE( m.insert_with( val.strVal, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( val.strVal, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 9:
                    updResult = m.update( i.nKey, []( bool, map_pair& ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i.nKey, []( bool bNew, map_pair& v ) {
                        EXPECT_TRUE( bNew );
                        v.second.nVal = v.first.nKey;
                    });
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i.nKey, []( bool bNew, map_pair& v ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 10:
                    updResult = m.update( i, []( bool, map_pair& ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i, []( bool bNew, map_pair& v ) {
                        EXPECT_TRUE( bNew );
                        v.second.nVal = v.first.nKey;
                    });
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i, []( bool bNew, map_pair& v ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 11:
                    updResult = m.update( val.strVal, []( bool, map_pair& ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( val.strVal, []( bool bNew, map_pair& v ) {
                        EXPECT_TRUE( bNew );
                        v.second.nVal = v.first.nKey;
                    });
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( val.strVal, []( bool bNew, map_pair& v ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 12:
                    ASSERT_TRUE( m.emplace( i.nKey ));
                    ASSERT_FALSE( m.emplace( i.nKey ));
                    ASSERT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 13:
                    ASSERT_TRUE( m.emplace( i, i.nKey ));
                    ASSERT_FALSE( m.emplace( i, i.nKey ));
                    break;
                case 14:
                    {
                        std::string str = val.strVal;
                        ASSERT_TRUE( m.emplace( i, std::move( str )));
                        ASSERT_TRUE( str.empty());
                        str = val.strVal;
                        ASSERT_FALSE( m.emplace( i, std::move( str )));
                        ASSERT_TRUE( str.empty());
                    }
                    break;
                case 15:
                    {
                        std::string str = val.strVal;
                        ASSERT_TRUE( m.emplace( i, i.nKey, std::move( str )));
                        ASSERT_TRUE( str.empty());
                        str = val.strVal;
                        ASSERT_FALSE( m.emplace( i, i.nKey, std::move( str )));
                        ASSERT_TRUE( str.empty());
                    }
                    break;
                }

                ASSERT_TRUE( m.contains( i.nKey ));
                ASSERT_TRUE( m.contains( i ));
                ASSERT_TRUE( m.contains( other_item( i.nKey ), other_predicate()));
                ASSERT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find_with( other_item( i.nKey ), other_predicate(), []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
            }
            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );

            shuffle( arrKeys.begin(), arrKeys.end());

            // erase/find
            for ( auto const& i : arrKeys ) {
                value_type const& val( arrVals.at( i.nKey ));

                ASSERT_TRUE( m.contains( i.nKey ));
                ASSERT_TRUE( m.contains( val.strVal ));
                ASSERT_TRUE( m.contains( i ));
                ASSERT_TRUE( m.contains( other_item( i.nKey ), other_predicate()));
                ASSERT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find_with( other_item( i.nKey ), other_predicate(), []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));


                switch ( i.nKey % 8 ) {
                case 0:
                    ASSERT_TRUE( m.erase( i ));
                    ASSERT_FALSE( m.erase( i ));
                    break;
                case 1:
                    ASSERT_TRUE( m.erase( i.nKey ));
                    ASSERT_FALSE( m.erase( i.nKey ));
                    break;
                case 2:
                    ASSERT_TRUE( m.erase( val.strVal ));
                    ASSERT_FALSE( m.erase( val.strVal ));
                    break;
                case 3:
                    ASSERT_TRUE( m.erase_with( other_item( i.nKey ), other_predicate()));
                    ASSERT_FALSE( m.erase_with( other_item( i.nKey ), other_predicate()));
                    break;
                case 4:
                    ASSERT_TRUE( m.erase( i, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    ASSERT_FALSE( m.erase( i, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 5:
                    ASSERT_TRUE( m.erase( i.nKey, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    ASSERT_FALSE( m.erase( i.nKey, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 6:
                    ASSERT_TRUE( m.erase( val.strVal, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    ASSERT_FALSE( m.erase( val.strVal, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 7:
                    ASSERT_TRUE( m.erase_with( other_item( i.nKey ), other_predicate(), []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    ASSERT_FALSE( m.erase_with( other_item( i.nKey ), other_predicate(), []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                }

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.contains( val.strVal ));
                ASSERT_FALSE( m.contains( other_item( i.nKey ), other_predicate()));
                ASSERT_FALSE( m.find( i, []( map_pair const& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find_with( other_item( i.nKey ), other_predicate(), []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
            }
            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            // clear
            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ));

            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );

            m.clear();

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_STRIPED_MAP_TEST_MAP_H
