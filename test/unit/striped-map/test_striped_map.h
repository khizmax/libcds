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

#ifndef CDSUNIT_STRIPED_MAP_TEST_STRIPED_MAP_H
#define CDSUNIT_STRIPED_MAP_TEST_STRIPED_MAP_H

#include "test_map_data.h"

#include <cds/container/striped_map.h>

namespace {
    namespace cc = cds::container;

    template <bool Exist, typename Map>
    struct call_contains_with
    {
        bool operator()( Map& m, int key ) const
        {
            return m.contains( cds_test::striped_map_fixture::other_item( key ), cds_test::striped_map_fixture::other_less() );
        }
    };

    template <typename Map>
    struct call_contains_with<false, Map>
    {
        bool operator()( Map& m, int key ) const
        {
            return m.contains( key );
        }
    };

    template <bool Exist, typename Map>
    struct call_find_with
    {
        template <typename Func>
        bool operator()( Map& m, int key, Func f ) const
        {
            return m.find_with( cds_test::striped_map_fixture::other_item( key ), cds_test::striped_map_fixture::other_less(), f );
        }
    };

    template <typename Map>
    struct call_find_with<false, Map>
    {
        template <typename Func>
        bool operator()( Map& m, int key, Func f ) const
        {
            return m.find( key, f );
        }
    };

    template <bool Exist, typename Map>
    struct call_erase_with
    {
        bool operator()( Map& m, int key ) const
        {
            return m.erase_with( cds_test::striped_map_fixture::other_item( key ), cds_test::striped_map_fixture::other_less() );
        }

        template <typename Func>
        bool operator()( Map& m, int key, Func f ) const
        {
            return m.erase_with( cds_test::striped_map_fixture::other_item( key ), cds_test::striped_map_fixture::other_less(), f );
        }
    };

    template <typename Map>
    struct call_erase_with<false, Map>
    {
        bool operator()( Map& m, int key ) const
        {
            return m.erase( key );
        }

        template <typename Func>
        bool operator()( Map& m, int key, Func f ) const
        {
            return m.erase( key, f );
        }
    };

    template <typename Traits>
    class StripedMap: public cds_test::striped_map_fixture
    {
    public:
        static size_t const kSize = 1000;

    protected:
        typedef typename Traits::container_type container_type;
        typedef typename Traits::copy_policy    copy_policy;

        static bool const c_hasFindWith = Traits::c_hasFindWith;
        static bool const c_hasEraseWith = Traits::c_hasEraseWith;

        //void SetUp()
        //{}

        //void TearDown()
        //{}

        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

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

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE(( call_contains_with<c_hasFindWith, Map>()( m, i.nKey ) ));
                ASSERT_FALSE( m.find( i, []( map_pair const& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE(( call_find_with< c_hasFindWith, Map>()( m, i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } )));

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
                    ASSERT_FALSE( m.insert_with( i, []( map_pair& v ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 7:
                    ASSERT_TRUE( m.insert_with( i.nKey, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( i.nKey, []( map_pair& v ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 8:
                    ASSERT_TRUE( m.insert_with( val.strVal, []( map_pair& v ) {
                        v.second.nVal = v.first.nKey;
                        v.second.strVal = std::to_string( v.first.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( val.strVal, []( map_pair& v ) {
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
                    }
                    break;
                case 15:
                    {
                        std::string str = val.strVal;
                        ASSERT_TRUE( m.emplace( i, i.nKey, std::move( str )));
                        ASSERT_TRUE( str.empty());
                        str = val.strVal;
                        ASSERT_FALSE( m.emplace( i, i.nKey, std::move( str )));
                    }
                    break;
                }

                ASSERT_TRUE( m.contains( i.nKey ));
                ASSERT_TRUE( m.contains( i ));
                ASSERT_TRUE(( call_contains_with<c_hasFindWith, Map>()( m, i.nKey ) ));
                ASSERT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE(( call_find_with<c_hasFindWith, Map>()( m, i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } )));
            }
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            shuffle( arrKeys.begin(), arrKeys.end() );

            // erase/find
            for ( auto const& i : arrKeys ) {
                value_type const& val( arrVals.at( i.nKey ) );

                ASSERT_TRUE( m.contains( i.nKey ));
                ASSERT_TRUE( m.contains( val.strVal ) );
                ASSERT_TRUE( m.contains( i ));
                ASSERT_TRUE(( call_contains_with<c_hasFindWith, Map>()( m, i.nKey )));
                ASSERT_TRUE( m.find( i, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } ));
                ASSERT_TRUE(( call_find_with<c_hasFindWith, Map>()( m, i.nKey, []( map_pair const& v ) {
                    EXPECT_EQ( v.first.nKey, v.second.nVal );
                    EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                } )));


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
                    ASSERT_TRUE(( call_erase_with< c_hasEraseWith, Map>()( m, i.nKey )));
                    ASSERT_FALSE(( call_erase_with< c_hasEraseWith, Map>()( m, i.nKey )));
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
                    ASSERT_TRUE(( call_erase_with< c_hasEraseWith, Map >()( m, i.nKey, []( map_pair& v ) {
                        EXPECT_EQ( v.first.nKey, v.second.nVal );
                        EXPECT_EQ( std::to_string( v.first.nKey ), v.second.strVal );
                    })));
                    ASSERT_FALSE(( call_erase_with< c_hasEraseWith, Map >()( m, i.nKey, []( map_pair& ) {
                        EXPECT_TRUE( false );
                    })));
                    break;
                }

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.contains( val.strVal ));
                ASSERT_FALSE(( call_contains_with< c_hasFindWith, Map >()( m, i.nKey )));
                ASSERT_FALSE( m.find( i, []( map_pair const& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE(( call_find_with< c_hasFindWith, Map >()( m, i.nKey, []( map_pair const& ) {
                    EXPECT_TRUE( false );
                } )));
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );

            // clear
            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ));

            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            m.clear();

            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );
        }
    };

    template <typename Traits>
    class RefinableMap: public StripedMap< Traits >
    {};

    TYPED_TEST_CASE_P( StripedMap );
    TYPED_TEST_CASE_P( RefinableMap );


    // ****************************************************************
    // striped map

    TYPED_TEST_P( StripedMap, compare )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::mutex_policy< cc::striped_set::striping<>>
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, less )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >
        > map_type;

        map_type m;
        this->test( m );
    }


    TYPED_TEST_P( StripedMap, cmpmix )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > map_type;

        map_type m( 32 );
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, spinlock )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::striping<cds::sync::spin>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, load_factor_resizing )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, load_factor_resizing_rt )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>
        > map_type;

        map_type m( 30, cc::striped_set::load_factor_resizing<0>( 8 ) );
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, single_bucket_resizing )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>
        > map_type;

