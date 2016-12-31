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

#ifndef CDSUNIT_LIST_TEST_LAZY_LIST_RCU_H
#define CDSUNIT_LIST_TEST_LAZY_LIST_RCU_H

#include "test_list_rcu.h"
#include <cds/container/lazy_list_rcu.h>

namespace cc = cds::container;

template <class RCU>
class LazyList : public cds_test::list_rcu
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

TYPED_TEST_CASE_P( LazyList );

TYPED_TEST_P( LazyList, less_ordered )
{
    typedef cc::LazyList< typename TestFixture::rcu_type, typename TestFixture::item,
        typename cc::lazy_list::make_traits<
            cds::opt::less< typename TestFixture::template lt< typename TestFixture::item>>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, compare_ordered )
{
    typedef cc::LazyList< typename TestFixture::rcu_type, typename TestFixture::item,
        typename cc::lazy_list::make_traits<
            cds::opt::compare< typename TestFixture::template cmp< typename TestFixture::item>>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, mix_ordered )
{
    typedef cc::LazyList< typename TestFixture::rcu_type, typename TestFixture::item,
        typename cc::lazy_list::make_traits<
            cds::opt::less< typename TestFixture::template lt< typename TestFixture::item>>
            ,cds::opt::compare< typename TestFixture::template cmp< typename TestFixture::item>>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, item_counting )
{
    struct traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cc::LazyList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, backoff )
{
    struct traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::empty back_off;
    };
    typedef cc::LazyList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, seq_cst )
{
    struct traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::opt::v::sequential_consistent memory_model;
    };
    typedef cc::LazyList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, stat )
{
    struct traits: public cc::lazy_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::container::lazy_list::stat<> stat;
    };
    typedef cc::LazyList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, wrapped_stat )
{
    struct traits: public cc::lazy_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::container::lazy_list::wrapped_stat<> stat;
    };
    typedef cc::LazyList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    cds::container::lazy_list::stat<> st;
    list_type l( st );
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( LazyList, mutex )
{
    struct traits : public cc::lazy_list::traits
    {
        typedef typename TestFixture::template lt< typename TestFixture::item> less;
        typedef cds::atomicity::item_counter item_counter;
        typedef std::mutex lock_type;
    };
    typedef cc::LazyList<typename TestFixture::rcu_type, typename TestFixture::item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( LazyList,
    less_ordered, compare_ordered, mix_ordered, item_counting, backoff, seq_cst, mutex, stat, wrapped_stat
    );

#endif // CDSUNIT_LIST_TEST_LAZY_LIST_RCU_H
