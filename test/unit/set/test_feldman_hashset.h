/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSUNIT_SET_TEST_FELDMAN_HASHSET_H
#define CDSUNIT_SET_TEST_FELDMAN_HASHSET_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

#include <cds/opt/hash.h>

// forward declaration
namespace cds { namespace container {}}
namespace co = cds::opt;
namespace cc = cds::container;

namespace cds_test {

    class feldman_hashset : public fixture
    {
    public:
        static size_t const kSize = 1000;

        struct stat
        {
            unsigned int nFindCount;
            unsigned int nUpdateNewCount;
            unsigned int nUpdateCount;

            stat()
            {
                clear_stat();
            }

            void clear_stat()
            {
                memset( this, 0, sizeof( *this ) );
            }
        };

        struct other_item {
            int nKey;

            explicit other_item( int k )
                : nKey( k )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct int_item: public stat
        {
            int nKey;
            int nVal;
            std::string strVal;

            int_item()
                : nKey( 0 )
                , nVal( 0 )
            {}

            explicit int_item( int k )
                : nKey( k )
                , nVal( k * 2 )
            {}

            template <typename Q>
            explicit int_item( Q const& src )
                : nKey( src.key() )
                , nVal( 0 )
            {}

            int_item( int_item const& src )
                : nKey( src.nKey )
                , nVal( src.nVal )
                , strVal( src.strVal )
            {}

            int_item( int_item&& src )
                : nKey( src.nKey )
                , nVal( src.nVal )
                , strVal( std::move( src.strVal ) )
            {}

            int_item( int k, std::string&& s )
                : nKey( k )
                , nVal( k * 2 )
                , strVal( std::move( s ) )
            {}

            explicit int_item( other_item const& s )
                : nKey( s.key() )
                , nVal( s.key() * 2 )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct get_hash {
            int operator()( int_item const& i ) const
            {
                return i.key();
            }

            int operator()( other_item const& i ) const
            {
                return i.key();
            }

            int operator()( int i ) const
            {
                return i;
            }
        };

        struct simple_item_counter {
            size_t  m_nCount;

            simple_item_counter()
                : m_nCount( 0 )
            {}

            size_t operator ++()
            {
                return ++m_nCount;
            }

            size_t operator --()
            {
                return --m_nCount;
            }

            void reset()
            {
                m_nCount = 0;
            }

            operator size_t() const
            {
                return m_nCount;
            }

        };

        struct cmp {
            int operator ()( int v1, int v2 ) const
            {
                if ( v1 < v2 )
                    return -1;
                return v1 > v2 ? 1 : 0;
            }
        };

        struct other_less {
            template <typename Q, typename T>
            bool operator()( Q const& lhs, T const& rhs ) const
            {
                return lhs.key() < rhs.key();
            }
        };

    protected:
        template <typename Set>
        void test( Set& s )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );
            size_t const nSetSize = kSize;

