// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_generic_queue.h"

#include <cds/gc/dhp.h>
#include <cds/container/speculative_pairing_queue.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;


    class SPQueue_DHP : public cds_test::generic_queue
    {
    protected:
        void SetUp()
        {
            typedef cc::SPQueue< gc_type, int > queue_type;

            cds::gc::dhp::smr::construct( queue_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( SPQueue_DHP, defaulted )
    {
        typedef cds::container::SPQueue< gc_type, int > test_queue;

        test_queue q;
        test(q);
    }

    TEST_F( SPQueue_DHP, item_counting )
    {
        typedef cds::container::SPQueue < gc_type, int,
            typename cds::container::speculative_pairing_queue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( SPQueue_DHP, relaxed )
    {
        typedef cds::container::SPQueue < gc_type, int,
            typename cds::container::speculative_pairing_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( SPQueue_DHP, aligned )
    {
        typedef cds::container::SPQueue < gc_type, int,
            typename cds::container::speculative_pairing_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::padding < 32 >
            >::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( SPQueue_DHP, seq_cst )
    {
        struct traits : public cc::speculative_pairing_queue::traits
        {
            typedef cds::opt::v::sequential_consistent memory_model;
            typedef cds::atomicity::item_counter item_counter;
            enum { padding = 64 };
        };
        typedef cds::container::SPQueue < gc_type, int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( SPQueue_DHP, move )
    {
        typedef cds::container::SPQueue< gc_type, std::string > test_queue;

        test_queue q;
        test_string( q );
    }

    TEST_F( SPQueue_DHP, move_item_counting )
    {
        struct traits : public cc::speculative_pairing_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::SPQueue< gc_type, std::string, traits > test_queue;

        test_queue q;
        test_string( q );
    }

} // namespace

