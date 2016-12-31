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

#ifndef CDSUNIT_SET_TEST_MICHAEL_ITERABLE_H
#define CDSUNIT_SET_TEST_MICHAEL_ITERABLE_H

#include "test_set_data.h"

namespace cds_test {

    class michael_iterable_set: public container_set_data
    {
    protected:
        template <typename Set>
        void test( Set& s )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            EXPECT_TRUE( s.empty());
            EXPECT_CONTAINER_SIZE( s, 0 );
            size_t const nSetSize = kSize;

            typedef typename Set::value_type value_type;

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key)));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end());

            // insert/find
            for ( auto idx : indices ) {
                auto& i = data[idx];

                EXPECT_FALSE( s.contains( i.nKey ));
                EXPECT_FALSE( s.contains( i ));
                EXPECT_FALSE( s.contains( other_item( i.key()), other_less()));
                EXPECT_FALSE( s.find( i.nKey, []( value_type&, int ) {} ));
                EXPECT_FALSE( s.find( i, []( value_type&, value_type const& ) {} ));
                EXPECT_FALSE( s.find_with( other_item( i.key()), other_less(), []( value_type&, other_item const& ) {} ));
                EXPECT_TRUE( s.find( i ) == s.end());
                EXPECT_TRUE( s.find( i.nKey ) == s.end());
                EXPECT_TRUE( s.find_with( other_item( i.key()), other_less()) == s.end());

                std::pair<bool, bool> updResult;

                std::string str;
                updResult = s.update( i.key(), []( value_type&, value_type* old )
                {
                    EXPECT_TRUE( old == nullptr );
                }, false );
                EXPECT_FALSE( updResult.first );
                EXPECT_FALSE( updResult.second );

                switch ( idx % 11 ) {
                case 0:
                    EXPECT_TRUE( s.insert( i ));
                    EXPECT_FALSE( s.insert( i ));
                    updResult = s.update( i, []( value_type& val, value_type* old)
                        {
                            ASSERT_FALSE( old == nullptr );
                            EXPECT_EQ( val.key(), old->key());
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 1:
                    EXPECT_TRUE( s.insert( i.key()));
                    EXPECT_FALSE( s.insert( i.key()));
                    updResult = s.update( i.key(), []( value_type& val, value_type* old)
                        {
                            ASSERT_FALSE( old == nullptr );
                            EXPECT_EQ( val.key(), old->key());
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 2:
                    EXPECT_TRUE( s.insert( i, []( value_type& v ) { ++v.nFindCount; } ));
                    EXPECT_FALSE( s.insert( i, []( value_type& v ) { ++v.nFindCount; } ));
                    EXPECT_TRUE( s.find( i.nKey, []( value_type const& v, int key )
                        {
                            EXPECT_EQ( v.key(), key );
                            EXPECT_EQ( v.nFindCount, 1u );
                        }));
                    break;
                case 3:
                    EXPECT_TRUE( s.insert( i.key(), []( value_type& v ) { ++v.nFindCount; } ));
                    EXPECT_FALSE( s.insert( i.key(), []( value_type& v ) { ++v.nFindCount; } ));
                    EXPECT_TRUE( s.find( i.nKey, []( value_type const& v, int key )
                        {
                            EXPECT_EQ( v.key(), key );
                            EXPECT_EQ( v.nFindCount, 1u );
                        }));
                    break;
                case 4:
                    updResult = s.update( i, [&i]( value_type& v, value_type* old )
                        {
                            EXPECT_TRUE( old == nullptr );
                            EXPECT_EQ( v.key(), i.key());
                            ++v.nUpdateNewCount;
                        });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = s.update( i, []( value_type& v, value_type* old )
                        {
                            ASSERT_FALSE( old == nullptr );
                            EXPECT_EQ( v.key(), old->key());
                            EXPECT_EQ( old->nUpdateNewCount, 1u );
                            v.nUpdateNewCount = old->nUpdateNewCount;
                            ++v.nUpdateCount;
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    EXPECT_TRUE( s.find( i.nKey, []( value_type const& v, int key )
                        {
                            EXPECT_EQ( v.key(), key );
                            EXPECT_EQ( v.nUpdateNewCount, 1u );
                            EXPECT_EQ( v.nUpdateCount, 1u );
                    }));
                    break;
                case 5:
                    updResult = s.update( i.key(), [&i]( value_type& v, value_type* old )
                        {
                            EXPECT_TRUE( old == nullptr );
                            EXPECT_EQ( v.key(), i.key());
                            ++v.nUpdateNewCount;
                        });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = s.update( i.key(), []( value_type& v, value_type* old )
                        {
                            EXPECT_FALSE( old == nullptr );
                            EXPECT_EQ( v.key(), old->key());
                            EXPECT_EQ( old->nUpdateNewCount, 1u );
                            v.nUpdateNewCount = old->nUpdateNewCount;
                            ++v.nUpdateCount;
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    EXPECT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                        {
                            EXPECT_EQ( v.key(), arg.key());
                            EXPECT_EQ( v.nUpdateNewCount, 1u );
                            EXPECT_EQ( v.nUpdateCount, 1u );
                    }));
                    break;
                case 6:
                    EXPECT_TRUE( s.emplace( i.key()));
                    EXPECT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                        {
                            EXPECT_EQ( v.key(), arg.key());
                            EXPECT_EQ( v.nVal, arg.nVal );
                        }));
                    break;
                case 7:
                    str = "Hello!";
                    EXPECT_TRUE( s.emplace( i.key(), std::move( str )));
                    EXPECT_TRUE( str.empty());
                    EXPECT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                        {
                            EXPECT_EQ( v.key(), arg.key());
                            EXPECT_EQ( v.nVal, arg.nVal );
                            EXPECT_EQ( v.strVal, std::string( "Hello!" ));
                        } ));
                    break;
                case 8:
                    str = "Hello!";
                    EXPECT_TRUE( s.insert( value_type( i.key(), std::move( str ))));
                    EXPECT_TRUE( str.empty());
                    EXPECT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                    {
                        EXPECT_EQ( v.key(), arg.key());
                        EXPECT_EQ( v.nVal, arg.nVal );
                        EXPECT_EQ( v.strVal, std::string( "Hello!" ));
                    } ));
                    break;
                case 9:
                    updResult = s.upsert( i.key(), false );
                    EXPECT_FALSE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    updResult = s.upsert( i.key());
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = s.upsert( i.key(), false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    EXPECT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                    {
                        EXPECT_EQ( v.key(), arg.key());
                    } ));
                    break;
                case 10:
                    updResult = s.upsert( i, false );
                    EXPECT_FALSE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    updResult = s.upsert( i );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = s.upsert( i, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    EXPECT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                    {
                        EXPECT_EQ( v.key(), arg.key());
                    } ));
                    break;
                default:
                    // forgot anything?..
                    EXPECT_TRUE( false );
                }

                EXPECT_TRUE( s.contains( i.nKey ));
                EXPECT_TRUE( s.contains( i ));
                EXPECT_TRUE( s.contains( other_item( i.key()), other_less()));
                EXPECT_TRUE( s.find( i.nKey, []( value_type&, int ) {} ));
                EXPECT_TRUE( s.find( i, []( value_type&, value_type const& ) {} ));
                EXPECT_TRUE( s.find_with( other_item( i.key()), other_less(), []( value_type&, other_item const& ) {} ));
                EXPECT_FALSE( s.find( i.nKey ) == s.end());
                EXPECT_FALSE( s.find( i ) == s.end());
                EXPECT_FALSE( s.find_with( other_item( i.key()), other_less()) == s.end());
            }

            EXPECT_FALSE( s.empty());
            EXPECT_CONTAINER_SIZE( s, nSetSize );

            // erase
            shuffle( indices.begin(), indices.end());
            for ( auto idx : indices ) {
                auto& i = data[idx];

                EXPECT_TRUE( s.contains( i.nKey ));
                EXPECT_TRUE( s.contains( i ));
                EXPECT_TRUE( s.contains( other_item( i.key()), other_less()));
                EXPECT_TRUE( s.find( i.nKey, []( value_type& v, int )
                    {
                        v.nFindCount = 1;
                    }));
                EXPECT_TRUE( s.find( i, []( value_type& v, value_type const& )
                    {
                        EXPECT_EQ( ++v.nFindCount, 2u );
                    }));
                EXPECT_TRUE( s.find_with( other_item( i.key()), other_less(), []( value_type& v, other_item const& )
                    {
                        EXPECT_EQ( ++v.nFindCount, 3u );
                    }));

                int nKey = i.key() - 1;
                switch ( idx % 6 ) {
                case 0:
                    EXPECT_TRUE( s.erase( i.key()));
                    EXPECT_FALSE( s.erase( i.key()));
                    break;
                case 1:
                    EXPECT_TRUE( s.erase( i ));
                    EXPECT_FALSE( s.erase( i ));
                    break;
                case 2:
                    EXPECT_TRUE( s.erase_with( other_item( i.key()), other_less()));
                    EXPECT_FALSE( s.erase_with( other_item( i.key()), other_less()));
                    break;
                case 3:
                    EXPECT_TRUE( s.erase( i.key(), [&nKey]( value_type const& v )
                        {
                            nKey = v.key();
                        } ));
                    EXPECT_EQ( i.key(), nKey );

                    nKey = i.key() - 1;
                    EXPECT_FALSE( s.erase( i.key(), [&nKey]( value_type const& v )
                        {
                            nKey = v.key();
                        } ));
                    EXPECT_EQ( i.key(), nKey + 1 );
                    break;
                case 4:
                    EXPECT_TRUE( s.erase( i, [&nKey]( value_type const& v )
                        {
                            nKey = v.key();
                        } ));
                    EXPECT_EQ( i.key(), nKey );

                    nKey = i.key() - 1;
                    EXPECT_FALSE( s.erase( i, [&nKey]( value_type const& v )
                        {
                            nKey = v.key();
                        } ));
                    EXPECT_EQ( i.key(), nKey + 1 );
                    break;
                case 5:
                    EXPECT_TRUE( s.erase_with( other_item( i.key()), other_less(), [&nKey]( value_type const& v )
                        {
                            nKey = v.key();
                        } ));
                    EXPECT_EQ( i.key(), nKey );

                    nKey = i.key() - 1;
                    EXPECT_FALSE( s.erase_with( other_item( i.key()), other_less(), [&nKey]( value_type const& v )
                        {
                            nKey = v.key();
                        } ));
                    EXPECT_EQ( i.key(), nKey + 1 );
                    break;
                }

                EXPECT_FALSE( s.contains( i.nKey ));
                EXPECT_FALSE( s.contains( i ));
                EXPECT_FALSE( s.contains( other_item( i.key()), other_less()));
                EXPECT_FALSE( s.find( i.nKey, []( value_type&, int ) {} ));
                EXPECT_FALSE( s.find( i, []( value_type&, value_type const& ) {} ));
                EXPECT_FALSE( s.find_with( other_item( i.key()), other_less(), []( value_type&, other_item const& ) {} ));
            }
            EXPECT_TRUE( s.empty());
            EXPECT_CONTAINER_SIZE( s, 0u );


            // clear
            for ( auto& i : data ) {
                EXPECT_TRUE( s.insert( i ));
            }

            EXPECT_FALSE( s.empty());
            EXPECT_CONTAINER_SIZE( s, nSetSize );

            s.clear();

            EXPECT_TRUE( s.empty());
            EXPECT_CONTAINER_SIZE( s, 0u );

            EXPECT_TRUE( s.begin() == s.end());
            EXPECT_TRUE( s.cbegin() == s.cend());
        }
    };

} // namespace cds_test

#endif // CDSUNIT_SET_TEST_MICHAEL_ITERABLE_H
