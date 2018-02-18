// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_iterable_list_hp.h"
#include <cds/intrusive/iterable_list_hp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveIterableList_HP : public cds_test::intrusive_iterable_list_hp
    {
    protected:
        void SetUp()
        {
            typedef ci::IterableList< gc_type, item_type > list_type;

            // +1 - for guarded_ptr
            // +3 - for iterator test
            cds::gc::hp::GarbageCollector::Construct( list_type::c_nHazardPtrCount + 3, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( IntrusiveIterableList_HP, less )
    {
        typedef ci::IterableList< gc_type, item_type,
            typename ci::iterable_list::make_traits<
                ci::opt::disposer< mock_disposer >
                ,cds::opt::less< less< item_type >>
                ,cds::opt::item_counter< cds::atomicity::item_counter >

            >::type
       > list_type;

       list_type l;
       test_common( l );
       test_ordered_iterator( l );
       test_hp( l );
    }

    TEST_F( IntrusiveIterableList_HP, compare )
    {
        typedef ci::IterableList< gc_type, item_type,
            typename ci::iterable_list::make_traits<
                ci::opt::disposer< mock_disposer >
                , cds::opt::compare< cmp< item_type >>
                , cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveIterableList_HP, item_counting )
    {
        struct traits : public ci::iterable_list::traits {
            typedef mock_disposer disposer;
            typedef cmp< item_type > compare;
            typedef intrusive_iterable_list::less< item_type > less;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::IterableList< gc_type, item_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveIterableList_HP, backoff )
    {
        struct traits : public ci::iterable_list::traits {
            typedef mock_disposer disposer;
            typedef cmp< item_type > compare;
            typedef intrusive_iterable_list::less< item_type > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
        };
        typedef ci::IterableList< gc_type, item_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveIterableList_HP, seqcst )
    {
        struct traits : public ci::iterable_list::traits {
            typedef mock_disposer disposer;
            typedef intrusive_iterable_list::less< item_type > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef ci::IterableList< gc_type, item_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveIterableList_HP, stat )
    {
        struct traits: public ci::iterable_list::traits {
            typedef mock_disposer disposer;
            typedef intrusive_iterable_list::less< item_type > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::iterable_list::stat<> stat;
        };
        typedef ci::IterableList< gc_type, item_type, traits > list_type;

        list_type l;
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveIterableList_HP, wrapped_stat )
    {
        struct traits: public ci::iterable_list::traits {
            typedef mock_disposer disposer;
            typedef intrusive_iterable_list::less< item_type > less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::iterable_list::wrapped_stat<> stat;
        };
        typedef ci::IterableList< gc_type, item_type, traits > list_type;

        traits::stat::stat_type st;

        list_type l( st );
        test_common( l );
        test_ordered_iterator( l );
        test_hp( l );
    }

    TEST_F( IntrusiveIterableList_HP, derived )
    {
        class list_type: public ci::IterableList< gc_type, item_type,
            typename ci::iterable_list::make_traits<
                ci::opt::disposer< mock_disposer >
                ,cds::opt::less< less< item_type >>
                ,cds::opt::item_counter< cds::atomicity::item_counter >

            >::type
        >
        {};

       list_type l;
       test_common( l );
       test_ordered_iterator( l );
       test_hp( l );
    }

} // namespace
