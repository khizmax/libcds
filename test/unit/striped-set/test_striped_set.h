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

#ifndef CDSUNIT_SET_TEST_STRIPED_SET_H
#define CDSUNIT_SET_TEST_STRIPED_SET_H

#include "test_set.h"

#include <cds/container/striped_set.h>

namespace {
    namespace cc = cds::container;

    template <bool Exist, typename Set>
    struct call_contains_with
    {
        bool operator()( Set& s, int key ) const
        {
            return s.contains( cds_test::container_set::other_item( key ), cds_test::container_set::other_less());
        }
    };

    template <typename Set>
    struct call_contains_with<false, Set>
    {
        bool operator()( Set& s, int key ) const
        {
            return s.contains( key );
        }
    };

    template <bool Exist, typename Set>
    struct call_find_with
    {
        template <typename Func>
        bool operator()( Set& s, int key, Func f ) const
        {
            return s.find_with( cds_test::container_set::other_item( key ), cds_test::container_set::other_less(), f );
        }
    };

    template <typename Set>
    struct call_find_with<false, Set>
    {
        template <typename Func>
        bool operator()( Set& s, int key, Func f ) const
        {
            return s.find( key, [&f]( typename Set::value_type& v, int key ) { 
                cds_test::container_set::other_item oi( key );
                f( v, oi ); 
            });
        }
    };

    template <bool Exist, typename Set>
    struct call_erase_with
    {
        bool operator()( Set& s, int key ) const
        {
            return s.erase_with( cds_test::container_set::other_item( key ), cds_test::container_set::other_less());
        }

        template <typename Func>
        bool operator()( Set& s, int key, Func f ) const
        {
            return s.erase_with( cds_test::container_set::other_item( key ), cds_test::container_set::other_less(), f );
        }
    };

    template <typename Set>
    struct call_erase_with<false, Set>
    {
        bool operator()( Set& s, int key ) const
        {
            return s.erase( key );
        }

        template <typename Func>
        bool operator()( Set& s, int key, Func f ) const
        {
            return s.erase( key, f );
        }
    };

    template <typename Traits >
    class StripedSet : public cds_test::container_set
    {
    protected:
        typedef cds_test::container_set base_class;

        typedef typename Traits::container_type container_type;
        typedef typename Traits::copy_policy    copy_policy;

        static bool const c_hasFindWith = Traits::c_hasFindWith;
        static bool const c_hasEraseWith = Traits::c_hasEraseWith;

        //void SetUp()
        //{}

        //void TearDown()
        //{}

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
                ASSERT_FALSE( s.contains( i ));
                ASSERT_FALSE( (call_contains_with< c_hasFindWith, Set >()( s, i.key())));
                ASSERT_FALSE( s.find( i.nKey, []( value_type&, int ) {} ) );
                ASSERT_FALSE( s.find( i, []( value_type&, value_type const& ) {} ) );
                ASSERT_FALSE( (call_find_with< c_hasFindWith, Set >()(s, i.key(), []( value_type&, other_item const& ) {} )));

                std::pair<bool, bool> updResult;

                std::string str;
                updResult = s.update( i.key(), []( bool bNew, value_type&, int )
                {
                    ASSERT_TRUE( false );
                }, false );
                EXPECT_FALSE( updResult.first );
                EXPECT_FALSE( updResult.second );

