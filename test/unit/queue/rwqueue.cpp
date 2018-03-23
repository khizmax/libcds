// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_generic_queue.h"

#include <cds/container/rwqueue.h>

namespace {
    namespace cc = cds::container;

    class RWQueue: public cds_test::generic_queue
    {};

    TEST_F( RWQueue, defaulted )
    {
        typedef cds::container::RWQueue< int > test_queue;

        test_queue q;
        test(q);
    }

    TEST_F( RWQueue, item_counting )
    {
        typedef cds::container::RWQueue< int,
            typename cds::container::rwqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( RWQueue, mutex )
    {
        struct traits : public cds::container::rwqueue::traits
        {
            typedef std::mutex lock_type;
        };
        typedef cds::container::RWQueue< int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( RWQueue, padding )
    {
        struct traits : public cds::container::rwqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            enum { padding = 64 };
        };
        typedef cds::container::RWQueue< int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( RWQueue, move )
    {
        typedef cds::container::RWQueue< std::string > test_queue;

        test_queue q;
        test_string( q );
    }

    TEST_F( RWQueue, move_item_counting )
    {
        struct traits : public cc::rwqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::RWQueue< std::string, traits > test_queue;

        test_queue q;
        test_string( q );
    }

} // namespace

