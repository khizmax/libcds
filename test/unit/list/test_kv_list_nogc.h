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

#ifndef CDSUNIT_LIST_TEST_KV_LIST_NOGC_H
#define CDSUNIT_LIST_TEST_KV_LIST_NOGC_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

namespace cds_test {

    class kv_list_nogc : public fixture
    {
    public:
        struct key_type {
            int key;

            key_type() = delete;
            explicit key_type( int n )
                : key( n )
            {}

            key_type( key_type const& s )
                : key( s.key )
            {}

            key_type( key_type&& s )
                : key( s.key )
            {
                s.key = 0;
            }
        };

        struct value_type {
            int val;

            value_type()
                : val( 0 )
            {}

            explicit value_type( int n )
                : val( n )
            {}
        };

        struct lt
        {
            bool operator()( key_type const& lhs, key_type const& rhs ) const
            {
                return lhs.key < rhs.key;
            }

            bool operator()( key_type const& lhs, int rhs ) const
            {
                return lhs.key < rhs;
            }

            bool operator()( int lhs, key_type const& rhs ) const
            {
                return lhs < rhs.key;
            }

            template <typename T>
            bool operator ()( T const& v1, T const& v2 ) const
            {
                return v1.key < v2.key;
            }
        };

        struct cmp
        {
            int operator()( key_type const& lhs, key_type const& rhs ) const
            {
                return lhs.key - rhs.key;
            }

            int operator()( key_type const& lhs, int rhs ) const
            {
                return lhs.key - rhs;
            }

            int operator()( int lhs, key_type const& rhs ) const
            {
                return lhs - rhs.key;
            }

            template <typename T>
            int operator ()( T const& lhs, T const& rhs ) const
            {
                return lhs.key - rhs.key;
            }
        };

        struct other_key
        {
            int key;

            other_key()
            {}

            other_key( int n )
                : key( n )
            {}
        };

        struct other_less
        {
            template <typename T1, typename T2>
            bool operator()( T1 const& t1, T2 const& t2 ) const
            {
                return t1.key < t2.key;
            }
        };

    protected:
        template <typename List>
        void test( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::key_type    list_key_type;
            typedef typename List::mapped_type list_mapped_type;
            typedef typename List::value_type  list_value_type;

            struct key_val {
                int key;
                int val;
            };
            key_val arr[nSize];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].key = static_cast<int>(i) + 1;
                arr[i].val = arr[i].key * 10;
            }
            shuffle( arr, arr + nSize );

            ASSERT_TRUE( l.empty() );
            ASSERT_CONTAINER_SIZE( l, 0 );

