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

#ifndef CDSUNIT_LIST_TEST_LIST_H
#define CDSUNIT_LIST_TEST_LIST_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

namespace cds_test {

    class list_common : public fixture
    {
    public:
        struct item {
            int     nKey;
            int     nVal;

            item()
            {}

            item( int key )
                : nKey( key )
                , nVal( key * 2 )
            {}

            item( int key, int val )
                : nKey( key )
                , nVal( val )
            {}

            item( const item& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
        };

        template <typename T>
        struct lt
        {
            bool operator ()( const T& v1, const T& v2 ) const
            {
                return v1.key() < v2.key();
            }

            template <typename Q>
            bool operator ()( const T& v1, const Q& v2 ) const
            {
                return v1.key() < v2;
            }

            template <typename Q>
            bool operator ()( const Q& v1, const T& v2 ) const
            {
                return v1 < v2.key();
            }
        };

        template <typename T>
        struct cmp {
            int operator ()( const T& v1, const T& v2 ) const
            {
                if ( v1.key() < v2.key())
                    return -1;
                return v1.key() > v2.key() ? 1 : 0;
            }

            template <typename Q>
            int operator ()( const T& v1, const Q& v2 ) const
            {
                if ( v1.key() < v2 )
                    return -1;
                return v1.key() > v2 ? 1 : 0;
            }

            template <typename Q>
            int operator ()( const Q& v1, const T& v2 ) const
            {
                if ( v1 < v2.key())
                    return -1;
                return v1 > v2.key() ? 1 : 0;
            }
        };

        struct other_item
        {
            int nKey;

            other_item()
            {}

            other_item( int n )
                : nKey( n )
            {}
        };

        struct other_less
        {
            template <typename T1, typename T2>
            bool operator()( T1 const& t1, T2 const& t2 ) const
            {
                return t1.nKey < t2.nKey;
            }
        };

    protected:
        template <typename List>
        void test_common( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::value_type value_type;
            value_type arr[nSize];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].nKey = static_cast<int>(i);
                arr[i].nVal = arr[i].nKey * 10;
            }
            shuffle( arr, arr + nSize );

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            // insert/find
            for ( auto const& i : arr ) {
                EXPECT_FALSE( l.contains( i ));
                EXPECT_FALSE( l.contains( i.nKey ));
                EXPECT_FALSE( l.contains( other_item( i.nKey ), other_less()));
                EXPECT_FALSE( l.find( i, []( value_type&, value_type const&) {} ));
                EXPECT_FALSE( l.find( i.nKey, []( value_type&, int ) {} ));
                EXPECT_FALSE( l.find_with( other_item( i.nKey ), other_less(), []( value_type&, other_item const& ) {} ));

                switch ( i.nKey & 3 ) {
                    case 0:
                        EXPECT_TRUE( l.insert( i.nKey ));
                        EXPECT_TRUE( l.find( i.nKey, []( value_type& n, int key ) {
                            EXPECT_EQ( n.nVal, key * 2 );
                        } ));
                        EXPECT_FALSE( l.insert( i.nKey ));
                        break;
                    case 1:
                        EXPECT_TRUE( l.insert( i, []( value_type& n ) {
                            n.nVal = n.nKey * 3;
                        }));
                        EXPECT_TRUE( l.find( i.nKey, []( value_type& n, int key ) {
                            EXPECT_EQ( n.nVal, key * 3 );
                        } ));
                        EXPECT_FALSE( l.insert( i ));
                        break;
                    case 2:
                        EXPECT_TRUE( l.emplace( i.nKey, i.nKey * 100 ));
                        EXPECT_TRUE( l.find( i.nKey, []( value_type& n, int key ) {
                            EXPECT_EQ( n.nVal, key * 100 );
                        } ));
                        EXPECT_FALSE( l.insert( i ));
                        break;
                    case 3:
                    {
                        auto pair = l.update( i.nKey, []( bool bNew, value_type& n, int key ) {
                            EXPECT_TRUE( bNew );
                            EXPECT_EQ( key, n.nKey );
                            n.nVal = n.nKey * 3;
                        }, false );
                        EXPECT_FALSE( pair.first );
                        EXPECT_FALSE( pair.second );

                        pair = l.update( i.nKey, []( bool bNew, value_type& n, int key ) {
                            EXPECT_TRUE( bNew );
                            EXPECT_EQ( key, n.nKey );
                            n.nVal = n.nKey * 3;
                        });
                        EXPECT_TRUE( pair.first );
                        EXPECT_TRUE( pair.second );

                        EXPECT_TRUE( l.find( i.nKey, []( value_type& n, int key ) {
                            EXPECT_EQ( n.nVal, key * 3 );
                        } ));
                        EXPECT_FALSE( l.insert( i ));
                    }
                        break;
                }

                EXPECT_TRUE( l.contains( i ));
                EXPECT_TRUE( l.contains( i.nKey ));
                EXPECT_TRUE( l.contains( other_item( i.nKey ), other_less()));
                EXPECT_TRUE( l.find( i, []( value_type& n, value_type const& arg ) {
                    EXPECT_EQ( arg.nKey, n.nKey );
                    n.nVal = n.nKey;
                } ));
                EXPECT_TRUE( l.find( i.nKey, []( value_type& n, int key ) {
                    EXPECT_EQ( key, n.nKey );
                    EXPECT_EQ( n.nKey, n.nVal );
                    n.nVal = n.nKey * 5;
                } ));
                EXPECT_TRUE( l.find_with( other_item( i.nKey ), other_less(), []( value_type& n, other_item const& key ) {
                    EXPECT_EQ( key.nKey, n.nKey );
                    EXPECT_EQ( n.nKey * 5, n.nVal );
                } ));

                auto pair = l.update( i.nKey, []( bool bNew, value_type& n, int key ) {
                    EXPECT_FALSE( bNew );
                    EXPECT_EQ( key, n.nKey );
                    EXPECT_EQ( key * 5, n.nVal );
                    n.nVal = n.nKey * 3;
                }, false );
                EXPECT_TRUE( pair.first );
                EXPECT_FALSE( pair.second );

                EXPECT_FALSE( l.empty());
            }

