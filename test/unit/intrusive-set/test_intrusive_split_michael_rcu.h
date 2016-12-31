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
#ifndef CDSUNIT_SET_TEST_INTRUSIVE_SPLIT_MICHAEL_RCU_H
#define CDSUNIT_SET_TEST_INTRUSIVE_SPLIT_MICHAEL_RCU_H

#include "test_intrusive_set_rcu.h"
#include <cds/intrusive/michael_list_rcu.h>
#include <cds/intrusive/split_list_rcu.h>
#include <cds/intrusive/free_list.h>

namespace ci = cds::intrusive;

template <class RCU>
class IntrusiveSplitMichaelSet: public cds_test::intrusive_set_rcu
{
    typedef cds_test::intrusive_set_rcu base_class;
public:
    typedef cds::urcu::gc<RCU> rcu_type;
    typedef typename base_class::base_int_item< ci::split_list::node<ci::michael_list::node<rcu_type>>>   base_item_type;
    typedef typename base_class::member_int_item< ci::split_list::node<ci::michael_list::node<rcu_type>>> member_item_type;

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

TYPED_TEST_CASE_P( IntrusiveSplitMichaelSet );

TYPED_TEST_P( IntrusiveSplitMichaelSet, base_cmp )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<base_item_type> cmp;
    typedef typename TestFixture::hash_int hash_int;

    typedef ci::MichaelList< rcu_type
        , base_item_type
        , typename ci::michael_list::make_traits<
            ci::opt::hook< ci::michael_list::base_hook< ci::opt::gc< rcu_type > > >
            , ci::opt::compare< cmp >
            , ci::opt::disposer< mock_disposer >
        >::type
    > bucket_type;

    typedef ci::SplitListSet< rcu_type, bucket_type,
        typename ci::split_list::make_traits<
            ci::opt::hash< hash_int >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, base_less )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<base_item_type> less;
    typedef typename TestFixture::hash_int hash_int;

    typedef ci::MichaelList< rcu_type
        , base_item_type
        , typename ci::michael_list::make_traits<
            ci::opt::hook< ci::michael_list::base_hook< ci::opt::gc< rcu_type >>>
            , ci::opt::less< less >
            , ci::opt::disposer< mock_disposer >
        >::type
    > bucket_type;

    typedef ci::SplitListSet< rcu_type, bucket_type,
        typename ci::split_list::make_traits<
            ci::opt::hash< hash_int >
            , ci::opt::item_counter< cds::atomicity::item_counter >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, base_cmpmix )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits : public ci::michael_list::traits
    {
        typedef ci::michael_list::base_hook< ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template less<base_item_type> less;
        typedef typename TestFixture::template cmp<base_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, base_item_type, list_traits > bucket_type;

    struct set_traits : public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
        typedef ci::split_list::stat<> stat;
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, base_static_bucket_table )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits: public ci::michael_list::traits
    {
        typedef ci::michael_list::base_hook< ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template less<base_item_type> less;
        typedef typename TestFixture::template cmp<base_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, base_item_type, list_traits > bucket_type;

    struct set_traits: public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
        typedef ci::split_list::stat<> stat;
        enum {
            dynamic_bucket_table = false
        };
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, base_static_bucket_table_free_list )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits: public ci::michael_list::traits
    {
        typedef ci::michael_list::base_hook< ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template cmp<base_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, base_item_type, list_traits > bucket_type;

    struct set_traits: public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
        enum {
            dynamic_bucket_table = false
        };
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, base_free_list )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::base_item_type base_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits: public ci::michael_list::traits
    {
        typedef ci::michael_list::base_hook< ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template less<base_item_type> less;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, base_item_type, list_traits > bucket_type;

    struct set_traits: public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
        typedef ci::split_list::stat<> stat;
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, member_cmp )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template cmp<member_item_type> cmp;
    typedef typename TestFixture::hash_int hash_int;

    typedef ci::MichaelList< rcu_type
        , member_item_type
        , typename ci::michael_list::make_traits<
            ci::opt::hook< ci::michael_list::member_hook<
                offsetof( member_item_type, hMember ),
                ci::opt::gc<rcu_type>
            >>
            , ci::opt::compare< cmp >
            , ci::opt::disposer< mock_disposer >
        >::type
    >    bucket_type;

    typedef ci::SplitListSet< rcu_type, bucket_type,
        typename ci::split_list::make_traits<
            ci::opt::hash< hash_int >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, member_less )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::template less<member_item_type> less;
    typedef typename TestFixture::hash_int hash_int;

    typedef ci::MichaelList< rcu_type
        , member_item_type
        , typename ci::michael_list::make_traits<
            ci::opt::hook< ci::michael_list::member_hook<
                offsetof( member_item_type, hMember ),
                ci::opt::gc<rcu_type>
            > >
            , ci::opt::less< less >
            , ci::opt::disposer< mock_disposer >
        >::type
    > bucket_type;

    typedef ci::SplitListSet< rcu_type, bucket_type,
        typename ci::split_list::make_traits<
            ci::opt::hash< hash_int >
            , ci::opt::back_off< cds::backoff::pause >
        >::type
    > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, member_cmpmix )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits : public ci::michael_list::traits
    {
        typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template less<member_item_type> less;
        typedef typename TestFixture::template cmp<member_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, member_item_type, list_traits > bucket_type;

    struct set_traits : public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, member_static_bucket_table )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits: public ci::michael_list::traits
    {
        typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template less<member_item_type> less;
        typedef typename TestFixture::template cmp<member_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, member_item_type, list_traits > bucket_type;

    struct set_traits: public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
        enum {
            dynamic_bucket_table = false
        };
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, member_static_bucket_table_free_list )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits: public ci::michael_list::traits
    {
        typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template cmp<member_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, member_item_type, list_traits > bucket_type;

    struct set_traits: public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
        enum {
            dynamic_bucket_table = false
        };
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

TYPED_TEST_P( IntrusiveSplitMichaelSet, member_free_list )
{
    typedef typename TestFixture::rcu_type rcu_type;
    typedef typename TestFixture::member_item_type member_item_type;
    typedef typename TestFixture::mock_disposer mock_disposer;
    typedef typename TestFixture::hash_int hash_int;

    struct list_traits: public ci::michael_list::traits
    {
        typedef ci::michael_list::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc<rcu_type>> hook;
        typedef typename TestFixture::template less<member_item_type> less;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList< rcu_type, member_item_type, list_traits > bucket_type;

    struct set_traits: public ci::split_list::traits
    {
        typedef hash_int hash;
        typedef typename TestFixture::simple_item_counter item_counter;
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet< rcu_type, bucket_type, set_traits > set_type;

    set_type s( TestFixture::kSize, 2 );
    this->test( s );
}

// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( IntrusiveSplitMichaelSet,
    base_cmp, base_less, base_cmpmix, base_static_bucket_table, base_static_bucket_table_free_list, base_free_list, member_cmp, member_less, member_cmpmix, member_static_bucket_table, member_static_bucket_table_free_list, member_free_list
);


#endif // CDSUNIT_SET_TEST_INTRUSIVE_SPLIT_MICHAEL_RCU_H