        map_type m( 30 );
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, single_bucket_resizing_rt )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>
        > map_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        map_type m( 24, resizing_policy );
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, copy_policy_copy )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>,
            cds::opt::copy_policy< cc::striped_set::copy_item >
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, copy_policy_move )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>,
            cds::opt::copy_policy< cc::striped_set::move_item >
        > map_type;

        map_type m( 30, cc::striped_set::load_factor_resizing<0>( 8 ) );
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, copy_policy_swap )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>,
            cds::opt::copy_policy< cc::striped_set::swap_item >
        > map_type;

        map_type m( 30 );
        this->test( m );
    }

    TYPED_TEST_P( StripedMap, copy_policy_special )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>,
            cds::opt::copy_policy< typename TestFixture::copy_policy >
        > map_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        map_type m( 24, resizing_policy );
        this->test( m );
    }


    // ****************************************************************
    // refinable map

    TYPED_TEST_P( RefinableMap, compare )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, less )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >
        > map_type;

        map_type m;
        this->test( m );
    }


    TYPED_TEST_P( RefinableMap, cmpmix )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > map_type;

        map_type m( 32 );
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, spinlock )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<cds::sync::reentrant_spin>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, load_factor_resizing )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, load_factor_resizing_rt )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>
        > map_type;

        map_type m( 30, cc::striped_set::load_factor_resizing<0>( 8 ) );
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, single_bucket_resizing )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>
        > map_type;

        map_type m( 30 );
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, single_bucket_resizing_rt )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>
        > map_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        map_type m( 24, resizing_policy );
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, copy_policy_copy )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>,
            cds::opt::copy_policy< cc::striped_set::copy_item >
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, copy_policy_move )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>,
            cds::opt::copy_policy< cc::striped_set::move_item >
        > map_type;

        map_type m( 30, cc::striped_set::load_factor_resizing<0>( 8 ) );
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, copy_policy_swap )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>,
            cds::opt::copy_policy< cc::striped_set::swap_item >
        > map_type;

        map_type m( 30 );
        this->test( m );
    }

    TYPED_TEST_P( RefinableMap, copy_policy_special )
    {
        typedef cc::StripedMap<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>,
            cds::opt::copy_policy< typename TestFixture::copy_policy >
        > map_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        map_type m( 24, resizing_policy );
        this->test( m );
    }

    REGISTER_TYPED_TEST_CASE_P( StripedMap,
        compare, less, cmpmix, spinlock, load_factor_resizing, load_factor_resizing_rt, single_bucket_resizing, single_bucket_resizing_rt, copy_policy_copy, copy_policy_move, copy_policy_swap, copy_policy_special
    );

    REGISTER_TYPED_TEST_CASE_P( RefinableMap,
        compare, less, cmpmix, spinlock, load_factor_resizing, load_factor_resizing_rt, single_bucket_resizing, single_bucket_resizing_rt, copy_policy_copy, copy_policy_move, copy_policy_swap, copy_policy_special
    );
} // namespace

#endif // #ifndef CDSUNIT_STRIPED_MAP_TEST_STRIPED_MAP_H
