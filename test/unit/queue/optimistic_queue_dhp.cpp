// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_generic_queue.h"

#include <cds/gc/dhp.h>
#include <cds/container/optimistic_queue.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;


    class OptimisticQueue_DHP : public cds_test::generic_queue
    {
    protected:
        void SetUp()
        {
            typedef cc::OptimisticQueue< gc_type, int > queue_type;

            cds::gc::dhp::smr::construct( queue_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( OptimisticQueue_DHP, defaulted )
    {
        typedef cds::container::OptimisticQueue< gc_type, int > test_queue;

        test_queue q;
        test(q);
    }

    TEST_F( OptimisticQueue_DHP, item_counting )
    {
        typedef cds::container::OptimisticQueue < gc_type, int,
            typename cds::container::optimistic_queue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( OptimisticQueue_DHP, relaxed )
    {
        typedef cds::container::OptimisticQueue < gc_type, int,
            typename cds::container::optimistic_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( OptimisticQueue_DHP, aligned )
    {
        typedef cds::container::OptimisticQueue < gc_type, int,
            typename cds::container::optimistic_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::padding < 32 >
            >::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( OptimisticQueue_DHP, seq_cst )
    {
        struct traits : public cc::optimistic_queue::traits
        {
            typedef cds::opt::v::sequential_consistent memory_model;
            typedef cds::atomicity::item_counter item_counter;
            enum { padding = 64 };
        };
        typedef cds::container::OptimisticQueue < gc_type, int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( OptimisticQueue_DHP, move )
    {
        typedef cds::container::OptimisticQueue< gc_type, std::string > test_queue;

        test_queue q;
        test_string( q );
    }

    TEST_F( OptimisticQueue_DHP, move_item_counting )
    {
        struct traits : public cc::optimistic_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::OptimisticQueue< gc_type, std::string, traits > test_queue;

        test_queue q;
        test_string( q );
    }

} // namespace

