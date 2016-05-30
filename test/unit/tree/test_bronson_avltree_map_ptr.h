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

#ifndef CDSUNIT_TREE_TEST_BRONSON_AVLTREE_MAP_PTR_H
#define CDSUNIT_TREE_TEST_BRONSON_AVLTREE_MAP_PTR_H

#include "test_tree_map_data.h"
#include <cds/container/bronson_avltree_map_rcu.h>
#include <cds/sync/pool_monitor.h>
#include <cds/memory/vyukov_queue_pool.h>

namespace {

    namespace cc = cds::container;

    class bronson_avltree_map_ptr: public cds_test::tree_map_fixture
    {
    public:
        static size_t const kSize = 1000;

        struct value_type: public cds_test::tree_map_fixture::value_type
        {
            typedef cds_test::tree_map_fixture::value_type base_class;

            size_t nDisposeCount = 0;

            // Inheriting constructors
            using base_class::value_type;
        };

        struct mock_disposer
        {
            void operator()( value_type * val ) const
            {
                ++val->nDisposeCount;
            }
        };

    protected:
        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::key_type key_type;
            typedef typename std::remove_pointer< typename Map::mapped_type >::type mapped_type;
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
                value_type& val( arrVals.at( i.nKey ));

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.contains( other_item( i.nKey ), other_less()));
                ASSERT_FALSE( m.find( i, []( key_type const&, mapped_type& ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( key_type const&, mapped_type& ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const&, mapped_type& ) {
                    EXPECT_TRUE( false );
                } ));

                std::pair< bool, bool > updResult;

                switch ( i.nKey % 6 ) {
                case 0:
                    ASSERT_TRUE( m.insert( i, &val ));
                    ASSERT_FALSE( m.insert( i, &val ));
                    ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, mapped_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    break;
                case 1:
                    ASSERT_TRUE( m.insert( i.nKey, &val ));
                    ASSERT_FALSE( m.insert( i.nKey, &val ));
                    ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, mapped_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    break;
                case 2:
                    ASSERT_TRUE( m.insert( std::to_string( i.nKey ), &val ));
                    ASSERT_FALSE( m.insert( std::to_string( i.nKey ), &val ));
                    ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, mapped_type& val ) {
                        val.nVal = key.nKey;
                        val.strVal = std::to_string( key.nKey );
                    } ));
                    break;
                case 3:
                    updResult = m.update( i.nKey, &val, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i.nKey, &val );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i.nKey, &val );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 4:
                    updResult = m.update( i, &val, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( i, &val );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( i, &val );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                case 5:
                    updResult = m.update( val.strVal, &val, false );
                    ASSERT_FALSE( updResult.first );
                    ASSERT_FALSE( updResult.second );

                    updResult = m.update( val.strVal, &val );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_TRUE( updResult.second );

                    updResult = m.update( val.strVal, &val );
                    ASSERT_TRUE( updResult.first );
                    ASSERT_FALSE( updResult.second );
                    break;
                }

                ASSERT_TRUE( m.contains( i.nKey ));
                ASSERT_TRUE( m.contains( i ));
                ASSERT_TRUE( m.contains( other_item( i.nKey ), other_less()));
                ASSERT_TRUE( m.find( i, []( key_type const& key, mapped_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, mapped_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const& key, mapped_type& val ) {
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
                ASSERT_TRUE( m.find( i, []( key_type const& key, mapped_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find( i.nKey, []( key_type const& key, mapped_type& val ) {
                    EXPECT_EQ( key.nKey, val.nVal );
                    EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                } ));
                ASSERT_TRUE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const& key, mapped_type& val ) {
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
                    ASSERT_TRUE( m.erase( i, []( key_type const& key, mapped_type& val ) {
                        EXPECT_EQ( key.nKey, val.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), val.strVal );
                    }));
                    ASSERT_FALSE( m.erase( i, []( key_type const& /*key*/, mapped_type& /*val*/ ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 5:
                    ASSERT_TRUE( m.erase( i.nKey, []( key_type const& key, mapped_type& v ) {
                        EXPECT_EQ( key.nKey, v.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), v.strVal );
                    }));
                    ASSERT_FALSE( m.erase( i.nKey, []( key_type const&, mapped_type& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 6:
                    ASSERT_TRUE( m.erase( val.strVal, []( key_type const& key, mapped_type& v ) {
                        EXPECT_EQ( key.nKey, v.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), v.strVal );
                    }));
                    ASSERT_FALSE( m.erase( val.strVal, []( key_type const&, mapped_type& ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                case 7:
                    ASSERT_TRUE( m.erase_with( other_item( i.nKey ), other_less(), []( key_type const& key, mapped_type& v ) {
                        EXPECT_EQ( key.nKey, v.nVal );
                        EXPECT_EQ( std::to_string( key.nKey ), v.strVal );
                    }));
                    ASSERT_FALSE( m.erase_with( other_item( i.nKey ), other_less(), []( key_type const& key, mapped_type& v ) {
                        EXPECT_TRUE( false );
                    }));
                    break;
                }

                ASSERT_FALSE( m.contains( i.nKey ));
                ASSERT_FALSE( m.contains( i ));
                ASSERT_FALSE( m.contains( val.strVal ));
                ASSERT_FALSE( m.contains( other_item( i.nKey ), other_less()));
                ASSERT_FALSE( m.find( i, []( key_type const& /*key*/, mapped_type& /*val*/ ) {
                    ASSERT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find( i.nKey, []( key_type const& /*key*/, mapped_type& /*val*/ ) {
                    EXPECT_TRUE( false );
                } ));
                ASSERT_FALSE( m.find_with( other_item( i.nKey ), other_less(), []( key_type const& /*key*/, mapped_type& /*val*/ ) {
                    EXPECT_TRUE( false );
                } ));
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );

            Map::gc::force_dispose();
            for ( auto const& item: arrVals ) {
                EXPECT_EQ( item.nDisposeCount, 1 );
            }

            // clear
            for ( auto const& i : arrKeys ) {
                value_type& val( arrVals.at( i.nKey ) );
                ASSERT_TRUE( m.insert( i, &val ));
            }

            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            m.clear();

            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );

            Map::gc::force_dispose();
            for ( auto const& item : arrVals ) {
                EXPECT_EQ( item.nDisposeCount, 2 );
            }

            ASSERT_TRUE( m.check_consistency() );


            // RCU-specific test related to exempt_ptr
            typedef typename Map::exempt_ptr exempt_ptr;
            exempt_ptr xp;

            // extract
            for ( auto const& i : arrKeys ) {
                value_type& val( arrVals.at( i.nKey ) );
                ASSERT_TRUE( m.insert( i, &val ) );
            }
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            for ( auto const& i : arrKeys ) {
                value_type const& val = arrVals.at( i.nKey );

                EXPECT_TRUE( m.contains( i.nKey ) );

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

                EXPECT_FALSE( m.contains( i.nKey ) );
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );
            xp.release();

            Map::gc::force_dispose();
            for ( auto const& item : arrVals ) {
                EXPECT_EQ( item.nDisposeCount, 3 );
            }

            // extract_min
            shuffle( arrKeys.begin(), arrKeys.end() );
            for ( auto const& i : arrKeys ) {
                value_type& val( arrVals.at( i.nKey ) );
                ASSERT_TRUE( m.insert( i, &val ) );
            }
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );
            ASSERT_TRUE( m.check_consistency() );

            int nPrevKey = -1;
            size_t nCount = 0;
            while ( !m.empty() ) {
                switch ( nCount % 3 ) {
                case 0:
                    xp = m.extract_min();
                    break;
                case 1:
                    xp = m.extract_min( [nPrevKey]( key_type const& k ) {
                        EXPECT_EQ( k.nKey, nPrevKey + 1 );
                    } );
                    break;
                case 2:
                {
                    key_type key;
                    xp = m.extract_min_key( key );
                    EXPECT_EQ( key.nKey, nPrevKey + 1 );
                }
                break;
                }
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->nVal, nPrevKey + 1 );

                nPrevKey = xp->nVal;
                ++nCount;
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );
            EXPECT_EQ( nCount, kkSize );
            xp.release();

            Map::gc::force_dispose();
            for ( auto const& item : arrVals ) {
                EXPECT_EQ( item.nDisposeCount, 4 );
            }

            // extract_max
            shuffle( arrKeys.begin(), arrKeys.end() );
            for ( auto const& i : arrKeys ) {
                value_type& val( arrVals.at( i.nKey ) );
                ASSERT_TRUE( m.insert( i, &val ) );
            }
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );
            ASSERT_TRUE( m.check_consistency() );

            nPrevKey = static_cast<int>(kkSize);
            nCount = 0;
            while ( !m.empty() ) {
                switch ( nCount % 3 ) {
                case 0:
                    xp = m.extract_max();
                    break;
                case 1:
                    xp = m.extract_max( [nPrevKey]( key_type const& k ) {
                        EXPECT_EQ( k.nKey, nPrevKey - 1 );
                    } );
                    break;
                case 2:
                {
                    key_type key;
                    xp = m.extract_max_key( key );
                    EXPECT_EQ( key.nKey, nPrevKey - 1 );
                }
                break;
                }
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->nVal, nPrevKey - 1 );

                nPrevKey = xp->nVal;
                ++nCount;
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );
            EXPECT_EQ( nCount, kkSize );
            xp.release();

            Map::gc::force_dispose();
            for ( auto const& item : arrVals ) {
                EXPECT_EQ( item.nDisposeCount, 5 );
            }

            // extract min/max on empty map
            xp = m.extract_min();
            EXPECT_TRUE( !xp );
            xp = m.extract_min( []( key_type const& ) { EXPECT_FALSE( true ); } );
            EXPECT_TRUE( !xp );
            xp = m.extract_max();
            EXPECT_TRUE( !xp );
            xp = m.extract_max( []( key_type const& ) { EXPECT_FALSE( true ); } );
            EXPECT_TRUE( !xp );
            {
                key_type key;
                key.nKey = -100;
                xp = m.extract_min_key( key );
                EXPECT_TRUE( !xp );
                EXPECT_EQ( key.nKey, -100 );
                xp = m.extract_max_key( key );
                EXPECT_TRUE( !xp );
                EXPECT_EQ( key.nKey, -100 );
            }

            // checking empty map
            ASSERT_TRUE( m.check_consistency() );
        }
    };

    template <class RCU>
    class BronsonAVLTreeMapPtr: public bronson_avltree_map_ptr
    {
        typedef bronson_avltree_map_ptr base_class;
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

    struct bronson_traits: public cds::container::bronson_avltree::traits
    {
        typedef bronson_avltree_map_ptr::mock_disposer disposer;
    };

    TYPED_TEST_CASE_P( BronsonAVLTreeMapPtr );

    TYPED_TEST_P( BronsonAVLTreeMapPtr, compare )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*,
            typename cc::bronson_avltree::make_traits<
                cds::opt::compare< typename TestFixture::cmp >
                ,cds::intrusive::opt::disposer< bronson_avltree_map_ptr::mock_disposer >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, less )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*,
            typename cc::bronson_avltree::make_traits<
                cds::opt::less< typename TestFixture::less >
                , cds::intrusive::opt::disposer< bronson_avltree_map_ptr::mock_disposer >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, cmpmix )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*,
            typename cc::bronson_avltree::make_traits<
                cds::opt::less< typename TestFixture::less >
                ,cds::opt::compare< typename TestFixture::cmp >
                , cds::intrusive::opt::disposer< bronson_avltree_map_ptr::mock_disposer >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, stat )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_traits
        {
            typedef typename TestFixture::less  less;
            typedef cc::bronson_avltree::stat<> stat;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, item_counting )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    struct bronson_relaxed_insert_traits: public bronson_traits
    {
        static bool const relaxed_insert = true;
    };

    TYPED_TEST_P( BronsonAVLTreeMapPtr, relaxed_insert )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_relaxed_insert_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, seq_cst )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, sync_monitor )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::vyukov_queue_pool< std::mutex >> sync_monitor;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, lazy_sync_monitor )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::lazy_vyukov_queue_pool< std::mutex >> sync_monitor;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, rcu_check_deadlock )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::vyukov_queue_pool< std::mutex >> sync_monitor;
            typedef cds::opt::v::rcu_assert_deadlock rcu_check_deadlock;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( BronsonAVLTreeMapPtr, rcu_no_check_deadlock )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public bronson_traits
        {
            typedef typename TestFixture::cmp    compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::sync::pool_monitor< cds::memory::lazy_vyukov_queue_pool< std::mutex >> sync_monitor;
            typedef cds::opt::v::rcu_no_check_deadlock rcu_check_deadlock;
        };

        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type*, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    REGISTER_TYPED_TEST_CASE_P( BronsonAVLTreeMapPtr,
        compare, less, cmpmix, stat, item_counting, relaxed_insert, seq_cst, sync_monitor, lazy_sync_monitor, rcu_check_deadlock, rcu_no_check_deadlock
    );


} // namespace

#endif // #ifndef CDSUNIT_TREE_TEST_BRONSON_AVLTREE_MAP_PTR_H