            typedef typename Set::value_type value_type;

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key) ) );
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end() );

            // insert/find
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_FALSE( s.contains( i.nKey ) );
                ASSERT_FALSE( s.find( i.nKey, []( value_type& ) {} ));

                std::pair<bool, bool> updResult;

                std::string str;
                updResult = s.update( i.key(), []( value_type&, value_type * )
                {
                    ASSERT_TRUE( false );
                }, false );
                EXPECT_FALSE( updResult.first );
                EXPECT_FALSE( updResult.second );

                switch ( idx % 8 ) {
                case 0:
                    ASSERT_TRUE( s.insert( i ));
                    ASSERT_FALSE( s.insert( i ));
                    updResult = s.update( i, []( value_type& val, value_type * prev ) 
                        {
                            ASSERT_TRUE( prev != nullptr );
                            EXPECT_EQ( val.key(), prev->key() );
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 1:
                    ASSERT_TRUE( s.insert( i.key() ));
                    ASSERT_FALSE( s.insert( i.key() ));
                    updResult = s.update( i.key(), []( value_type& val, value_type * prev )
                        {
                            ASSERT_TRUE( prev != nullptr );
                            EXPECT_EQ( val.key(), prev->key() );
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 2:
                    ASSERT_TRUE( s.insert( i, []( value_type& v ) { ++v.nFindCount; } ));
                    ASSERT_FALSE( s.insert( i, []( value_type& v ) { ++v.nFindCount; } ));
                    ASSERT_TRUE( s.find( i.nKey, []( value_type const& v ) 
                        {
                            EXPECT_EQ( v.nFindCount, 1 );
                        }));
                    break;
                case 3:
                    ASSERT_TRUE( s.insert( i.key(), []( value_type& v ) { ++v.nFindCount; } ));
                    ASSERT_FALSE( s.insert( i.key(), []( value_type& v ) { ++v.nFindCount; } ));
                    ASSERT_TRUE( s.find( i.nKey, []( value_type const& v ) 
                        {
                            EXPECT_EQ( v.nFindCount, 1 );
                        }));
                    break;
                case 4:
                    updResult = s.update( i, []( value_type& v, value_type * prev )
                        {
                            EXPECT_TRUE( prev == nullptr );
                            ++v.nUpdateNewCount;
                        });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = s.update( i, []( value_type& v, value_type * prev )
                        {
                            ASSERT_TRUE( prev != nullptr );
                            EXPECT_EQ( prev->nUpdateNewCount, 1 );
                            EXPECT_EQ( v.key(), prev->key() );
                            ++v.nUpdateCount;
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    ASSERT_TRUE( s.find( i.nKey, []( value_type const& v )
                        {
                            EXPECT_EQ( v.nUpdateCount, 1 );
                            EXPECT_EQ( v.nUpdateNewCount, 0 );
                        }));
                    break;
                case 5:
                    updResult = s.update( i.key(), []( value_type& v, value_type * prev )
                        {
                            EXPECT_TRUE( prev == nullptr );
                            ++v.nUpdateNewCount;
                        });
                        EXPECT_TRUE( updResult.first );
                        EXPECT_TRUE( updResult.second );

                        updResult = s.update( i.key(), []( value_type& v, value_type * prev )
                        {
                            ASSERT_TRUE( prev != nullptr );
                            EXPECT_EQ( v.key(), prev->key() );
                            EXPECT_EQ( prev->nUpdateNewCount, 1 );
                            EXPECT_EQ( v.nUpdateNewCount, 0 );
                            ++v.nUpdateNewCount;
                        }, false );
                        EXPECT_TRUE( updResult.first );
                        EXPECT_FALSE( updResult.second );

                        ASSERT_TRUE( s.find( i.key(), []( value_type const& v )
                        {
                            EXPECT_EQ( v.nUpdateNewCount, 1 );
                        } ) );
                        break;
                case 6:
                    ASSERT_TRUE( s.emplace( i.key() ) );
                    ASSERT_TRUE( s.contains( i.key() ) );
                    break;
                case 7:
                    str = "Hello!";
                    ASSERT_TRUE( s.emplace( i.key(), std::move( str ) ) );
                    EXPECT_TRUE( str.empty() );
                    ASSERT_TRUE( s.find( i.key(), []( value_type const& v )
                    {
                        EXPECT_EQ( v.strVal, std::string( "Hello!" ) );
                    } ) );
                    break;
                default:
                    // forgot anything?..
                    ASSERT_TRUE( false );
                }

                ASSERT_TRUE( s.contains( i.nKey ) );
                ASSERT_TRUE( s.find( i.nKey, []( value_type& ) {} ) );
            }

            ASSERT_FALSE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            // erase
            shuffle( indices.begin(), indices.end() );
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( s.contains( i.nKey ) );
                ASSERT_TRUE( s.find( i.nKey, []( value_type& v )
                {
                    v.nFindCount = 1;
                } ) );

                int nKey = i.key() - 1;
                switch ( idx % 2 ) {
                case 0:
                    ASSERT_TRUE( s.erase( i.key() ) );
                    ASSERT_FALSE( s.erase( i.key() ) );
                    break;
                case 1:
                    ASSERT_TRUE( s.erase( i.key(), [&nKey]( value_type const& v )
                    {
                        EXPECT_EQ( v.nFindCount, 1 );
                        nKey = v.key();
                    } ) );
                    EXPECT_EQ( i.key(), nKey );

                    nKey = i.key() - 1;
                    ASSERT_FALSE( s.erase( i.key(), [&nKey]( value_type const& v )
                    {
                        nKey = v.key();
                    } ) );
                    EXPECT_EQ( i.key(), nKey + 1 );
                    break;
                }

                ASSERT_FALSE( s.contains( i.nKey ) );
                ASSERT_FALSE( s.find( i.nKey, []( value_type const& ) {} ) );
            }
            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );


            // clear
            for ( auto& i : data ) {
                ASSERT_TRUE( s.insert( i ) );
            }

            {
                typename Set::stat const& statistics = s.statistics();
                CDS_UNUSED( statistics );

                std::vector< typename Set::level_statistics > lstat;
                s.get_level_statistics( lstat );
                EXPECT_EQ( lstat[0].node_capacity, s.head_size() );
                for ( size_t i = 1; i < lstat.size(); ++i ) {
                    EXPECT_EQ( lstat[i].node_capacity, s.array_node_size());
                }
            }

            ASSERT_FALSE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            s.clear();

            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );

            ASSERT_TRUE( s.begin() == s.end() );
            ASSERT_TRUE( s.cbegin() == s.cend() );
            ASSERT_TRUE( s.rbegin() == s.rend() );
            ASSERT_TRUE( s.crbegin() == s.crend() );
        }
    };

} // namespace cds_test

#endif // CDSUNIT_SET_TEST_FELDMAN_HASHSET_H
