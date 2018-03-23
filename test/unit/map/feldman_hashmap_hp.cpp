// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_feldman_hashmap_hp.h"

#include <cds/container/feldman_hashmap_hp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class FeldmanHashMap_HP : public cds_test::feldman_hashmap_hp
    {
    protected:
        typedef cds_test::feldman_hashmap_hp base_class;

        void SetUp()
        {
            typedef cc::FeldmanHashMap< gc_type, key_type, value_type > map_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( map_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( FeldmanHashMap_HP, defaulted )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type > map_type;

        map_type m;
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, compare )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m( 4, 5 );
        EXPECT_GE( m.head_size(), static_cast<size_t>( 1 << 4 ));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>( 1 << 5 ));
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, less )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::less< less >
            >::type
        > map_type;

        map_type m( 3, 2 );
        EXPECT_GE( m.head_size(), static_cast<size_t>( 1 << 4 )); // min = 2 ** 4
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>( 1 << 2 ));
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, cmpmix )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::less< less >
                ,cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m( 4, 4 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>( 1 << 4 ));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>( 1 << 4 ));
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, backoff )
    {
        struct map_traits: public cc::feldman_hashmap::traits
        {
            typedef cmp compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( 8, 2 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>( 1 << 8 ));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>( 1 << 2 ));
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, stat )
    {
        struct map_traits: public cc::feldman_hashmap::traits
        {
            typedef cds::backoff::yield back_off;
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( 1, 1 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>( 1 << 4 )); // min = 2**4
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>( 1 << 2 )); // min = 2**2
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, explicit_key_size )
    {
        struct map_traits: public cc::feldman_hashmap::traits
        {
            enum: size_t {
                hash_size = sizeof(int) + sizeof( uint16_t)
            };
            typedef hash2 hash;
            typedef less2 less;
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef cc::FeldmanHashMap< gc_type, key_type2, value_type, map_traits > map_type;

        map_type m( 5, 3 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>(1 << 6));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>(1 << 3));
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, byte_cut )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::compare< cmp >
                , cc::feldman_hashmap::hash_splitter< cds::algo::byte_splitter< key_type >>
            >::type
        > map_type;

        map_type m( 8, 8 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>( 1 << 8 ));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>( 1 << 8 ));
        test( m );
    }

    TEST_F( FeldmanHashMap_HP, byte_cut_explicit_key_size )
    {
        struct map_traits: public cc::feldman_hashmap::traits
        {
            enum: size_t {
                hash_size = sizeof(int) + sizeof( uint16_t)
            };
            typedef cds::algo::byte_splitter< key_type2, hash_size > hash_splitter;
            typedef hash2 hash;
            typedef less2 less;
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef cc::FeldmanHashMap< gc_type, key_type2, value_type, map_traits > map_type;

        map_type m( 8, 8 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>(1 << 8));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>(1 << 8));
        test( m );
    }

} // namespace
