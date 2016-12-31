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

#ifndef CDSUNIT_LIST_TEST_KV_ITERABLE_LIST_H
#define CDSUNIT_LIST_TEST_KV_ITERABLE_LIST_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

namespace cds_test {

    class kv_iterable_list : public fixture
    {
    public:
        struct key_type {
            int nKey;

            key_type() = delete;
            explicit key_type( int n )
                : nKey( n )
            {}

            key_type( key_type const& s )
                : nKey( s.nKey )
            {}

            key_type( key_type&& s )
                : nKey( s.nKey )
            {
                s.nKey = 0;
            }

            int key() const
            {
                return nKey;
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
                return lhs.key() < rhs.key();
            }

            bool operator()( key_type const& lhs, int rhs ) const
            {
                return lhs.key() < rhs;
            }

            bool operator()( int lhs, key_type const& rhs ) const
            {
                return lhs < rhs.key();
            }

            template <typename T>
            bool operator ()( T const& v1, T const& v2 ) const
            {
                return v1.key() < v2.key();
            }
        };

        struct cmp
        {
            int operator()( key_type const& lhs, key_type const& rhs ) const
            {
                return lhs.key() - rhs.key();
            }

            int operator()( key_type const& lhs, int rhs ) const
            {
                return lhs.key() - rhs;
            }

            int operator()( int lhs, key_type const& rhs ) const
            {
                return lhs - rhs.key();
            }

            template <typename T>
            int operator ()( T const& lhs, T const& rhs ) const
            {
                return lhs.key() - rhs.key();
            }
        };

        struct other_key
        {
            int nKey;

            other_key()
            {}

            other_key( int n )
                : nKey( n )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct other_less
        {
            template <typename T1, typename T2>
            bool operator()( T1 const& t1, T2 const& t2 ) const
            {
                return t1.key() < t2.key();
            }
        };

    protected:
        template <typename List>
        void test_common( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::key_type    list_key_type;
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

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            // insert/find
            for ( auto const& i : arr ) {
                EXPECT_FALSE( l.contains( i.key ));
                EXPECT_FALSE( l.contains( key_type( i.key )));
                EXPECT_FALSE( l.contains( other_key( i.key ), other_less()));
                EXPECT_FALSE( l.find( i.key, []( list_value_type& ) {} ));
                EXPECT_FALSE( l.find( key_type( i.key ), []( list_value_type& ) {} ));
                EXPECT_FALSE( l.find_with( other_key( i.key ), other_less(), []( list_value_type& ) {} ));
                EXPECT_TRUE( l.find( i.key ) == l.end());
                EXPECT_TRUE( l.find_with( other_key( i.key ), other_less()) == l.end());

                switch ( i.key % 6 ) {
                    case 0:
                        EXPECT_TRUE( l.insert( i.key ));
                        EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                            EXPECT_EQ( n.second.val, 0 );
                        } ));
                        EXPECT_FALSE( l.insert( i.key ));
                        break;

                    case 1:
                        EXPECT_TRUE( l.insert( i.key, i.val ));
                        EXPECT_TRUE( l.find( key_type(i.key), []( list_value_type& n ) {
                            EXPECT_EQ( n.second.val, n.first.nKey * 10 );
                        } ));
                        EXPECT_FALSE( l.insert( key_type( i.key )));
                        break;

