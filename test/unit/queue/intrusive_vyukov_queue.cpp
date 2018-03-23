// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/intrusive/vyukov_mpmc_cycle_queue.h>
#include "test_intrusive_bounded_queue.h"

namespace {

    class IntrusiveVyukovQueue : public cds_test::intrusive_bounded_queue
    {
    public:
        struct disposer {
            void operator()( item * p )
            {
                ++p->nDisposeCount;
            }
        };
    };

    static const size_t c_Capacity = 64;
    static const size_t c_RealCapacity = c_Capacity;

    TEST_F( IntrusiveVyukovQueue, defaulted )
    {
        cds::intrusive::VyukovMPMCCycleQueue< item > q( c_Capacity );
        ASSERT_EQ( q.capacity(), c_RealCapacity );
        test( q );
    }

    TEST_F( IntrusiveVyukovQueue, disposer )
    {
        struct traits : public cds::intrusive::vyukov_queue::traits
        {
            typedef IntrusiveVyukovQueue::disposer disposer;
            typedef cds::atomicity::item_counter item_counter;
        };

        cds::intrusive::VyukovMPMCCycleQueue< item, traits > q( c_Capacity );
        ASSERT_EQ( q.capacity(), c_RealCapacity );
        test( q );
    }

    TEST_F( IntrusiveVyukovQueue, static_buffer )
    {
        struct traits : public cds::intrusive::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_static_buffer< int, c_Capacity > buffer;
            typedef IntrusiveVyukovQueue::disposer disposer;
        };

        cds::intrusive::VyukovMPMCCycleQueue< item, traits > q;
        ASSERT_EQ( q.capacity(), c_RealCapacity );
        test( q );
    }

    TEST_F( IntrusiveVyukovQueue, dynamic_buffer )
    {
        typedef typename cds::intrusive::vyukov_queue::make_traits<
            cds::opt::buffer< cds::opt::v::uninitialized_dynamic_buffer< int >>
            ,cds::opt::item_counter< cds::atomicity::item_counter >
            ,cds::opt::back_off< cds::backoff::pause >
            ,cds::intrusive::opt::disposer< disposer >
        >::type traits;

        cds::intrusive::VyukovMPMCCycleQueue< item, traits > q( c_Capacity );
        ASSERT_EQ( q.capacity(), c_RealCapacity );
        test( q );
    }

    TEST_F( IntrusiveVyukovQueue, padding )
    {
        struct traits : public cds::intrusive::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_static_buffer< int, c_Capacity > buffer;
            typedef IntrusiveVyukovQueue::disposer disposer;
            enum { padding = 16 | cds::opt::padding_tiny_data_only };
        };

        cds::intrusive::VyukovMPMCCycleQueue< item, traits > q;
        ASSERT_EQ( q.capacity(), c_RealCapacity );
        test( q );
    }


} // namespace
