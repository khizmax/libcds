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

#ifndef CDSUNIT_TREE_TEST_BRONSON_AVLTREE_MAP_H
#define CDSUNIT_TREE_TEST_BRONSON_AVLTREE_MAP_H

#include "test_tree_map_data.h"
#include <cds/container/bronson_avltree_map_rcu.h>
#include <cds/sync/pool_monitor.h>
#include <cds/memory/vyukov_queue_pool.h>

namespace {

    namespace cc = cds::container;

    class bronson_avltree_map: public cds_test::tree_map_fixture
    {
    public:
        static size_t const kSize = 1000;

    protected:
        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

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
                ASSERT_FALSE( m.contains( other_item( i.nKey ), other_less()));
                ASSERT_FALSE( m.find( i, []( key_type const&, value_type& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( key_type const&, value_type& ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const&, value_type& ) {
                    EXPECT_TRUE( false );
                } ));

                std::pair< bool, bool > updResult;

                switch ( i.nKey % 16 ) {
                case 0:
                    ASSERT_TRUE( m.insert( i ));
                    ASSERT_FALSE( m.insert( i ));
                    ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, value_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    break;
                case 1:
                    ASSERT_TRUE( m.insert( i.nKey ));
                    ASSERT_FALSE( m.insert( i.nKey ));
                    ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, value_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    break;
                case 2:
                    ASSERT_TRUE( m.insert( std::to_string( i.nKey )));
                    ASSERT_FALSE( m.insert( std::to_string( i.nKey )));
                    ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, value_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
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
                    ASSERT_TRUE( m.insert_with( i, []( key_type const& key, value_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( i, []( key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 7:
                    ASSERT_TRUE( m.insert_with( i.nKey, []( key_type const& key, value_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( i.nKey, []( key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 8:
                    ASSERT_TRUE( m.insert_with( val.strVal, []( key_type const& key, value_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    ASSERT_FALSE( m.insert_with( val.strVal, []( key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    } ));
                    break;
                case 9:
                    updResult = m.update( i.nKey, []( bool, key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i.nKey, []( bool bNew, key_type const& key, value_type& val ) {
                        EXPECT_TRUE( bNew );
                        val.nVal = key.nKey;
                    });
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i.nKey, []( bool bNew, key_type const& key, value_type& val ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( key.nKey, val.nVal );
                        val.strVal = std::to_string( val.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 10:
                    updResult = m.update( i, []( bool, key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i, []( bool bNew, key_type const& key, value_type& val ) {
                        EXPECT_TRUE( bNew );
                        val.nVal = key.nKey;
                    });
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i, []( bool bNew, key_type const& key, value_type& val ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( key.nKey, val.nVal );
                        val.strVal = std::to_string( val.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 11:
                    updResult = m.update( val.strVal, []( bool, key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( val.strVal, []( bool bNew, key_type const& key, value_type& val ) {
                        EXPECT_TRUE( bNew );
                        val.nVal = key.nKey;
                    });
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( val.strVal, []( bool bNew, key_type const& key, value_type& val ) {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( key.nKey, val.nVal );
                        val.strVal = std::to_string( val.nVal );
                    } );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 12:
                    ASSERT_TRUE( m.emplace( i.nKey ));
                    ASSERT_FALSE( m.emplace( i.nKey ));
                    ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, value_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
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
                ASSERT_TRUE( m.contains( other_item( i.nKey ), other_less()));
                ASSERT_TRUE( m.find( i, []( key_type const& key, value_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, value_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const& key, value_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
            }
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            ASSERT_TRUE( m.check_consistency());

            shuffle( arrKeys.begin(), arrKeys.end() );

            // erase/find
            for ( auto const& i : arrKeys ) {
                value_type const& val( arrVals.at( i.nKey ) );

                ASSERT_TRUE( m.contains( i.nKey ));
                ASSERT_TRUE( m.contains( val.strVal ) );
                ASSERT_TRUE( m.contains( i ));
                ASSERT_TRUE( m.contains( other_item( i.nKey ), other_less()));
                ASSERT_TRUE( m.find( i, []( key_type const& key, value_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, value_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const& key, value_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));


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
                    ASSERT_TRUE( m.erase_with( other_item( i.nKey ), other_less()));
                    ASSERT_FALSE( m.erase_with( other_item( i.nKey ), other_less()));
                    break;
                case 4:
                    ASSERT_TRUE( m.erase( i, []( key_type const& key, value_type& val ) {
                        EXPECT_EQ( key.nKey, val.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                    }));
                    ASSERT_FALSE( m.erase( i, []( key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 5:
                    ASSERT_TRUE( m.erase( i.nKey, []( key_type const& key, value_type& val ) {
                        EXPECT_EQ( key.nKey, val.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                    }));
                    ASSERT_FALSE( m.erase( i.nKey, []( key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 6:
                    ASSERT_TRUE( m.erase( val.strVal, []( key_type const& key, value_type& val ) {
                        EXPECT_EQ( key.nKey, val.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                    }));
                    ASSERT_FALSE( m.erase( val.strVal, []( key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 7:
                    ASSERT_TRUE( m.erase_with( other_item( i.nKey ), other_less(), []( key_type const& key, value_type& val ) {
                        EXPECT_EQ( key.nKey, val.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                    }));
                    ASSERT_FALSE( m.erase_with( other_item( i.nKey ), other_less(), []( key_type const& /*key*/, value_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                }

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.contains( val.strVal ));
                ASSERT_FALSE( m.contains( other_item( i.nKey ), other_less()));
                ASSERT_FALSE( m.find( i, []( key_type const& /*key*/, value_type& /*val*/ ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( key_type const& /*key*/, value_type& /*val*/ ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const& /*key*/, value_type& /*val*/ ) {
                    EXPECT_TRUE( false );
                } ));
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


            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i, arrVals[ i.nKey ] ));
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            typedef typename Map::exempt_ptr exempt_ptr;

            // extract
            shuffle( arrKeys.begin(), arrKeys.end() );

            exempt_ptr xp;
            for ( auto const& i : arrKeys ) {
                value_type const& val = arrVals.at( i.nKey );

                ASSERT_TRUE( m.contains( i.nKey ) );

                switch ( i.nKey % 4 ) {
                case 0:
                    xp = m.extract( i.nKey );
                    break;
                case 1:
                    xp = m.extract( i );
                    break;
                case 2:
                    xp = m.extract( val.strVal );
                    break;
                case 3:
                    xp = m.extract_with( other_item( i.nKey ), other_less() );
                    break;
                }
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->nVal, i.nKey );

                ASSERT_FALSE( m.contains( i.nKey ) );

                switch ( i.nKey % 4 ) {
                case 0:
                    xp = m.extract( i.nKey );
                    break;
                case 1:
                    xp = m.extract( i );
                    break;
                case 2:
                    xp = m.extract( val.strVal );
                    break;
                case 3:
                    xp = m.extract_with( other_item( i.nKey ), other_less() );
                    break;
                }
                EXPECT_TRUE( !xp );
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );


            // extract_min
            shuffle( arrKeys.begin(), arrKeys.end() );
            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i, arrVals[ i.nKey ] ));

            size_t nCount = 0;
            int nKey = -1;
            while ( !m.empty() ) {
                switch ( nCount % 3 ) {
                case 0:
                    xp = m.extract_min();
                    break;
                case 1:
                    xp = m.extract_min( [nKey]( key_type const& key ) {
                        EXPECT_EQ( nKey + 1, key.nKey );
                    });
                    break;
                case 2:
                    {
                        key_type key;
                        xp = m.extract_min_key( key );
                        EXPECT_EQ( nKey + 1, key.nKey );
                    }
                    break;
                }
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->nVal, nKey + 1 );
                nKey = xp->nVal;
                ++nCount;
            }
            xp = m.extract_min();
            ASSERT_TRUE( !xp );
            EXPECT_EQ( kkSize, nCount );
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );

            // extract_max
            shuffle( arrKeys.begin(), arrKeys.end() );
            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i, arrVals[ i.nKey ] ));

            nKey = kkSize;
            nCount = 0;
            while ( !m.empty() ) {
                switch ( nCount % 3 ) {
                case 0:
                    xp = m.extract_max();
                    break;
                case 1:
                    xp = m.extract_max( [nKey]( key_type const& key ) {
                        EXPECT_EQ( nKey - 1, key.nKey );
                    } );
                    break;
                case 2:
                    {
                        key_type key;
                        xp = m.extract_max_key( key );
                        EXPECT_EQ( nKey - 1, key.nKey );
                    }
                    break;
                }
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->nVal, nKey - 1 );
                nKey = xp->nVal;
                ++nCount;
            }

            xp = m.extract_max();
            ASSERT_TRUE( !xp );
            EXPECT_EQ( kkSize, nCount );
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );

        }
    };


    template <class RCU>
    class BronsonAVLTreeMap: public bronson_avltree_map
    {
        typedef bronson_avltree_map base_class;
    public:
        typedef cds::urcu::gc<RCU> rcu_type;

    protected:
        void SetUp()
        {
            RCU::Construct();
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            RCU::Destruct();
        }
    };

    TYPED_TEST_CASE_P( BronsonAVLTreeMap );

    TYPED_TEST_P( BronsonAVLTreeMap, compare )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type,
            typename cc::bronson_avltree::make_traits<
                cds::opt::compare< typename TestFixture::cmp >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, less )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type,
            typename cc::bronson_avltree::make_traits<
                cds::opt::less< typename TestFixture::less >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, cmpmix )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type,
            typename cc::bronson_avltree::make_traits<
                cds::opt::less< typename TestFixture::less >
                , cds::opt::compare< typename TestFixture::cmp >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, stat )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::bronson_avltree::traits
        {
            typedef typename TestFixture::less  less;
            typedef cc::bronson_avltree::stat<> stat;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, item_counting )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::bronson_avltree::traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    struct bronson_relaxed_insert_traits: public cc::bronson_avltree::traits
    {
        static bool const relaxed_insert = true;
    };

    TYPED_TEST_P( BronsonAVLTreeMap, relaxed_insert )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_relaxed_insert_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, seq_cst )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::bronson_avltree::traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, sync_monitor )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::bronson_avltree::traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::vyukov_queue_pool< std::mutex >> sync_monitor;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, lazy_sync_monitor )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::bronson_avltree::traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::lazy_vyukov_queue_pool< std::mutex >> sync_monitor;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, rcu_check_deadlock )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::bronson_avltree::traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::vyukov_queue_pool< std::mutex >> sync_monitor;
            typedef cds::opt::v::rcu_assert_deadlock rcu_check_deadlock;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMap, rcu_no_check_deadlock )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::bronson_avltree::traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::lazy_vyukov_queue_pool< std::mutex >> sync_monitor;
            typedef cds::opt::v::rcu_no_check_deadlock rcu_check_deadlock;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    REGISTER_TYPED_TEST_CASE_P( BronsonAVLTreeMap,
        compare, less, cmpmix, stat, item_counting, relaxed_insert, seq_cst, sync_monitor, lazy_sync_monitor, rcu_check_deadlock, rcu_no_check_deadlock
    );

} // namespace

#endif // #ifndef CDSUNIT_TREE_TEST_BRONSON_AVLTREE_MAP_H
