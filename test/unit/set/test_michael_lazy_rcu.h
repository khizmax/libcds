// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_SET_TEST_MICHAEL_LAZY_RCU_H
#define CDSUNIT_SET_TEST_MICHAEL_LAZY_RCU_H

#include "test_set_rcu.h"
#include <cds/container/lazy_list_rcu.h>
#include <cds/container/michael_set_rcu.h>

namespace cc = cds::container;

template <class RCU>
class MichaelLazySet: public cds_test::container_set_rcu
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

TYPED_TEST_CASE_P( MichaelLazySet );

TYPED_TEST_P( MichaelLazySet, compare )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    typedef cc::LazyList< rcu_type, int_item,
        typename cc::lazy_list::make_traits<
            cds::opt::compare< typename TestFixture::cmp >
        >::type
    > list_type;

    typedef cc::MichaelHashSet< rcu_type, list_type,
        typename cc::michael_set::make_traits<
            cds::opt::hash< typename TestFixture::hash_int >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( MichaelLazySet, less )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    typedef cc::LazyList< rcu_type, int_item,
        typename cc::lazy_list::make_traits<
            cds::opt::less< typename TestFixture::less >
        >::type
    > list_type;

    typedef cc::MichaelHashSet< rcu_type, list_type,
        typename cc::michael_set::make_traits<
            cds::opt::hash< typename TestFixture::hash_int >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( MichaelLazySet, cmpmix )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct list_traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::less less;
        typedef typename TestFixture::cmp compare;
    };
    typedef cc::LazyList< rcu_type, int_item, list_traits > list_type;

    typedef cc::MichaelHashSet< rcu_type, list_type,
        typename cc::michael_set::make_traits<
            cds::opt::hash< typename TestFixture::hash_int >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( MichaelLazySet, item_counting )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct list_traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::cmp compare;
    };
    typedef cc::LazyList< rcu_type, int_item, list_traits > list_type;

    struct set_traits: public cc::michael_set::traits
    {
        typedef typename TestFixture::hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
    };
    typedef cc::MichaelHashSet< rcu_type, list_type, set_traits >set_type;

    set_type s( TestFixture::kSize, 3 );
    this->test( s );
}

TYPED_TEST_P( MichaelLazySet, backoff )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct list_traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::cmp compare;
        typedef cds::backoff::make_exponential_t<cds::backoff::pause, cds::backoff::yield> back_off;
    };
    typedef cc::LazyList< rcu_type, int_item, list_traits > list_type;

    struct set_traits : public cc::michael_set::traits
    {
        typedef typename TestFixture::hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::MichaelHashSet< rcu_type, list_type, set_traits >set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
}

TYPED_TEST_P( MichaelLazySet, seq_cst )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct list_traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cds::backoff::pause back_off;
        typedef cds::opt::v::sequential_consistent memory_model;
    };
    typedef cc::LazyList< rcu_type, int_item, list_traits > list_type;

    struct set_traits : public cc::michael_set::traits
    {
        typedef typename TestFixture::hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::MichaelHashSet< rcu_type, list_type, set_traits >set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
}

TYPED_TEST_P( MichaelLazySet, mutex )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct list_traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cds::backoff::pause back_off;
        typedef std::mutex lock_type;
    };
    typedef cc::LazyList< rcu_type, int_item, list_traits > list_type;

    struct set_traits : public cc::michael_set::traits
    {
        typedef typename TestFixture::hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::MichaelHashSet< rcu_type, list_type, set_traits >set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
}

TYPED_TEST_P( MichaelLazySet, stat )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct list_traits: public cc::lazy_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cds::backoff::pause back_off;
        typedef cc::lazy_list::stat<> stat;
    };
    typedef cc::LazyList< rcu_type, int_item, list_traits > list_type;

    struct set_traits: public cc::michael_set::traits
    {
        typedef typename TestFixture::hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::MichaelHashSet< rcu_type, list_type, set_traits >set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
    EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
}

TYPED_TEST_P( MichaelLazySet, wrapped_stat )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::int_item int_item;

    struct list_traits: public cc::lazy_list::traits
    {
        typedef typename TestFixture::less less;
        typedef cds::backoff::pause back_off;
        typedef cc::lazy_list::wrapped_stat<> stat;
    };
    typedef cc::LazyList< rcu_type, int_item, list_traits > list_type;

    struct set_traits: public cc::michael_set::traits
    {
        typedef typename TestFixture::hash_int hash;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::MichaelHashSet< rcu_type, list_type, set_traits >set_type;

    set_type s( TestFixture::kSize, 4 );
    this->test( s );
    EXPECT_GE( s.statistics().m_nInsertSuccess, 0u );
}

// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( MichaelLazySet,
    compare, less, cmpmix, item_counting, backoff, seq_cst, mutex, stat, wrapped_stat
);


#endif // CDSUNIT_SET_TEST_INTRUSIVE_MICHAEL_LAZY_RCU_H

