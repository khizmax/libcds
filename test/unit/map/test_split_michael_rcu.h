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
#ifndef CDSUNIT_MAP_TEST_SPLIT_LIST_MICHAEL_RCU_H
#define CDSUNIT_MAP_TEST_SPLIT_LIST_MICHAEL_RCU_H

#include "test_map_rcu.h"
#include <cds/container/michael_list_rcu.h>
#include <cds/container/split_list_map_rcu.h>

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

namespace {
    struct set_static_traits: public cc::split_list::traits
    {
        static bool const dynamic_bucket_table = false;
    };
}

TYPED_TEST_P( SplitListMichaelMap, static_bucket_table )
{
    typedef typename TestFixture::rcu_type   rcu_type;
    typedef typename TestFixture::key_type   key_type;
    typedef typename TestFixture::value_type value_type;
    typedef typename TestFixture::hash1      hash1;

    struct map_traits: public set_static_traits
    {
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

REGISTER_TYPED_TEST_CASE_P( SplitListMichaelMap,
    compare, less, cmpmix, item_counting, stat, back_off, static_bucket_table
);


#endif // CDSUNIT_MAP_TEST_SPLIT_LIST_MICHAEL_RCU_H
 