                switch ( idx % 8 ) {
                case 0:
                    ASSERT_TRUE( s.insert( i ) );
                    ASSERT_FALSE( s.insert( i ) );
                    updResult = s.update( i, []( bool bNew, value_type& val, value_type const& arg )
                    {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( val.key(), arg.key() );
                    }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 1:
                    ASSERT_TRUE( s.insert( i.key() ) );
                    ASSERT_FALSE( s.insert( i.key() ) );
                    updResult = s.update( i.key(), []( bool bNew, value_type& val, int arg )
                    {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( val.key(), arg );
                    }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 2:
                    ASSERT_TRUE( s.insert( i, []( value_type& v ) { ++v.nFindCount; } ) );
                    ASSERT_FALSE( s.insert( i, []( value_type& v ) { ++v.nFindCount; } ) );
                    ASSERT_TRUE( s.find( i.nKey, []( value_type const& v, int key )
                    {
                        EXPECT_EQ( v.key(), key );
                        EXPECT_EQ( v.nFindCount, 1 );
                    } ) );
                    break;
                case 3:
                    ASSERT_TRUE( s.insert( i.key(), []( value_type& v ) { ++v.nFindCount; } ) );
                    ASSERT_FALSE( s.insert( i.key(), []( value_type& v ) { ++v.nFindCount; } ) );
                    ASSERT_TRUE( s.find( i.nKey, []( value_type const& v, int key )
                    {
                        EXPECT_EQ( v.key(), key );
                        EXPECT_EQ( v.nFindCount, 1 );
                    } ) );
                    break;
                case 4:
                    updResult = s.update( i, []( bool bNew, value_type& v, value_type const& arg )
                    {
                        EXPECT_TRUE( bNew );
                        EXPECT_EQ( v.key(), arg.key() );
                        ++v.nUpdateNewCount;
                    } );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = s.update( i, []( bool bNew, value_type& v, value_type const& arg )
                    {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.key(), arg.key() );
                        ++v.nUpdateNewCount;
                    }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    ASSERT_TRUE( s.find( i.nKey, []( value_type const& v, int key )
                    {
                        EXPECT_EQ( v.key(), key );
                        EXPECT_EQ( v.nUpdateNewCount, 2 );
                    } ) );
                    break;
                case 5:
                    updResult = s.update( i.key(), []( bool bNew, value_type& v, int arg )
                    {
                        EXPECT_TRUE( bNew );
                        EXPECT_EQ( v.key(), arg );
                        ++v.nUpdateNewCount;
                    } );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );

                    updResult = s.update( i.key(), []( bool bNew, value_type& v, int arg )
                    {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( v.key(), arg );
                        ++v.nUpdateNewCount;
                    }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    ASSERT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                    {
                        EXPECT_EQ( v.key(), arg.key() );
                        EXPECT_EQ( v.nUpdateNewCount, 2 );
                    } ) );
                    break;
                case 6:
                    ASSERT_TRUE( s.emplace( i.key() ) );
                    ASSERT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                    {
                        EXPECT_EQ( v.key(), arg.key() );
                        EXPECT_EQ( v.nVal, arg.nVal );
                    } ) );
                    break;
                case 7:
                    str = "Hello!";
                    ASSERT_TRUE( s.emplace( i.key(), std::move( str ) ) );
                    EXPECT_TRUE( str.empty() );
                    ASSERT_TRUE( s.find( i, []( value_type const& v, value_type const& arg )
                    {
                        EXPECT_EQ( v.key(), arg.key() );
                        EXPECT_EQ( v.nVal, arg.nVal );
                        EXPECT_EQ( v.strVal, std::string( "Hello!" ) );
                    } ) );
                    break;
                default:
                    // forgot anything?..
                    ASSERT_TRUE( false );
                }

                ASSERT_TRUE( s.contains( i.nKey ) );
                ASSERT_TRUE( s.contains( i ) );
                ASSERT_TRUE( (call_contains_with< c_hasFindWith, Set>()( s, i.key() )));
                ASSERT_TRUE( s.find( i.nKey, []( value_type&, int ) {} ) );
                ASSERT_TRUE( s.find( i, []( value_type&, value_type const& ) {} ) );
                ASSERT_TRUE( (call_find_with< c_hasFindWith, Set >()(s, i.key(), []( value_type&, other_item const& ) {})));
            }

            ASSERT_FALSE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            // erase
            shuffle( indices.begin(), indices.end() );
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( s.contains( i.nKey ) );
                ASSERT_TRUE( s.contains( i ) );
                ASSERT_TRUE( (call_contains_with<c_hasFindWith, Set>()(s, i.key())));
                ASSERT_TRUE( s.find( i.nKey, []( value_type& v, int )
                {
                    v.nFindCount = 1;
                } ));
                ASSERT_TRUE( s.find( i, []( value_type& v, value_type const& )
                {
                    EXPECT_EQ( ++v.nFindCount, 2 );
                } ));
                ASSERT_TRUE( (call_find_with< c_hasFindWith, Set >()(s, i.key(), []( value_type& v, other_item const& ) {
                    EXPECT_EQ( ++v.nFindCount, 3 );
                })));

                int nKey = i.key() - 1;
                switch ( idx % 6 ) {
                case 0:
                    ASSERT_TRUE( s.erase( i.key() ) );
                    ASSERT_FALSE( s.erase( i.key() ) );
                    break;
                case 1:
                    ASSERT_TRUE( s.erase( i ) );
                    ASSERT_FALSE( s.erase( i ) );
                    break;
                case 2:
                    ASSERT_TRUE( (call_erase_with<c_hasEraseWith, Set>()( s, i.key())));
                    ASSERT_FALSE( (call_erase_with<c_hasEraseWith, Set>()(s, i.key())));
                    break;
                case 3:
                    ASSERT_TRUE( s.erase( i.key(), [&nKey]( value_type const& v )
                    {
                        nKey = v.key();
                    } ));
                    EXPECT_EQ( i.key(), nKey );

                    nKey = i.key() - 1;
                    ASSERT_FALSE( s.erase( i.key(), [&nKey]( value_type const& v )
                    {
                        nKey = v.key();
                    } ) );
                    EXPECT_EQ( i.key(), nKey + 1 );
                    break;
                case 4:
                    ASSERT_TRUE( s.erase( i, [&nKey]( value_type const& v )
                    {
                        nKey = v.key();
                    } ) );
                    EXPECT_EQ( i.key(), nKey );

                    nKey = i.key() - 1;
                    ASSERT_FALSE( s.erase( i, [&nKey]( value_type const& v )
                    {
                        nKey = v.key();
                    } ) );
                    EXPECT_EQ( i.key(), nKey + 1 );
                    break;
                case 5:
                    ASSERT_TRUE( (call_erase_with<c_hasEraseWith, Set>()(s, i.key(), [&nKey]( value_type const& v )
                    {
                        nKey = v.key();
                    })));
                    EXPECT_EQ( i.key(), nKey );

                    nKey = i.key() - 1;
                    ASSERT_FALSE( (call_erase_with<c_hasEraseWith, Set>()(s, i.key(), [&nKey]( value_type const& v )
                    {
                        nKey = v.key();
                    })));
                    EXPECT_EQ( i.key(), nKey + 1 );
                    break;
                }

                ASSERT_FALSE( s.contains( i.nKey ) );
                ASSERT_FALSE( s.contains( i ) );
                ASSERT_FALSE( (call_contains_with<c_hasFindWith, Set>()( s, i.key())));
                ASSERT_FALSE( s.find( i.nKey, []( value_type&, int ) {} ) );
                ASSERT_FALSE( s.find( i, []( value_type&, value_type const& ) {} ));
                ASSERT_FALSE( (call_find_with<c_hasFindWith, Set>()( s, i.key(), []( value_type&, other_item const& ) {})));
            }
            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );


            // clear
            for ( auto& i : data ) {
                ASSERT_TRUE( s.insert( i ) );
            }

            ASSERT_FALSE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            s.clear();

            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );
        }

    };

    template <typename Traits>
    class RefinableSet: public StripedSet< Traits >
    {};

    TYPED_TEST_CASE_P( StripedSet );
    TYPED_TEST_CASE_P( RefinableSet );

