// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_SET_TEST_SPLIT_LIST_MICHAEL_RCU_H
#define CDSUNIT_SET_TEST_SPLIT_LIST_MICHAEL_RCU_H

#include "test_set_rcu.h"
#include <cds/container/michael_list_rcu.h>
#include <cds/container/split_list_set_rcu.h>
#include <cds/intrusive/free_list.h>

namespace cc = cds::container;

template <class RCU>
class SplitListMichaelSet: public cds_test::container_set_rcu
{
    typedef cds_test::container_set_rcu base_class;
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

TYPED_TEST_CASE_P( SplitListMichaelSet );

TYPED_TEST_P( SplitListMichaelSet, compare )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    typedef cc::SplitListSet< rcu_type, int_item,
        typename cc::split_list::make_traits<
            cc::split_list::ordered_list< cc::michael_list_tag >
            , cds::opt::hash< hash_int >
            , cc::split_list::ordered_list_traits<
                typename cc::michael_list::make_traits<
                    cds::opt::compare< typename TestFixture::cmp >
                >::type
            >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, less )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    typedef cc::SplitListSet< rcu_type, int_item,
        typename cc::split_list::make_traits<
            cc::split_list::ordered_list< cc::michael_list_tag >
            , cds::opt::hash< hash_int >
            , cc::split_list::ordered_list_traits<
                typename cc::michael_list::make_traits<
                    cds::opt::less< typename TestFixture::less >
                >::type
            >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, cmpmix )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    typedef cc::SplitListSet< rcu_type, int_item,
        typename cc::split_list::make_traits<
            cc::split_list::ordered_list< cc::michael_list_tag >
            , cds::opt::hash< hash_int >
            , cc::split_list::ordered_list_traits<
                typename cc::michael_list::make_traits<
                    cds::opt::less< typename TestFixture::less >
                    , cds::opt::compare< typename TestFixture::cmp >
                >::type
            >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 3 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, item_counting )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::atomicity::item_counter item_counter;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::backoff::empty back_off;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 8 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, stat )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
        typedef cc::split_list::stat<> stat;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, back_off )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::yield back_off;
        typedef cds::opt::v::sequential_consistent memory_model;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, free_list )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::intrusive::FreeList free_list;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, static_bucket_table )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        enum: bool { dynamic_bucket_table = false };
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::atomicity::item_counter item_counter;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, static_bucket_table_free_list )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        enum: bool { dynamic_bucket_table = false };
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::intrusive::FreeList free_list;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::pause back_off;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, bit_reversal_swar )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    typedef cc::SplitListSet< rcu_type, int_item,
        typename cc::split_list::make_traits<
            cc::split_list::ordered_list< cc::michael_list_tag >
            , cds::opt::hash< hash_int >
            , cc::split_list::bit_reversal< cds::algo::bit_reversal::swar >
            , cc::split_list::ordered_list_traits<
                typename cc::michael_list::make_traits<
                    cds::opt::less< typename TestFixture::less >
                    , cds::opt::compare< typename TestFixture::cmp >
                >::type
            >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, bit_reversal_lookup )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::algo::bit_reversal::lookup bit_reversal;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::backoff::empty back_off;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 8 );
    this->test( s );
}

TYPED_TEST_P( SplitListMichaelSet, bit_reversal_muldiv )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;
    typedef typename TestFixture::hash_int hash_int;

    struct set_traits: public cc::split_list::traits
    {
        typedef cc::michael_list_tag ordered_list;
        typedef hash_int hash;
        typedef cds::algo::bit_reversal::muldiv bit_reversal;

        struct ordered_list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::backoff::empty back_off;
        };
    };
    typedef cc::SplitListSet< rcu_type, int_item, set_traits > set_type;

    set_type s( TestFixture::kSize, 8 );
    this->test( s );
}

// GCC 5: All this->test names should be written on single line, otherwise a runtime error will be encountered like as
// "No this->test named <test_name> can be found in this this->test case"
REGISTER_TYPED_TEST_CASE_P( SplitListMichaelSet,
    compare, less, cmpmix, item_counting, stat, back_off, static_bucket_table, free_list, static_bucket_table_free_list, bit_reversal_swar, bit_reversal_lookup, bit_reversal_muldiv
);


#endif // CDSUNIT_SET_TEST_SPLIT_LIST_MICHAEL_RCU_H

