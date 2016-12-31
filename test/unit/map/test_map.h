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

#ifndef CDSUNIT_MAP_TEST_MAP_H
#define CDSUNIT_MAP_TEST_MAP_H

#include "test_map_data.h"

// forward declaration
namespace cds { namespace container {} }

namespace cds_test {

    class container_map: public map_fixture
    {
    public:
        static size_t const kSize = 1000;

    protected:
        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            EXPECT_TRUE( m.empty());
            EXPECT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::value_type map_pair;
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

                EXPECT_FALSE( m.contains( i.nKey ));
                EXPECT_FALSE( m.contains( i ));
                EXPECT_FALSE( m.contains( other_item( i.nKey ), other_less()));
                EXPECT_FALSE( m.find( i, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                EXPECT_FALSE( m.find( i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                EXPECT_FALSE( m.find_with( other_item( i.nKey ), other_less(), []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));

                std::pair< bool, bool > updResult;

                switch ( i.nKey % 16 ) {
                case 0:
                    EXPECT_TRUE( m.insert( i ));
                    EXPECT_FALSE( m.insert( i ));
                    EXPECT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 1:
                    EXPECT_TRUE( m.insert( i.nKey ));
                    EXPECT_FALSE( m.insert( i.nKey ));
                    EXPECT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 2:
                    EXPECT_TRUE( m.insert( std::to_string( i.nKey )));
                    EXPECT_FALSE( m.insert( std::to_string( i.nKey )));
                    EXPECT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 3:
                    EXPECT_TRUE( m.insert( i, val ));
                    EXPECT_FALSE( m.insert( i, val ));
                    break;
                case 4:
                    EXPECT_TRUE( m.insert( i.nKey, val.strVal ));
                    EXPECT_FALSE( m.insert( i.nKey, val.strVal ));
                    break;
                case 5:
                    EXPECT_TRUE( m.insert( val.strVal, i.nKey ));
                    EXPECT_FALSE( m.insert( val.strVal, i.nKey ));
                    break;
                case 6:
                    EXPECT_TRUE( m.insert_with( i, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    EXPECT_FALSE( m.insert_with( i, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 7:
                    EXPECT_TRUE( m.insert_with( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    EXPECT_FALSE( m.insert_with( i.nKey, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 8:
                    EXPECT_TRUE( m.insert_with( val.strVal, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    EXPECT_FALSE( m.insert_with( val.strVal, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 9:
                    updResult = m.update( i.nKey, []( bool, map_pair& ) {
                        EXPECT_TRUE( false );
                    }, false );
                    EXPECT_FALSE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    updResult = m.update( i.nKey, []( bool bNew, map_pair& v ) {
                        EXPECT_TRUE( bNew );
                        v.second.nVal = v.first.nKey;
                    });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = m.update( i.nKey, []( bool bNew, map_pair& v ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 10:
                    updResult = m.update( i, []( bool, map_pair& ) {
                        EXPECT_TRUE( false );
                    }, false );
                    EXPECT_FALSE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    updResult = m.update( i, []( bool bNew, map_pair& v ) {
                        EXPECT_TRUE( bNew );
                        v.second.nVal = v.first.nKey;
                    });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = m.update( i, []( bool bNew, map_pair& v ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 11:
                    updResult = m.update( val.strVal, []( bool, map_pair& ) {
                        EXPECT_TRUE( false );
                    }, false );
                    EXPECT_FALSE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    updResult = m.update( val.strVal, []( bool bNew, map_pair& v ) {
                        EXPECT_TRUE( bNew );
                        v.second.nVal = v.first.nKey;
                    });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = m.update( val.strVal, []( bool bNew, map_pair& v ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 12:
                    EXPECT_TRUE( m.emplace( i.nKey ));
                    EXPECT_FALSE( m.emplace( i.nKey ));
                    EXPECT_TRUE( m.find( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    break;
                case 13:
                    EXPECT_TRUE( m.emplace( i, i.nKey ));
                    EXPECT_FALSE( m.emplace( i, i.nKey ));
                    break;
                case 14:
                    {
                        std::string str = val.strVal;
                        EXPECT_TRUE( m.emplace( i, std::move( str )));
                        EXPECT_TRUE( str.empty());
                        str = val.strVal;
                        EXPECT_FALSE( m.emplace( i, std::move( str )));
                        EXPECT_TRUE( str.empty());
                    }
                    break;
                case 15:
                    {
                        std::string str = val.strVal;
                        EXPECT_TRUE( m.emplace( i, i.nKey, std::move( str )));
                        EXPECT_TRUE( str.empty());
                        str = val.strVal;
                        EXPECT_FALSE( m.emplace( i, i.nKey, std::move( str )));
                        EXPECT_TRUE( str.empty());
                    }
                    break;
                }

                EXPECT_TRUE( m.contains( i.nKey ));
                EXPECT_TRUE( m.contains( i ));
                EXPECT_TRUE( m.contains( other_item( i.nKey ), other_less()));
                EXPECT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                EXPECT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                EXPECT_TRUE( m.find_with( other_item( i.nKey ), other_less(), []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
            }
            EXPECT_FALSE( m.empty());
            EXPECT_CONTAINER_SIZE( m, kkSize );
            EXPECT_FALSE( m.begin() == m.end());
            EXPECT_FALSE( m.cbegin() == m.cend());

            shuffle( arrKeys.begin(), arrKeys.end());

            // erase/find
            for ( auto const& i : arrKeys ) {
                value_type const& val( arrVals.at( i.nKey ));

                EXPECT_TRUE( m.contains( i.nKey ));
                EXPECT_TRUE( m.contains( val.strVal ));
                EXPECT_TRUE( m.contains( i ));
                EXPECT_TRUE( m.contains( other_item( i.nKey ), other_less()));
                EXPECT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                EXPECT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                EXPECT_TRUE( m.find_with( other_item( i.nKey ), other_less(), []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));


                switch ( i.nKey % 8 ) {
                case 0:
                    EXPECT_TRUE( m.erase( i ));
                    EXPECT_FALSE( m.erase( i ));
                    break;
                case 1:
                    EXPECT_TRUE( m.erase( i.nKey ));
                    EXPECT_FALSE( m.erase( i.nKey ));
                    break;
                case 2:
                    EXPECT_TRUE( m.erase( val.strVal ));
                    EXPECT_FALSE( m.erase( val.strVal ));
                    break;
                case 3:
                    EXPECT_TRUE( m.erase_with( other_item( i.nKey ), other_less()));
                    EXPECT_FALSE( m.erase_with( other_item( i.nKey ), other_less()));
                    break;
                case 4:
                    EXPECT_TRUE( m.erase( i, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    EXPECT_FALSE( m.erase( i, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 5:
                    EXPECT_TRUE( m.erase( i.nKey, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    EXPECT_FALSE( m.erase( i.nKey, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 6:
                    EXPECT_TRUE( m.erase( val.strVal, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    EXPECT_FALSE( m.erase( val.strVal, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 7:
                    EXPECT_TRUE( m.erase_with( other_item( i.nKey ), other_less(), []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    }));
                    EXPECT_FALSE( m.erase_with( other_item( i.nKey ), other_less(), []( map_pair& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                }

                EXPECT_FALSE( m.contains( i.nKey ));
                EXPECT_FALSE( m.contains( i ));
                EXPECT_FALSE( m.contains( val.strVal ));
                EXPECT_FALSE( m.contains( other_item( i.nKey ), other_less()));
                EXPECT_FALSE( m.find( i, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                EXPECT_FALSE( m.find( i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                EXPECT_FALSE( m.find_with( other_item( i.nKey ), other_less(), []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
            }
            EXPECT_TRUE( m.empty());
            EXPECT_CONTAINER_SIZE( m, 0 );

            EXPECT_TRUE( m.begin() == m.end());
            EXPECT_TRUE( m.cbegin() == m.cend());

            // clear
            for ( auto const& i : arrKeys )
                EXPECT_TRUE( m.insert( i ));

            EXPECT_FALSE( m.empty());
            EXPECT_CONTAINER_SIZE( m, kkSize );

            m.clear();

            EXPECT_TRUE( m.empty());
            EXPECT_CONTAINER_SIZE( m, 0 );
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_MAP_TEST_MAP_H