            ASSERT_FALSE( l.empty());
            EXPECT_CONTAINER_SIZE( l, nSize );

            // erase
            for ( auto const&i : arr ) {
                switch ( i.nKey & 3 ) {
                    case 0:
                        EXPECT_TRUE( l.erase( i.nKey ));
                        break;
                    case 1:
                        EXPECT_TRUE( l.erase_with( other_item( i.nKey ), other_less()));
                        break;
                    case 2:
                        EXPECT_TRUE( l.erase( i, [ &i ]( value_type const& n ) {
                            EXPECT_EQ( n.nKey, i.nKey );
                            EXPECT_EQ( n.nKey * 3, n.nVal );
                        }));
                        break;
                    case 3:
                        EXPECT_TRUE( l.erase_with( other_item( i.nKey ), other_less(), [ &i ]( value_type const& n) {
                            EXPECT_EQ( n.nKey, i.nKey );
                            EXPECT_EQ( n.nKey * 3, n.nVal );
                        } ));
                }

                EXPECT_FALSE( l.contains( i ));
                EXPECT_FALSE( l.contains( i.nKey ));
                EXPECT_FALSE( l.contains( other_item( i.nKey ), other_less()));
                EXPECT_FALSE( l.find( i, []( value_type&, value_type const&) {} ));
                EXPECT_FALSE( l.find( i.nKey, []( value_type&, int ) {} ));
                EXPECT_FALSE( l.find_with( other_item( i.nKey ), other_less(), []( value_type&, other_item const& ) {} ));
            }

            ASSERT_TRUE( l.empty());
            EXPECT_CONTAINER_SIZE( l, 0 );

            // clear test
            for ( auto& i : arr )
                EXPECT_TRUE( l.insert( i ));

            ASSERT_FALSE( l.empty());
            EXPECT_CONTAINER_SIZE( l, nSize );

            l.clear();

            ASSERT_TRUE( l.empty());
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
            typedef typename List::value_type value_type;
            value_type arr[nSize];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].nKey = static_cast<int>(i);
                arr[i].nVal = arr[i].nKey;
            }
            shuffle( arr, arr + nSize );

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            for ( auto& i : arr )
                EXPECT_TRUE( l.insert( i ));

            int key = 0;
            for ( auto& it : l ) {
                EXPECT_EQ( key, it.nKey );
                EXPECT_EQ( it.nVal, it.nKey );
                it.nVal = it.nKey * 10;
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            key = 0;
            for ( auto it = l.cbegin(); it != l.cend(); ++it ) {
                EXPECT_EQ( key, it->nKey );
                EXPECT_EQ( key, (*it).nKey );
                EXPECT_EQ( it->nKey * 10, it->nVal );
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            key = 0;
            for ( auto it = l.begin(); it != l.end(); ++it ) {
                EXPECT_EQ( key, it->nKey );
                EXPECT_EQ( key, (*it).nKey );
                EXPECT_EQ( it->nKey * 10, it->nVal );
                it->nVal = it->nKey * 2;
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            List const& cl = l;
            key = 0;
            for ( auto it = cl.begin(); it != cl.end(); ++it ) {
                EXPECT_EQ( key, it->nKey );
                EXPECT_EQ( key, (*it).nKey );
                EXPECT_EQ( it->nKey * 2, it->nVal );
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            l.clear();
            ASSERT_TRUE( l.empty());
            EXPECT_CONTAINER_SIZE( l, 0 );
        }
    };

} // namespace cds_test

#endif // CDSUNIT_LIST_TEST_LIST_H