// ****************************************************************
// striped set

    TYPED_TEST_P( StripedSet, compare )
    {
        typedef cc::StripedSet< 
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::mutex_policy< cc::striped_set::striping<>>
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, less )
    {
        typedef cc::StripedSet< 
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >
        > set_type;

        set_type s;
        this->test( s );
    }


    TYPED_TEST_P( StripedSet, cmpmix )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > set_type;

        set_type s( 32 );
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, spinlock )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::striping<cds::sync::spin>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, load_factor_resizing )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, load_factor_resizing_rt )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>
        > set_type;

        set_type s( 30, cc::striped_set::load_factor_resizing<0>( 8 ));
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, single_bucket_resizing )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>
        > set_type;

        set_type s( 30 );
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, single_bucket_resizing_rt )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>
        > set_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        set_type s( 24, resizing_policy );
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, copy_policy_copy )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>,
            cds::opt::copy_policy< cc::striped_set::copy_item >
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, copy_policy_move )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>,
            cds::opt::copy_policy< cc::striped_set::move_item >
        > set_type;

        set_type s( 30, cc::striped_set::load_factor_resizing<0>( 8 ) );
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, copy_policy_swap )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>,
            cds::opt::copy_policy< cc::striped_set::swap_item >
        > set_type;

        set_type s( 30 );
        this->test( s );
    }

    TYPED_TEST_P( StripedSet, copy_policy_special )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>,
            cds::opt::copy_policy< typename TestFixture::copy_policy >
        > set_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        set_type s( 24, resizing_policy );
        this->test( s );
    }


