// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_list_hp.h"
#include <cds/container/michael_list_hp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class MichaelList_HP : public cds_test::list_hp
    {
    protected:
        void SetUp()
        {
            typedef cc::MichaelList< gc_type, item > list_type;

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

    TEST_F( MichaelList_HP, less_ordered )
    {
        typedef cc::MichaelList< gc_type, item,
            typename cc::michael_list::make_traits<
                cds::opt::less< lt<item> >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelList_HP, compare_ordered )
    {
        typedef cc::MichaelList< gc_type, item,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp<item> >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelList_HP, mix_ordered )
    {
        typedef cc::MichaelList< gc_type, item,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp<item> >
                ,cds::opt::less< lt<item> >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelList_HP, item_counting )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelList_HP, backoff )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::empty back_off;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelList_HP, seq_cst )
    {
        struct traits : public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelList_HP, stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::stat<> stat;

        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( MichaelList_HP, wrapped_stat )
    {
        struct traits: public cc::michael_list::traits
        {
            typedef lt<item> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::container::michael_list::wrapped_stat<> stat;

        };
        typedef cc::MichaelList<gc_type, item, traits > list_type;

        cds::container::michael_list::stat<> st;
        list_type l( st );
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

} // namespace
