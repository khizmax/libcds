// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_MAP_TEST_MICHAEL_MICHAEL_RCU_H
#define CDSUNIT_MAP_TEST_MICHAEL_MICHAEL_RCU_H

#include "test_map_rcu.h"
#include <cds/container/michael_kvlist_rcu.h>
#include <cds/container/michael_map_rcu.h>

namespace {
    namespace cc = cds::container;

    template <class RCU>
    class MichaelMap: public cds_test::container_map_rcu
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

    TYPED_TEST_CASE_P( MichaelMap );

    TYPED_TEST_P( MichaelMap, compare )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::compare< typename TestFixture::cmp >
            >::type
        > list_type;

        typedef cc::MichaelHashMap< rcu_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< typename TestFixture::hash1 >
            >::type
        > map_type;

        map_type m( TestFixture::kSize, 2 );
        this->test( m );
    }

    TYPED_TEST_P( MichaelMap, less )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::MichaelKVList< rcu_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::less< typename TestFixture::less >
            >::type
        > list_type;

        typedef cc::MichaelHashMap< rcu_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< typename TestFixture::hash1 >
            >::type
        > map_type;

        map_type m( TestFixture::kSize, 2 );
        this->test( m );
    }

    TYPED_TEST_P( MichaelMap, cmpmix )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct list_traits : public cc::michael_list::traits
        {
            typedef typename TestFixture::less less;
            typedef typename TestFixture::cmp compare;
        };
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, list_traits > list_type;

        typedef cc::MichaelHashMap< rcu_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< typename TestFixture::hash1 >
            >::type
        > map_type;

        map_type m( TestFixture::kSize, 2 );
        this->test( m );
    }

    TYPED_TEST_P( MichaelMap, backoff )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct list_traits : public cc::michael_list::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef cds::backoff::make_exponential_t<cds::backoff::pause, cds::backoff::yield> back_off;
        };
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, list_traits > list_type;

        struct map_traits : public cc::michael_map::traits
        {
            typedef typename TestFixture::hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< rcu_type, list_type, map_traits >map_type;

        map_type m( TestFixture::kSize, 4 );
        this->test( m );
    }

    TYPED_TEST_P( MichaelMap, seq_cst )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct list_traits : public cc::michael_list::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::backoff::pause back_off;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, list_traits > list_type;

        struct map_traits : public cc::michael_map::traits
        {
            typedef typename TestFixture::hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< rcu_type, list_type, map_traits >map_type;

        map_type m( TestFixture::kSize, 4 );
        this->test( m );
    }

    TYPED_TEST_P( MichaelMap, stat )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::michael_list::stat<> stat;
        };
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef typename TestFixture::hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< rcu_type, list_type, map_traits >map_type;

        map_type m( TestFixture::kSize, 4 );
        this->test( m );
        EXPECT_GE( m.statistics().m_nInsertSuccess, 0u );
    }

    TYPED_TEST_P( MichaelMap, wrapped_stat )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::key_type key_type;
        typedef typename TestFixture::value_type value_type;

        struct list_traits: public cc::michael_list::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::backoff::pause back_off;
            typedef cc::michael_list::wrapped_stat<> stat;
        };
        typedef cc::MichaelKVList< rcu_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef typename TestFixture::hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< rcu_type, list_type, map_traits >map_type;

        map_type m( TestFixture::kSize, 4 );
        this->test( m );
        EXPECT_GE( m.statistics().m_nInsertSuccess, 0u );
    }

    REGISTER_TYPED_TEST_CASE_P( MichaelMap,
        compare, less, cmpmix, backoff, seq_cst, stat, wrapped_stat
    );
}

#endif // CDSUNIT_MAP_TEST_MICHAEL_MICHAEL_RCU_H

