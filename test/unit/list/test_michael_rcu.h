// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_LIST_TEST_MICHAEL_LIST_RCU_H
#define CDSUNIT_LIST_TEST_MICHAEL_LIST_RCU_H

#include "test_list_rcu.h"
#include <cds/container/michael_list_rcu.h>

namespace cc = cds::container;

template <class RCU>
class MichaelList : public cds_test::list_rcu
{
    typedef cds_test::list_rcu base_class;
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

TYPED_TEST_CASE_P( MichaelList );

TYPED_TEST_P( MichaelList, less_ordered )
{
    typedef cc::MichaelList< typename TestFixture::rcu_type, typename TestFixture::item,
        typename cc::michael_list::make_traits<
            cds::opt::less< typename TestFixture::template lt< typename TestFixture::item>>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelList, compare_ordered )
{
    typedef cc::MichaelList< typename TestFixture::rcu_type, typename TestFixture::item,
        typename cc::michael_list::make_traits<
            cds::opt::compare< typename TestFixture::template cmp<typename TestFixture::item>>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelList, mix_ordered )
{
    typedef cc::MichaelList< typename TestFixture::rcu_type, typename TestFixture::item,
        typename cc::michael_list::make_traits<
            cds::opt::compare< typename TestFixture::template cmp<typename TestFixture::item>>
            ,cds::opt::less< typename TestFixture::template lt< typename TestFixture::item>>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelList, item_counting )
{
    struct traits : public cc::michael_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::MichaelList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelList, backoff )
{
    struct traits : public cc::michael_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::empty back_off;
    };
    typedef cc::MichaelList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelList, seq_cst )
{
    struct traits : public cc::michael_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::opt::v::sequential_consistent memory_model;
    };
    typedef cc::MichaelList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelList, stat )
{
    struct traits: public cc::michael_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::container::michael_list::stat<> stat;
    };
    typedef cc::MichaelList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( MichaelList, wrapped_stat )
{
    struct traits: public cc::michael_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::container::michael_list::wrapped_stat<> stat;
    };
    typedef cc::MichaelList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    cds::container::michael_list::stat<> st;
    list_type l( st );
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( MichaelList,
    less_ordered, compare_ordered, mix_ordered, item_counting, backoff, seq_cst, stat, wrapped_stat
    );

#endif // CDSUNIT_LIST_TEST_MICHAEL_LIST_RCU_H
