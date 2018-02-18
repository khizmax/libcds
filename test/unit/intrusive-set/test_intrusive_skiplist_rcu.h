// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_SET_TEST_INTRUSIVE_SKIPLIST_RCU_H
#define CDSUNIT_SET_TEST_INTRUSIVE_SKIPLIST_RCU_H

#include "test_intrusive_set_rcu.h"
#include <cds/intrusive/skip_list_rcu.h>

namespace ci = cds::intrusive;

template <class RCU>
class IntrusiveSkipListSet: public cds_test::intrusive_set_rcu
{
    typedef cds_test::intrusive_set_rcu base_class;
public:
    typedef cds::urcu::gc<RCU> rcu_type;
    typedef typename base_class::base_int_item< ci::skip_list::node<rcu_type>>   base_item_type;
    typedef typename base_class::member_int_item< ci::skip_list::node<rcu_type>> member_item_type;

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

TYPED_TEST_CASE_P( IntrusiveSkipListSet );

TYPED_TEST_P( IntrusiveSkipListSet, base_cmp )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_less )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<base_item_type> less_predicate;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef less_predicate less;
        typedef cds::atomicity::item_counter item_counter;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_cmpmix )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;
    typedef typename TestFixture::template less<base_item_type> less_predicate;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_xorshift32 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef ci::skip_list::xorshift32 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_xorshift24 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef ci::skip_list::xorshift24 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_xorshift16 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef ci::skip_list::xorshift16 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_turbo32 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef ci::skip_list::turbo32 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_turbo24 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef ci::skip_list::turbo24 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_turbo16 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef ci::skip_list::turbo16 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}


TYPED_TEST_P( IntrusiveSkipListSet, member_cmp )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_less )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    //typedef typename TestFixture::hash_int hash_int;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef less_predicate less;
        typedef cds::atomicity::item_counter item_counter;
        typedef ci::opt::v::sequential_consistent memory_model;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_cmpmix )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_xorshift32 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
        typedef ci::skip_list::xorshift32 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_xorshift24 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
        typedef ci::skip_list::xorshift24 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_xorshift16 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
        typedef ci::skip_list::xorshift16 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_turbo32 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
        typedef ci::skip_list::turbo32 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_turbo24 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
        typedef ci::skip_list::turbo24 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_turbo16 )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> item_cmp;

    struct traits: public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef item_cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
        typedef ci::skip_list::turbo16 random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}


// All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( IntrusiveSkipListSet,
    base_cmp, base_less, base_cmpmix, base_xorshift32, base_xorshift24, base_xorshift16, base_turbo32, base_turbo24, base_turbo16, member_cmp, member_less, member_cmpmix, member_xorshift32, member_xorshift24, member_xorshift16, member_turbo32, member_turbo24, member_turbo16
);


#endif // CDSUNIT_SET_TEST_INTRUSIVE_SKIPLIST_RCU_H