                    case 2:
                        EXPECT_TRUE( l.insert_with( i.key, []( list_value_type& n ) {
                            n.second.val = n.first.nKey * 2;
                        }));
                        EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                            EXPECT_EQ( n.second.val, n.first.key() * 2 );
                        } ));
                        EXPECT_FALSE( l.insert_with( i.key, []( list_value_type& n ) {
                            n.second.val = n.first.nKey * 3;
                        } ));
                        EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                            EXPECT_EQ( n.second.val, n.first.key() * 2 );
                        } ));
                        break;

                    case 3:
                        {
                            key_type k( i.key );
                            EXPECT_TRUE( l.emplace( std::move(k), i.key * 100 ));
                            EXPECT_EQ( k.key(), 0 );
                            EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                                EXPECT_EQ( n.second.val, n.first.nKey * 100 );
                            } ));
                            k.nKey = i.key;
                            EXPECT_FALSE( l.emplace( std::move( k ), i.key ));
                            //EXPECT_EQ( k.key(), i.key ); // ???
                            EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                                EXPECT_EQ( n.second.val, n.first.nKey * 100 );
                            } ));
                        }
                        break;

                    case 4:
                        {
                            auto pair = l.update( i.key, []( list_value_type&, list_value_type* ) {
                                ASSERT_TRUE( false );
                            }, false );
                            EXPECT_FALSE( pair.first );
                            EXPECT_FALSE( pair.second );

                            pair = l.update( list_key_type(i.key), []( list_value_type& n, list_value_type* old ) {
                                EXPECT_TRUE( old == nullptr );
                                n.second.val = n.first.nKey * 3;
                            });
                            EXPECT_TRUE( pair.first );
                            EXPECT_TRUE( pair.second );

                            EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                                EXPECT_EQ( n.second.val, n.first.key() * 3 );
                            } ));

                            pair = l.update( list_key_type(i.key), []( list_value_type& n, list_value_type* old ) {
                                EXPECT_FALSE( old == nullptr );
                                n.second.val = n.first.nKey * 5;
                            });
                            EXPECT_TRUE( pair.first );
                            EXPECT_FALSE( pair.second );

                            EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                                EXPECT_EQ( n.second.val, n.first.key() * 5 );
                            } ));
                        }
                        break;
                    case 5:
                        {
                            auto ret = l.upsert( i.key, i.val, false );
                            EXPECT_FALSE( ret.first );
                            EXPECT_FALSE( ret.second );
                            EXPECT_FALSE( l.contains( i.key ));

                            ret = l.upsert( i.key, i.val );
                            EXPECT_TRUE( ret.first );
                            EXPECT_TRUE( ret.second );
                            EXPECT_TRUE( l.contains( i.key ));

                            ret = l.upsert( i.key, i.key * 12 );
                            EXPECT_TRUE( ret.first );
                            EXPECT_FALSE( ret.second );
                            EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                                EXPECT_EQ( n.second.val, n.first.key() * 12 );
                            }));
                        }
                        break;
                }

                EXPECT_TRUE( l.contains( i.key ));
                EXPECT_TRUE( l.contains( list_key_type(i.key)));
                EXPECT_TRUE( l.contains( other_key( i.key ), other_less()));
                EXPECT_TRUE( l.find( i.key, []( list_value_type& n  ) {
                    n.second.val = n.first.nKey;
                } ));
                EXPECT_TRUE( l.find( i.key, []( list_value_type& n ) {
                    EXPECT_EQ( n.first.nKey, n.second.val );
                    n.second.val = n.first.nKey * 5;
                } ));
                EXPECT_TRUE( l.find_with( other_key( i.key ), other_less(), []( list_value_type& n ) {
                    EXPECT_EQ( n.first.nKey * 5, n.second.val );
                } ));

                auto pair = l.update( i.key, []( list_value_type& n, list_value_type* old ) {
                    ASSERT_FALSE( old == nullptr );
                    EXPECT_EQ( n.first.nKey * 5, old->second.val );
                    n.second.val = n.first.nKey * 3;
                }, false );
                EXPECT_TRUE( pair.first );
                EXPECT_FALSE( pair.second );

                EXPECT_FALSE( l.find( i.key ) == l.end());
                EXPECT_EQ( l.find( i.key )->first.nKey, i.key );
                EXPECT_EQ( l.find( i.key )->second.val, i.key * 3 );
                EXPECT_FALSE( l.find_with( other_key( i.key ), other_less()) == l.end());
                EXPECT_EQ( l.find_with( other_key( i.key ), other_less())->first.nKey, i.key );
                EXPECT_EQ( l.find_with( other_key( i.key ), other_less())->second.val, i.key * 3 );

                EXPECT_FALSE( l.empty());
            }

            ASSERT_FALSE( l.empty());
            EXPECT_CONTAINER_SIZE( l, nSize );

            // erase
            for ( auto const&i : arr ) {
                switch ( i.key % 4 ) {
                    case 0:
                        EXPECT_TRUE( l.erase( i.key ));
                        break;
                    case 1:
                        EXPECT_TRUE( l.erase_with( other_key( i.key ), other_less()));
                        break;
                    case 2:
                        EXPECT_TRUE( l.erase( i.key, [ &i ]( list_value_type const& n ) {
                            EXPECT_EQ( n.first.nKey, i.key );
                            EXPECT_EQ( n.first.nKey * 3, n.second.val );
                        }));
                        break;
                    case 3:
                        EXPECT_TRUE( l.erase_with( other_key( i.key ), other_less(), [ &i ]( list_value_type const& n) {
                            EXPECT_EQ( n.first.nKey, i.key );
                            EXPECT_EQ( n.first.nKey * 3, n.second.val );
                        } ));
                }

                EXPECT_FALSE( l.contains( i.key ));
                EXPECT_FALSE( l.contains( key_type( i.key )));
                EXPECT_FALSE( l.contains( other_key( i.key ), other_less()));
                EXPECT_FALSE( l.find( key_type( i.key ), []( list_value_type& ) {} ));
                EXPECT_FALSE( l.find( i.key, []( list_value_type& ) {} ));
                EXPECT_FALSE( l.find_with( other_key( i.key ), other_less(), []( list_value_type& ) {} ));
            }

            ASSERT_TRUE( l.empty());
            EXPECT_CONTAINER_SIZE( l, 0 );

            // clear test
            for ( auto& i : arr )
                EXPECT_TRUE( l.insert( i.key, i.val ));

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

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            for ( auto& i : arr )
                EXPECT_TRUE( l.insert( i.key, i.val ));

            int key = 0;
            for ( auto& it : l ) {
                EXPECT_EQ( key, it.first.key());
                EXPECT_EQ( it.second.val, it.first.key());
                it.second.val = it.first.key() * 10;
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            key = 0;
            for ( auto it = l.cbegin(); it != l.cend(); ++it ) {
                EXPECT_EQ( key, it->first.key());
                EXPECT_EQ( it->first.key() * 10, it->second.val );
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            key = 0;
            for ( auto it = l.begin(); it != l.end(); ++it ) {
                EXPECT_EQ( key, it->first.key());
                EXPECT_EQ( it->first.key() * 10, it->second.val );
                it->second.val = it->first.key() * 2;
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            List const& cl = l;
            key = 0;
            for ( auto it = cl.begin(); it != cl.end(); ++it ) {
                EXPECT_EQ( key, it->first.nKey );
                EXPECT_EQ( it->first.nKey * 2, it->second.val );
                ++key;
            }
            EXPECT_EQ( static_cast<size_t>(key), nSize );

            l.clear();
            ASSERT_TRUE( l.empty());
            EXPECT_CONTAINER_SIZE( l, 0 );
        }
    };

} // namespace cds_test

#endif // CDSUNIT_LIST_TEST_KV_ITERABLE_LIST_H
