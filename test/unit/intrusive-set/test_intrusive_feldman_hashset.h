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

#ifndef CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_H
#define CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

#include <cds/opt/hash.h>
#include <functional>   // ref

// forward declaration
namespace cds { namespace intrusive {}}
namespace ci = cds::intrusive;

namespace cds_test {
    class intrusive_feldman_hashset: public fixture
    {
    public:
        struct stat
        {
            unsigned int nDisposeCount;   // count of disposer calling
            unsigned int nFindCount;   // count of find-functor calling
            unsigned int nInsertCount;
            mutable unsigned int nEraseCount;

            stat()
            {
                clear_stat();
            }

            void clear_stat()
            {
                memset( this, 0, sizeof( *this ));
            }
        };

        struct int_item: public stat
        {
            int nKey;
            int nVal;

            int_item()
            {}

            explicit int_item( int key )
                : nKey( key )
                , nVal( key )
            {}

            int_item( int key, int val )
                : nKey( key )
                , nVal( val )
            {}

            int_item( int_item const& v )
                : stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct key_val {
            int nKey;
            int nVal;

            key_val()
            {}

            key_val( int key )
                : nKey( key )
                , nVal( key )
            {}

            key_val( int key, int val )
                : nKey( key )
                , nVal( val )
            {}

            key_val( key_val const& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct int_item2: public key_val, public stat
        {
            int_item2()
            {}

            explicit int_item2( int key )
                : key_val( key )
            {}

            int_item2( int key, int val )
                : key_val( key, val )
            {}

            int_item2( int_item2 const& v )
                : key_val( v )
                , stat()
            {}
        };

        struct hash_accessor {
            int operator()( int_item const& v ) const
            {
                return v.key();
            }
        };

        struct hash_accessor2 {
            key_val const& operator()( int_item2 const& v ) const
            {
                return v;
            }
        };

        struct simple_item_counter {
            size_t  m_nCount;

            simple_item_counter()
                : m_nCount(0)
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
            int operator ()( int lhs, int rhs ) const
            {
                if ( lhs < rhs )
                    return -1;
                return lhs > rhs ? 1 : 0;
            }
        };

        struct cmp2 {
            int operator ()( key_val const& lhs, key_val const& rhs ) const
            {
                if ( lhs.key() < rhs.key())
                    return -1;
                return lhs.key() > rhs.key() ? 1 : 0;
            }
        };

        struct mock_disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount;
            }
        };

    protected:
        template <class Set>
        void test( Set& s )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );
            size_t const nSetSize = std::max( s.head_size() * 2, static_cast<size_t>(100));

            typedef typename Set::value_type value_type;

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( nSetSize );
            indices.reserve( nSetSize );
            for ( size_t key = 0; key < nSetSize; ++key ) {
                data.push_back( value_type( static_cast<int>( key )));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end());

            // insert/find
            for ( auto idx : indices ) {
                auto& i = data[ idx ];

                ASSERT_FALSE( s.contains( i.nKey ));
                ASSERT_FALSE( s.find( i.nKey, []( value_type& ) {} ));

                std::pair<bool, bool> updResult;

                updResult = s.update( i, false );
                EXPECT_FALSE( updResult.first );
                EXPECT_FALSE( updResult.second );

                switch ( i.key() % 3 ) {
                case 0:
                    ASSERT_TRUE( s.insert( i ));
                    ASSERT_FALSE( s.insert( i ));
                    updResult = s.update( i, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 1:
                    EXPECT_EQ( i.nInsertCount, 0u );
                    ASSERT_TRUE( s.insert( i, []( value_type& v ) { ++v.nInsertCount;} ));
                    EXPECT_EQ( i.nInsertCount, 1u );
                    ASSERT_FALSE( s.insert( i, []( value_type& v ) { ++v.nInsertCount;} ));
                    EXPECT_EQ( i.nInsertCount, 1u );
                    i.nInsertCount = 0;
                    break;
                case 2:
                    updResult = s.update( i );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );
                    break;
                }

                ASSERT_TRUE( s.contains( i.nKey ));
                EXPECT_EQ( i.nFindCount, 0u );
                ASSERT_TRUE( s.find( i.nKey, []( value_type& v ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 1u );
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            std::for_each( data.begin(), data.end(), []( value_type& v ) { v.clear_stat(); });

            // get_level_statistics
            {
                std::vector< typename Set::level_statistics > level_stat;
                s.get_level_statistics( level_stat );
                EXPECT_GT( level_stat.size(), 0u );
            }

            // erase
            shuffle( indices.begin(), indices.end());
            for ( auto idx : indices ) {
                auto& i = data[ idx ];

                ASSERT_TRUE( s.contains( i.nKey ));
                EXPECT_EQ( i.nFindCount, 0u );
                ASSERT_TRUE( s.find( i.nKey, []( value_type& v ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 1u );

                value_type v( i );
                switch ( i.key() % 3 ) {
                case 0:
                    ASSERT_FALSE( s.unlink( v ));
                    ASSERT_TRUE( s.unlink( i ));
                    ASSERT_FALSE( s.unlink( i ));
                    break;
                case 1:
                    ASSERT_TRUE( s.erase( i.key()));
                    ASSERT_FALSE( s.erase( i.key()));
                    break;
                case 2:
                    EXPECT_EQ( i.nEraseCount, 0u );
                    ASSERT_TRUE( s.erase( v.key(), []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1u );
                    ASSERT_FALSE( s.erase( v.key(), []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1u );
                    break;
                }

                ASSERT_FALSE( s.contains( i.nKey ));
                ASSERT_FALSE( s.find( i.nKey, []( value_type const& ) {} ));
            }
            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            // Force retiring cycle
            Set::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }

            // clear
            for ( auto& i : data ) {
                i.clear_stat();
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            // Forward iterator test
            for ( auto it = s.begin(); it != s.end(); ++it ) {
                ++it->nFindCount;
            }
            for ( auto it = s.cbegin(); it != s.cend(); ++it ) {
                EXPECT_EQ( it->nFindCount, 1u );
            }

            // Reverse iterator test
            for ( auto it = s.rbegin(); it != s.rend(); ++it ) {
                ++it->nFindCount;
            }
            for ( auto it = s.crbegin(); it != s.crend(); ++it ) {
                EXPECT_EQ( it->nFindCount, 2u );
            }

            for ( auto& i : data ) {
                EXPECT_EQ( i.nFindCount, 2u );
            }

            // clear test
            s.clear();

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0u );
            ASSERT_TRUE( s.begin() == s.end());
            ASSERT_TRUE( s.cbegin() == s.cend());

            // Force retiring cycle
            Set::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_H
