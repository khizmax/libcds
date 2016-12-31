/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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
    typedef typename TestFixture::template cmp<base_item_type> cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef cmp compare;
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
    typedef typename TestFixture::template cmp<base_item_type> cmp;
    typedef typename TestFixture::template less<base_item_type> less_predicate;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
    };

    typedef ci::SkipListSet< rcu_type, base_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, base_xorshift )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::base_hook< ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef cmp compare;
        typedef ci::skip_list::xorshift random_level_generator;
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
    typedef typename TestFixture::template cmp<member_item_type> cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef cmp compare;
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
    typedef typename TestFixture::template cmp<member_item_type> cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}

TYPED_TEST_P( IntrusiveSkipListSet, member_xorshift )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less_predicate;
    typedef typename TestFixture::template cmp<member_item_type> cmp;

    struct traits : public ci::skip_list::traits
    {
        typedef ci::skip_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
        typedef mock_disposer disposer;
        typedef cmp compare;
        typedef less_predicate less;
        typedef ci::skip_list::stat<> stat;
        typedef ci::skip_list::xorshift random_level_generator;
    };

    typedef ci::SkipListSet< rcu_type, member_item_type, traits > set_type;

    set_type s;
    this->test( s );
}


// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( IntrusiveSkipListSet,
    base_cmp, base_less, base_cmpmix, base_xorshift, member_cmp, member_less, member_cmpmix, member_xorshift
);


#endif // CDSUNIT_SET_TEST_INTRUSIVE_SKIPLIST_RCU_H

