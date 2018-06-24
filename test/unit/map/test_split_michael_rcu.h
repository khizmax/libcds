// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_MAP_TEST_SPLIT_LIST_MICHAEL_RCU_H
#define CDSUNIT_MAP_TEST_SPLIT_LIST_MICHAEL_RCU_H

#include "test_map_rcu.h"
#include <cds/container/michael_list_rcu.h>
#include <cds/container/split_list_map_rcu.h>
#include <cds/intrusive/free_list.h>

namespace cc = cds::container;

template <class RCU>
class SplitListMichaelMap: public cds_test::container_map_rcu
{
    typedef cds_test::container_map_rcu base_class;
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

TYPED_TEST_CASE_P( SplitListMichaelMap );

TYPED_TEST_P( SplitListMichaelMap, compare )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    typedef cc::SplitListMap< rcu_type, key_type, value_type,
        typename cc::split_list::make_traits<
            cc::split_list::ordered_list< cc::michael_list_tag >
            , cds::opt::hash< hash1 >
            , cc::split_list::ordered_list_traits<
                typename cc::michael_list::make_traits<
                    cds::opt::compare< typename TestFixture::cmp >
                >::type
            >
        >::type
    > map_type;

    map_type m( TestFixture::kSize, 2 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, less )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    typedef cc::SplitListMap< rcu_type, key_type, value_type,
        typename cc::split_list::make_traits<
            cc::split_list::ordered_list< cc::michael_list_tag >
            , cds::opt::hash< hash1 >
            , cc::split_list::ordered_list_traits<
                typename cc::michael_list::make_traits<
                    cds::opt::less< typename TestFixture::less >
                >::type
            >
        >::type
    > map_type;

    map_type m( TestFixture::kSize, 2 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, cmpmix )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    typedef cc::SplitListMap< rcu_type, key_type, value_type,
        typename cc::split_list::make_traits<
            cc::split_list::ordered_list< cc::michael_list_tag >
            , cds::opt::hash< hash1 >
            , cc::split_list::ordered_list_traits<
                typename cc::michael_list::make_traits<
                    cds::opt::less< typename TestFixture::less >
                    , cds::opt::compare< typename TestFixture::cmp >
                >::type
            >
        >::type
    > map_type;

    map_type m( TestFixture::kSize, 3 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, item_counting )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::atomicity::item_counter item_counter;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::backoff::empty back_off;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 8 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, stat )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::split_list::stat<> stat;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 4 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, back_off )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
        typedef cds::opt::v::sequential_consistent memory_model;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 2 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, free_list )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::intrusive::FreeList free_list;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 2 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, static_bucket_table )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        enum: bool{ dynamic_bucket_table = false };
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::atomicity::item_counter item_counter;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 4 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, static_bucket_table_free_list )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        enum: bool { dynamic_bucket_table = false };
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::intrusive::FreeList free_list;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 4 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, bit_reversal_swar )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::algo::bit_reversal::swar bit_reversal;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 2 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, bit_reversal_lookup )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::algo::bit_reversal::lookup bit_reversal;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 2 );
    this->test( m );
}

TYPED_TEST_P( SplitListMichaelMap, bit_reversal_muldiv )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash1 hash;
        typedef cds::algo::bit_reversal::muldiv bit_reversal;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
        };
    };
    typedef cc::SplitListMap< rcu_type, key_type, value_type, map_traits > map_type;

    map_type m( TestFixture::kSize, 2 );
    this->test( m );
}


REGISTER_TYPED_TEST_CASE_P( SplitListMichaelMap,
    compare, less, cmpmix, item_counting, stat, back_off, free_list, static_bucket_table, static_bucket_table_free_list, bit_reversal_swar, bit_reversal_lookup, bit_reversal_muldiv
);


#endif // CDSUNIT_MAP_TEST_SPLIT_LIST_MICHAEL_RCU_H

