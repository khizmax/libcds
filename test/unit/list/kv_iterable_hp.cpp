// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_kv_iterable_list_hp.h"
#include <cds/container/iterable_kvlist_hp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class IterableKVList_HP : public cds_test::kv_iterable_list_hp
    {
    protected:
        void SetUp()
        {
            typedef cc::IterableKVList< gc_type, key_type, value_type > list_type;

            // +3 - for guarded_ptr, iterators
            cds::gc::hp::GarbageCollector::Construct( list_type::c_nHazardPtrCount + 3, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( IterableKVList_HP, less_ordered )
    {
        typedef cc::IterableKVList< gc_type, key_type, value_type,
            typename cc::iterable_list::make_traits<
                cds::opt::less< lt >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IterableKVList_HP, compare_ordered )
    {
        typedef cc::IterableKVList< gc_type, key_type, value_type,
            typename cc::iterable_list::make_traits<
                cds::opt::compare< cmp >
                , cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IterableKVList_HP, mix_ordered )
    {
        typedef cc::IterableKVList< gc_type, key_type, value_type,
            typename cc::iterable_list::make_traits<
                cds::opt::compare< cmp >
                ,cds::opt::less< lt >
                , cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IterableKVList_HP, backoff )
    {
        struct traits : public cc::iterable_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::IterableKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IterableKVList_HP, seq_cst )
    {
        struct traits : public cc::iterable_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::IterableKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IterableKVList_HP, stat )
    {
        struct traits: public cc::iterable_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::iterable_list::stat<> stat;
        };
        typedef cc::IterableKVList<gc_type, key_type, value_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IterableKVList_HP, wrapped_stat )
    {
        struct traits: public cc::iterable_list::traits
        {
            typedef lt less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::iterable_list::wrapped_stat<> stat;
        };
        typedef cc::IterableKVList<gc_type, key_type, value_type, traits > list_type;

        cds::container::iterable_list::stat<> st;
        list_type l( st );
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

} // namespace
