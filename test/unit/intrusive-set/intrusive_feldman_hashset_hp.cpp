// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_feldman_hashset_hp.h"

#include <cds/intrusive/feldman_hashset_hp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveFeldmanHashSet_HP : public cds_test::intrusive_feldman_hashset_hp
    {
    protected:
        typedef cds_test::intrusive_feldman_hashset_hp base_class;

    protected:

        void SetUp()
        {
            typedef ci::FeldmanHashSet< gc_type, int_item,
                typename ci::feldman_hashset::make_traits<
                    ci::feldman_hashset::hash_accessor< hash_accessor >
                    ,ci::opt::less< std::less<int>>
                    ,ci::opt::disposer<mock_disposer>
                >::type
            > set_type;

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

    TEST_F( IntrusiveFeldmanHashSet_HP, compare )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef mock_disposer disposer;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, less )
    {
        typedef ci::FeldmanHashSet< gc_type, int_item,
            typename ci::feldman_hashset::make_traits<
                ci::feldman_hashset::hash_accessor< hash_accessor >
                , ci::opt::less< std::less<int>>
                , ci::opt::disposer<mock_disposer>
            >::type
        > set_type;

        set_type s( 5, 2 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, cmpmix )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef std::less<int> less;
            typedef mock_disposer disposer;
            typedef simple_item_counter item_counter;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 3, 4 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, backoff )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef mock_disposer disposer;
            typedef cds::backoff::empty back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, stat )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef mock_disposer disposer;
            typedef ci::feldman_hashset::stat<> stat;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, explicit_hash_size )
    {
        struct traits: public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor2 hash_accessor;
            enum: size_t {
                hash_size = sizeof( std::declval<key_val>().nKey )
            };
            typedef base_class::cmp2 compare;
            typedef mock_disposer disposer;
            typedef ci::feldman_hashset::stat<> stat;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item2, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, byte_cut )
    {
        struct traits: public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cds::algo::byte_splitter< int > hash_splitter;
            typedef cmp compare;
            typedef std::less<int> less;
            typedef mock_disposer disposer;
            typedef simple_item_counter item_counter;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 8 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, byte_cut_explicit_hash_size )
    {
        struct traits: public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor2 hash_accessor;
            typedef cds::algo::byte_splitter< key_val > hash_splitter;
            enum: size_t {
                hash_size = sizeof( std::declval<key_val>().nKey )
            };
            typedef base_class::cmp2 compare;
            typedef mock_disposer disposer;
            typedef ci::feldman_hashset::stat<> stat;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item2, traits > set_type;

        set_type s( 8, 8 );
        test( s );
    }

} // namespace
