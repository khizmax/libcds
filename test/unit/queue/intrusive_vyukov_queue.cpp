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
