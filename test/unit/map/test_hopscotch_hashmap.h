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

#ifndef CDSUNIT_MAP_TEST_HOPSCOTCHHASHMAP_H_
#define CDSUNIT_MAP_TEST_HOPSCOTCHHASHMAP_H_

#include "test_map_data.h"

// forward declaration
namespace cds { namespace container {}}
namespace co = cds::opt;
namespace cc = cds::container;

namespace cds_test {

    class hopscotch_hashmap : public map_fixture
    {
    public:
        static size_t const kSize = 1000;

        struct key_type2 {
            int         nKey;
            uint16_t    subkey;

            explicit key_type2( int n )
                : nKey( n )
                , subkey( static_cast<uint16_t>( n ))
            {}

            explicit key_type2( size_t n )
                : nKey( static_cast<int>( n ))
                , subkey( static_cast<uint16_t>( n ))
            {}

            explicit key_type2( std::string const& str )
                : nKey( std::stoi( str ))
                , subkey( static_cast<uint16_t>( nKey ))
            {}

            key_type2( key_type2 const& s )
                : nKey( s.nKey )
                , subkey( s.subkey )
            {}
        };

        struct less2
        {
            bool operator ()( key_type2 const& v1, key_type2 const& v2 ) const
            {
                return v1.nKey < v2.nKey;
            }

            bool operator ()( key_type2 const& v1, int v2 ) const
            {
                return v1.nKey < v2;
            }

            bool operator ()( int v1, key_type2 const& v2 ) const
            {
                return v1 < v2.nKey;
            }

            bool operator ()( key_type2 const& v1, std::string const& v2 ) const
            {
                return v1.nKey < std::stoi( v2 );
            }

            bool operator ()( std::string const& v1, key_type2 const& v2 ) const
            {
                return std::stoi( v1 ) < v2.nKey;
            }
        };

        struct cmp2 {
            int operator ()( key_type2 const& v1, key_type2 const& v2 ) const
            {
                if ( v1.nKey < v2.nKey )
                    return -1;
                return v1.nKey > v2.nKey ? 1 : 0;
            }

            int operator ()( key_type2 const& v1, int v2 ) const
            {
                if ( v1.nKey < v2 )
                    return -1;
                return v1.nKey > v2 ? 1 : 0;
            }

            int operator ()( int v1, key_type2 const& v2 ) const
            {
                if ( v1 < v2.nKey )
                    return -1;
                return v1 > v2.nKey ? 1 : 0;
            }

            int operator ()( key_type2 const& v1, std::string const& v2 ) const
            {
                int n2 = std::stoi( v2 );
                if ( v1.nKey < n2 )
                    return -1;
                return v1.nKey > n2 ? 1 : 0;
            }

            int operator ()( std::string const& v1, key_type2 const& v2 ) const
            {
                int n1 = std::stoi( v1 );
                if ( n1 < v2.nKey )
                    return -1;
                return n1 > v2.nKey ? 1 : 0;
            }
        };

        struct hash2 {
            key_type2 operator()( int i ) const
            {
                return key_type2( cds::opt::v::hash<int>()(i));
            }

            key_type2 operator()( std::string const& str ) const
            {
                return key_type2( cds::opt::v::hash<int>()(std::stoi( str )));
            }

            template <typename T>
            key_type2 operator()( T const& i ) const
            {
                return key_type2( cds::opt::v::hash<int>()(i.nKey));
            }
        };


    protected:
        template <typename Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::key_type key_type;
            typedef typename Map::mapped_type value_type;
            typedef typename Map::value_type map_pair;
            size_t const kkSize = kSize;

            std::vector<key_type> arrKeys;
            for ( int i = 0; i < static_cast<int>(kkSize); ++i )
                arrKeys.push_back( key_type( i ));
            shuffle( arrKeys.begin(), arrKeys.end());

            std::vector< value_type > arrVals;
            for ( size_t i = 0; i < kkSize; ++i ) {
                value_type val;
                val.nVal = static_cast<int>(i);
                val.strVal = std::to_string( i );
                arrVals.push_back( val );
            }

            // insert/find
            for ( auto const& i : arrKeys ) {
                value_type const& val( arrVals.at( i.nKey ));

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.find( i, []( map_pair const& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( map_pair const& ) {
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
                    updResult = m.update( i.nKey, []( map_pair&, map_pair* ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i.nKey, []( map_pair& v, map_pair* old ) {
                        EXPECT_TRUE( old == nullptr );
                        v.second.nVal = v.first.nKey;
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i.nKey, []( map_pair& v, map_pair* old ) {
                        ASSERT_FALSE( old == nullptr );
                        EXPECT_EQ( v.first.nKey, old->second.nVal );
                        v.second.nVal = old->second.nVal;
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 10:
                    updResult = m.update( i, []( map_pair&, map_pair* ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i, []( map_pair& v, map_pair* old ) {
                        EXPECT_TRUE( old == nullptr );
                        v.second.nVal = v.first.nKey;
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i, []( map_pair& v, map_pair* old ) {
                        ASSERT_FALSE( old == nullptr );
                        EXPECT_EQ( v.first.nKey, old->second.nVal );
                        v.second.nVal = old->second.nVal;
                        v.second.strVal = std::to_string( v.second.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 11:
                    updResult = m.update( val.strVal, []( map_pair&, map_pair* ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( val.strVal, []( map_pair& v, map_pair* old ) {
                        EXPECT_TRUE( old == nullptr );
                        v.second.nVal = v.first.nKey;
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( val.strVal, []( map_pair& v, map_pair* old ) {
                        ASSERT_FALSE( old == nullptr );
                        EXPECT_EQ( v.first.nKey, old->second.nVal );
                        v.second.nVal = old->second.nVal;
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
                ASSERT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
            }
            ASSERT_FALSE( m.empty());
            ASSERT_CONTAINER_SIZE( m, kkSize );
            ASSERT_FALSE( m.begin() == m.end());
            ASSERT_FALSE( m.cbegin() == m.cend());

            shuffle( arrKeys.begin(), arrKeys.end());

            {
                std::vector< typename Map::level_statistics > vect;
                m.get_level_statistics( vect );
            }

            // erase/find
            for ( auto const& i : arrKeys ) {
                value_type const& val( arrVals.at( i.nKey ));

                ASSERT_TRUE( m.contains( i.nKey ));
                ASSERT_TRUE( m.contains( val.strVal ));
                ASSERT_TRUE( m.contains( i ));
                ASSERT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));

                switch ( i.nKey % 6 ) {
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
                    ASSERT_TRUE( m.erase( i, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    } ));
                    ASSERT_FALSE( m.erase( i, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 4:
                    ASSERT_TRUE( m.erase( i.nKey, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    } ));
                    ASSERT_FALSE( m.erase( i.nKey, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 5:
                    ASSERT_TRUE( m.erase( val.strVal, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    } ));
                    ASSERT_FALSE( m.erase( val.strVal, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                }

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.contains( val.strVal ));
                ASSERT_FALSE( m.find( i, []( map_pair const& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
            }
            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            ASSERT_TRUE( m.begin() == m.end());
            ASSERT_TRUE( m.cbegin() == m.cend());

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

#endif // CDSUNIT_MAP_TEST_HOPSCOTCHHASHMAP_H_
