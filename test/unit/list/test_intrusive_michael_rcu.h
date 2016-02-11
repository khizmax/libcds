/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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
#ifndef CDSUNIT_LIST_TEST_INTRUSIVE_MICHAEL_LIST_RCU_H
#define CDSUNIT_LIST_TEST_INTRUSIVE_MICHAEL_LIST_RCU_H

#include "test_intrusive_list_rcu.h"
#include <cds/intrusive/michael_list_rcu.h>

namespace ci = cds::intrusive;

template <class RCU>
class IntrusiveMichaelList : public cds_test::intrusive_list_rcu
{
    typedef cds_test::intrusive_list_rcu base_class;
public:
    typedef cds::urcu::gc<RCU> rcu_type;
    typedef typename base_class::base_item< ci::michael_list::node< rcu_type >> base_item;
    typedef typename base_class::member_item< ci::michael_list::node< rcu_type >> member_item;

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

TYPED_TEST_CASE_P( IntrusiveMichaelList );

TYPED_TEST_P( IntrusiveMichaelList, base_hook )
{
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::base_item,
        typename ci::michael_list::make_traits<
            ci::opt::hook< ci::michael_list::base_hook< cds::opt::gc< typename TestFixture::rcu_type >>>
            , ci::opt::disposer< typename TestFixture::mock_disposer >
            , cds::opt::less< typename TestFixture::template less< typename TestFixture::base_item >>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, base_hook_cmp )
{
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::base_item,
        typename ci::michael_list::make_traits<
            ci::opt::hook< ci::michael_list::base_hook< cds::opt::gc< typename TestFixture::rcu_type >>>
            , ci::opt::disposer< typename TestFixture::mock_disposer >
            , cds::opt::compare< typename TestFixture::template cmp< typename TestFixture::base_item >>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, base_hook_item_counting )
{
    struct traits : public ci::michael_list::traits {
        typedef ci::michael_list::base_hook< cds::opt::gc< typename TestFixture::rcu_type >> hook;
        typedef typename TestFixture::mock_disposer disposer;
        typedef typename TestFixture::template cmp< typename TestFixture::base_item > compare;
        typedef typename TestFixture::template less< typename TestFixture::base_item > less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::base_item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, base_hook_backoff )
{
    struct traits : public ci::michael_list::traits {
        typedef ci::michael_list::base_hook< cds::opt::gc< typename TestFixture::rcu_type >> hook;
        typedef typename TestFixture::mock_disposer disposer;
        typedef typename TestFixture::template cmp< typename TestFixture::base_item > compare;
        typedef typename TestFixture::template less< typename TestFixture::base_item > less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::pause back_off;
    };
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::base_item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, base_hook_seqcst )
{
    struct traits : public ci::michael_list::traits {
        typedef ci::michael_list::base_hook< cds::opt::gc< typename TestFixture::rcu_type >> hook;
        typedef typename TestFixture::mock_disposer disposer;
        typedef typename TestFixture::template cmp< typename TestFixture::base_item > compare;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::opt::v::sequential_consistent memory_model;
    };
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::base_item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, member_hook )
{
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::member_item,
        typename ci::michael_list::make_traits< 
            ci::opt::hook< ci::michael_list::member_hook< offsetof( typename TestFixture::member_item, hMember ), cds::opt::gc< typename TestFixture::rcu_type >>>
            ,ci::opt::disposer< typename TestFixture::mock_disposer >
            ,cds::opt::less< typename TestFixture::template less< typename TestFixture::member_item >>
        >::type 
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, member_hook_cmp )
{
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::member_item,
        typename ci::michael_list::make_traits<
            ci::opt::hook< ci::michael_list::member_hook< offsetof( typename TestFixture::member_item, hMember ), cds::opt::gc< typename TestFixture::rcu_type >>>
            ,ci::opt::disposer< typename TestFixture::mock_disposer >
            ,cds::opt::compare< typename TestFixture::template cmp< typename TestFixture::member_item >>
        >::type
    > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, member_hook_item_counting )
{
    struct traits : public ci::michael_list::traits {
        typedef ci::michael_list::member_hook< offsetof( typename TestFixture::member_item, hMember ), cds::opt::gc< typename TestFixture::rcu_type >> hook;
        typedef typename TestFixture::mock_disposer disposer;
        typedef typename TestFixture::template cmp< typename TestFixture::member_item > compare;
        typedef typename TestFixture::template less< typename TestFixture::member_item > less;
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::member_item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, member_hook_seqcst )
{
    struct traits : public ci::michael_list::traits {
        typedef ci::michael_list::member_hook< offsetof( typename TestFixture::member_item, hMember ), cds::opt::gc< typename TestFixture::rcu_type >> hook;
        typedef typename TestFixture::mock_disposer disposer;
        typedef typename TestFixture::template less< typename TestFixture::member_item > less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::opt::v::sequential_consistent memory_model;
    };
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::member_item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}

TYPED_TEST_P( IntrusiveMichaelList, member_hook_back_off )
{
    struct traits : public ci::michael_list::traits {
        typedef ci::michael_list::member_hook< offsetof( typename TestFixture::member_item, hMember ), cds::opt::gc< typename TestFixture::rcu_type >> hook;
        typedef typename TestFixture::mock_disposer disposer;
        typedef typename TestFixture::template cmp< typename TestFixture::member_item > compare;
        typedef typename TestFixture::template less< typename TestFixture::member_item > less;
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::backoff::empty back_off;
    };
    typedef ci::MichaelList< typename TestFixture::rcu_type, typename TestFixture::member_item, traits > list_type;

    list_type l;
    this->test_common( l );
    this->test_ordered_iterator( l );
    this->test_rcu( l );
}


// GCC 5: All test names should be written on single line, otherwise a runtime error will be encountered like as
// "No test named <test_name> can be found in this test case"
REGISTER_TYPED_TEST_CASE_P( IntrusiveMichaelList,
    base_hook, base_hook_cmp, base_hook_item_counting, base_hook_backoff, base_hook_seqcst, member_hook, member_hook_cmp, member_hook_item_counting, member_hook_seqcst, member_hook_back_off
);


#endif // CDSUNIT_LIST_TEST_INTRUSIVE_LIST_RCU_H 
 