// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_iterable_list_hp.h"
#include <cds/intrusive/iterable_list_dhp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveIterableList_DHP : public cds_test::intrusive_iterable_list_hp
    {
    protected:
        void SetUp()
        {
            typedef ci::IterableList< gc_type, item_type > list_type;

            cds::gc::dhp::smr::construct( list_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( IntrusiveIterableList_DHP, less )
    {
        typedef ci::IterableList< gc_type, item_type,
            typename ci::iterable_list::make_traits<
                ci::opt::disposer< mock_disposer >
                ,cds::opt::less< less< item_type >>
                , cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
       > list_type;

       list_type l;
       test_common( l );
       test_ordered_iterator( l );
       test_hp( l );
    }

    TEST_F( IntrusiveIterableList_DHP, compare )
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

    TEST_F( IntrusiveIterableList_DHP, item_counting )
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

    TEST_F( IntrusiveIterableList_DHP, backoff )
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

    TEST_F( IntrusiveIterableList_DHP, seqcst )
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

    TEST_F( IntrusiveIterableList_DHP, stat )
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

    TEST_F( IntrusiveIterableList_DHP, wrapped_stat )
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

} // namespace
