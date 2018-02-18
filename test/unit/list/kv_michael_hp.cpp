// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_kv_list_hp.h"
#include <cds/container/michael_kvlist_hp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class MichaelKVList_HP : public cds_test::kv_list_hp
    {
    protected:
        void SetUp()
        {
            typedef cc::MichaelKVList< gc_type, key_type, value_type > list_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( list_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( MichaelKVList_HP, less_ordered )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::less< lt >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelKVList_HP, compare_ordered )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelKVList_HP, mix_ordered )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp >
                ,cds::opt::less< lt >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelKVList_HP, item_counting )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelKVList_HP, backoff )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelKVList_HP, seq_cst )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelKVList_HP, stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::stat<> stat;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelKVList_HP, wrapped_stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::wrapped_stat<> stat;
        };
        typedef cc::MichaelKVList<gc_type, key_type, value_type, traits > list_type;

        cds::container::michael_list::stat<> st;
        list_type l( st );
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

} // namespace
