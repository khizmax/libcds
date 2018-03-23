// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_LIST_TEST_MICHAEL_KV_LIST_RCU_H
#define CDSUNIT_LIST_TEST_MICHAEL_KV_LIST_RCU_H

#include "test_kv_list_rcu.h"
#include <cds/container/michael_kvlist_rcu.h>

namespace cc = cds::container;

template <class RCU>
class MichaelKVList : public cds_test::kv_list_rcu
{
    typedef cds_test::kv_list_rcu base_class;
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

TYPED_TEST_CASE_P( MichaelKVList );

TYPED_TEST_P( MichaelKVList, less_ordered )
{
    typedef cc::MichaelKVList< typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type,
        typename cc::michael_list::make_traits<
            cds::opt::less< typename TestFixture::lt >
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelKVList, compare_ordered )
{
    typedef cc::MichaelKVList< typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type,
        typename cc::michael_list::make_traits<
            cds::opt::compare< typename TestFixture::cmp>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelKVList, mix_ordered )
{
    typedef cc::MichaelKVList< typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type,
        typename cc::michael_list::make_traits<
            cds::opt::compare< typename TestFixture::cmp>
            ,cds::opt::less< typename TestFixture::lt>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelKVList, item_counting )
{
    struct traits : public cc::michael_list::traits
    {
        typedef typename TestFixture::lt less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::MichaelKVList<typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelKVList, backoff )
{
    struct traits : public cc::michael_list::traits
    {
        typedef typename TestFixture::lt less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::empty back_off;
    };
    typedef cc::MichaelKVList<typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelKVList, seq_cst )
{
    struct traits : public cc::michael_list::traits
    {
        typedef typename TestFixture::lt less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::opt::v::sequential_consistent memory_model;
    };
    typedef cc::MichaelKVList<typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelKVList, stat )
{
    struct traits: public cc::michael_list::traits
    {
        typedef typename TestFixture::lt less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::container::michael_list::stat<> stat;
    };
    typedef cc::MichaelKVList<typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelKVList, wrapped_stat )
{
    struct traits: public cc::michael_list::traits
    {
        typedef typename TestFixture::lt less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::container::michael_list::wrapped_stat<> stat;
    };
    typedef cc::MichaelKVList<typename TestFixture::rcu_type, typename TestFixture::key_type, typename TestFixture::value_type, traits > list_type;

    cds::container::michael_list::stat<> st;
    list_type l( st );
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( MichaelKVList,
    less_ordered, compare_ordered, mix_ordered, item_counting, backoff, seq_cst, stat, wrapped_stat
    );

#endif // CDSUNIT_LIST_TEST_MICHAEL_KV_LIST_RCU_H