// ****************************************************************
// refinable set

    TYPED_TEST_P( RefinableSet, compare )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, less )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >
        > set_type;

        set_type s;
        this->test( s );
    }


    TYPED_TEST_P( RefinableSet, cmpmix )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > set_type;

        set_type s( 32 );
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, spinlock )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<cds::sync::reentrant_spin>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, load_factor_resizing )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, load_factor_resizing_rt )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>
        > set_type;

        set_type s( 30, cc::striped_set::load_factor_resizing<0>( 8 ) );
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, single_bucket_resizing )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>
        > set_type;

        set_type s( 30 );
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, single_bucket_resizing_rt )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>
        > set_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        set_type s( 24, resizing_policy );
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, copy_policy_copy )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::less< typename TestFixture::less >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<4>>,
            cds::opt::copy_policy< cc::striped_set::copy_item >
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, copy_policy_move )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::load_factor_resizing<0>>,
            cds::opt::copy_policy< cc::striped_set::move_item >
        > set_type;

        set_type s( 30, cc::striped_set::load_factor_resizing<0>( 8 ) );
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, copy_policy_swap )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<6>>,
            cds::opt::copy_policy< cc::striped_set::swap_item >
        > set_type;

        set_type s( 30 );
        this->test( s );
    }

    TYPED_TEST_P( RefinableSet, copy_policy_special )
    {
        typedef cc::StripedSet<
            typename TestFixture::container_type,
            cds::opt::mutex_policy< cc::striped_set::refinable<>>,
            cds::opt::hash< typename TestFixture::hash1 >,
            cds::opt::compare< typename TestFixture::cmp >,
            cds::opt::resizing_policy< cc::striped_set::single_bucket_size_threshold<0>>,
            cds::opt::copy_policy< typename TestFixture::copy_policy >
        > set_type;

        cc::striped_set::single_bucket_size_threshold<0> resizing_policy( 8 );
        set_type s( 24, resizing_policy );
        this->test( s );
    }

    REGISTER_TYPED_TEST_CASE_P( StripedSet,
        compare, less, cmpmix, spinlock, load_factor_resizing, load_factor_resizing_rt, single_bucket_resizing, single_bucket_resizing_rt, copy_policy_copy, copy_policy_move, copy_policy_swap, copy_policy_special
        );

    REGISTER_TYPED_TEST_CASE_P( RefinableSet,
        compare, less, cmpmix, spinlock, load_factor_resizing, load_factor_resizing_rt, single_bucket_resizing, single_bucket_resizing_rt, copy_policy_copy, copy_policy_move, copy_policy_swap, copy_policy_special
        );

} // namespace

#endif // CDSUNIT_SET_TEST_INTRUSIVE_STRIPED_SET_H