            // insert/find
            for ( key_val const& i : arr ) {
                EXPECT_TRUE( l.contains( i.key ) == l.end());
                EXPECT_TRUE( l.contains( key_type( i.key )) == l.end());
                EXPECT_TRUE( l.contains( other_key( i.key ), other_less()) == l.end());

                switch ( i.key % 5 ) {
                    case 0:
                    {
                        auto it = l.insert( i.key );
                        ASSERT_FALSE( it == l.end());
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, 0 );
                        it = l.contains( i.key );
                        EXPECT_FALSE( it == l.end() );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, 0 );
                        it = l.insert( i.key );
                        EXPECT_TRUE( it == l.end() );
                        break;
                    }
                    case 1:
                    {
                        auto it = l.insert( i.key, i.val );
                        ASSERT_FALSE( it == l.end() );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.val );
                        it = l.contains( key_type( i.key ));
                        EXPECT_FALSE( it == l.end() );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.val );
                        it = l.insert( key_type( i.key ), i.val );
                        EXPECT_TRUE( it == l.end() );
                        break;
                    }
                    case 2:
                    {
                        auto it = l.emplace( i.key, i.key * 100 );
                        ASSERT_FALSE( it == l.end() );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.key * 100 );
                        it = l.contains( other_key( i.key ), other_less());
                        ASSERT_FALSE( it == l.end() );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.key * 100 );
                        it = l.emplace( i.key, i.key * 50 );
                        EXPECT_TRUE( it == l.end() );
                        break;
                    }
                    case 3:
                    {
                        auto pair = l.update( i.key, false );
                        EXPECT_TRUE( pair.first == l.end());
                        EXPECT_FALSE( pair.second );

                        pair = l.update( i.key );
                        ASSERT_FALSE( pair.first == l.end() );
                        EXPECT_TRUE( pair.second );
                        pair.first->second.val = i.key * 3;

                        auto it = l.contains( other_key( i.key ), other_less() );
                        ASSERT_FALSE( it == l.end() );
                        EXPECT_TRUE( it == pair.first );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.key * 3 );

                        pair = l.update( i.key, false );
                        ASSERT_FALSE( pair.first == l.end() );
                        EXPECT_TRUE( pair.first == it );
                        EXPECT_FALSE( pair.second );
                        EXPECT_EQ( pair.first->first.key, i.key );
                        pair.first->second.val = i.key * 5;

                        it = l.contains( other_key( i.key ), other_less() );
                        ASSERT_FALSE( it == l.end() );
                        EXPECT_TRUE( it == pair.first );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.key * 5 );
                        break;
                    }
                    case 4:
                    {
                        auto it = l.insert_with( key_type( i.key ), [&i]( list_value_type& n ) {
                            EXPECT_EQ( i.key, n.first.key );
                            n.second.val = n.first.key * 7;
                        });
                        ASSERT_FALSE( it == l.end() );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.key * 7 );
                        it = l.contains( i.key );
                        ASSERT_FALSE( it == l.end() );
                        EXPECT_EQ( it->first.key, i.key );
                        EXPECT_EQ( it->second.val, i.key * 7 );
                        it = l.insert_with( i.key, []( list_value_type& ) {
                            EXPECT_TRUE( false );
                        });
                        EXPECT_TRUE( it == l.end() );
                        break;
                    }
                }

                EXPECT_TRUE( l.contains( i.key ) != l.end());
                EXPECT_TRUE( l.contains( key_type( i.key )) != l.end());
                EXPECT_TRUE( l.contains( other_key( i.key ), other_less()) != l.end());

                EXPECT_FALSE( l.empty() );
            }

            ASSERT_FALSE( l.empty() );
            EXPECT_CONTAINER_SIZE( l, nSize );

            l.clear();

            ASSERT_TRUE( l.empty() );
            EXPECT_CONTAINER_SIZE( l, 0 );

            // empty list iterator test
            {
                List const& cl = l;
                EXPECT_TRUE( l.begin() == l.end());
                EXPECT_TRUE( l.cbegin() == l.cend());
                EXPECT_TRUE( cl.begin() == cl.end());
                EXPECT_TRUE( l.begin() == l.cend());
                EXPECT_TRUE( cl.begin() == l.end());
            }
        }

        template <typename List>
        void test_ordered_iterator( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::key_type    list_key_type;
            typedef typename List::mapped_type list_mapped_type;
            typedef typename List::value_type  list_value_type;

            struct key_val {
                int key;
                int val;
            };
            key_val arr[nSize];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].key = static_cast<int>(i);
                arr[i].val = arr[i].key;
            }
            shuffle( arr, arr + nSize );

            ASSERT_TRUE( l.empty() );
            ASSERT_CONTAINER_SIZE( l, 0 );

            for ( auto& i : arr )
                EXPECT_TRUE( l.insert( i.key, i.val ) != l.end());

            int key = 0;
            for ( auto& it : l ) {
                EXPECT_EQ( key, it.first.key );
                EXPECT_EQ( it.second.val, it.first.key );
                it.second.val = it.first.key * 10;
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            key = 0;
            for ( auto it = l.cbegin(); it != l.cend(); ++it ) {
                EXPECT_EQ( key, it->first.key );
                EXPECT_EQ( key, (*it).first.key );
                EXPECT_EQ( it->first.key * 10, it->second.val );
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            key = 0;
            for ( auto it = l.begin(); it != l.end(); ++it ) {
                EXPECT_EQ( key, it->first.key );
                EXPECT_EQ( it->first.key * 10, it->second.val );
                it->second.val = it->first.key * 2;
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            List const& cl = l;
            key = 0;
            for ( auto it = cl.begin(); it != cl.end(); ++it ) {
                EXPECT_EQ( key, it->first.key );
                EXPECT_EQ( it->first.key * 2, it->second.val );
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            l.clear();
            ASSERT_TRUE( l.empty() );
            EXPECT_CONTAINER_SIZE( l, 0 );
        }
    };

} // namespace cds_test

#endif // CDSUNIT_LIST_TEST_KV_LIST_NOGC_H
