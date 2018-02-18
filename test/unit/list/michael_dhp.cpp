// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_list_hp.h"
#include <cds/container/michael_list_dhp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class MichaelList_DHP : public cds_test::list_hp
    {
    protected:
        void SetUp()
        {
            typedef cc::MichaelList< gc_type, item > list_type;

            cds::gc::dhp::GarbageCollector::Construct( list_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( MichaelList_DHP, less_ordered )
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

    TEST_F( MichaelList_DHP, compare_ordered )
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

    TEST_F( MichaelList_DHP, mix_ordered )
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

    TEST_F( MichaelList_DHP, item_counting )
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

    TEST_F( MichaelList_DHP, backoff )
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

    TEST_F( MichaelList_DHP, seq_cst )
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

    TEST_F( MichaelList_DHP, stat )
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

    TEST_F( MichaelList_DHP, wrapped_stat )
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
