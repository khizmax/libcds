// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_feldman_hashset_hp.h"

#include <cds/container/feldman_hashset_hp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class FeldmanHashSet_HP : public cds_test::feldman_hashset_hp
    {
    protected:
        typedef cds_test::feldman_hashset_hp base_class;

        void SetUp()
        {
            struct set_traits: public cc::feldman_hashset::traits
            {
                typedef get_hash hash_accessor;
            };
            typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( set_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( FeldmanHashSet_HP, defaulted )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, compare )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s( 4, 5 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, less )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                ,cds::opt::less< std::less<int> >
            >::type
        > set_type;

        set_type s( 3, 2 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, cmpmix )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                , cds::opt::less< std::less<int> >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s( 4, 4 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, item_counting )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef cmp compare;
            typedef std::less<int> less;
            typedef simple_item_counter item_counter;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

        set_type s( 3, 3 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, backoff )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef cmp compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

        set_type s( 8, 2 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, stat )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef cds::backoff::yield back_off;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

        set_type s( 1, 1 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, explicit_hash_size )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash2 hash_accessor;
            enum: size_t {
                hash_size = sizeof( std::declval<key_val>(). nKey )
            };
            typedef cmp2 compare;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item2, set_traits > set_type;

        set_type s( 1, 1 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, byte_cut )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                , cc::feldman_hashset::hash_splitter< cds::algo::byte_splitter<int>>
                , cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s( 8, 8 );
        test( s );
    }

    TEST_F( FeldmanHashSet_HP, byte_cut_explicit_hash_size )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash2 hash_accessor;
            enum: size_t {
                hash_size = sizeof( std::declval<key_val>().nKey )
            };
            typedef cds::algo::byte_splitter< key_val, hash_size > hash_splitter;
            typedef cmp2 compare;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item2, set_traits > set_type;

        set_type s( 8, 8 );
        test( s );
    }

} // namespace
