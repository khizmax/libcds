// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_bounded_queue.h"

#include <cds/container/vyukov_mpmc_cycle_queue.h>

namespace {
    namespace cc = cds::container;

    class VyukovMPMCCycleQueue: public cds_test::bounded_queue
    {
    public:
        template <typename Queue>
        void test_single_consumer( Queue& q )
        {
            typedef typename Queue::value_type value_type;

            const size_t nSize = q.capacity();

            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            // enqueue/dequeue
            for ( unsigned pass = 0; pass < 3; ++pass ) {
                for ( size_t i = 0; i < nSize; ++i ) {
                    ASSERT_TRUE( q.enqueue( static_cast<value_type>( i )));
                    ASSERT_CONTAINER_SIZE( q, i + 1 );
                }
                ASSERT_FALSE( q.empty());
                ASSERT_CONTAINER_SIZE( q, nSize );
                ASSERT_FALSE( q.enqueue( static_cast<value_type>( nSize ) * 2 ));

                for ( size_t i = 0; i < nSize; ++i ) {
                    value_type* fr = q.front();
                    ASSERT_TRUE( fr != nullptr );
                    ASSERT_EQ( *fr, static_cast<value_type>( i ));
                    ASSERT_TRUE( q.pop_front());
                    ASSERT_CONTAINER_SIZE( q, nSize - i - 1 );
                }
                ASSERT_TRUE( q.empty());
                ASSERT_CONTAINER_SIZE( q, 0 );

                ASSERT_TRUE( q.front() == nullptr );
                ASSERT_FALSE( q.pop_front());
            }
        }
    };

    TEST_F( VyukovMPMCCycleQueue, defaulted )
    {
        typedef cds::container::VyukovMPMCCycleQueue< int > test_queue;

        test_queue q( 128 );
        test(q);
    }

    TEST_F( VyukovMPMCCycleQueue, stat )
    {
        struct traits: public cds::container::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_static_buffer<int, 128> buffer;
        };
        typedef cds::container::VyukovMPMCCycleQueue< int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( VyukovMPMCCycleQueue, stat_item_counting )
    {

        typedef cds::container::VyukovMPMCCycleQueue< int,
            cds::container::vyukov_queue::make_traits<
                cds::opt::buffer< cds::opt::v::uninitialized_static_buffer<int, 128>>
                , cds::opt::item_counter< cds::atomicity::item_counter>
            >::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( VyukovMPMCCycleQueue, dynamic )
    {
        struct traits : public cds::container::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer<int> buffer;
        };
        typedef cds::container::VyukovMPMCCycleQueue< int, traits > test_queue;

        test_queue q( 128 );
        test( q );
    }

    TEST_F( VyukovMPMCCycleQueue, dynamic_item_counting )
    {
        typedef cds::container::VyukovMPMCCycleQueue< int,
            cds::container::vyukov_queue::make_traits<
                cds::opt::buffer< cds::opt::v::uninitialized_dynamic_buffer<int>>
                , cds::opt::item_counter< cds::atomicity::item_counter>
            >::type
        > test_queue;

        test_queue q( 128 );
        test( q );
    }

    TEST_F( VyukovMPMCCycleQueue, dynamic_padding )
    {
        struct traits : public cds::container::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer<int> buffer;
            enum { padding = 64 };
        };
        typedef cds::container::VyukovMPMCCycleQueue< int, traits > test_queue;

        test_queue q( 128 );
        test( q );
    }

    TEST_F( VyukovMPMCCycleQueue, move )
    {
        typedef cds::container::VyukovMPMCCycleQueue< std::string > test_queue;

        test_queue q( 128 );
        test_string( q );
    }

    TEST_F( VyukovMPMCCycleQueue, move_item_counting )
    {
        struct traits : public cds::container::vyukov_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::VyukovMPMCCycleQueue< std::string, traits > test_queue;

        test_queue q( 128 );
        test_string( q );
    }

    TEST_F( VyukovMPMCCycleQueue, single_consumer )
    {
        struct traits: public cds::container::vyukov_queue::traits
        {
            enum: bool { single_consumer = true };
        };
        typedef cds::container::VyukovMPMCCycleQueue< int, traits > test_queue;

        test_queue q( 128 );
        test( q );
        test_single_consumer( q );
    }

} // namespace

