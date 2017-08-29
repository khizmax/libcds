/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

