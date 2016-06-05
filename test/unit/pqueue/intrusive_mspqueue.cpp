/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#include "test_data.h"
#include <cds/intrusive/mspriority_queue.h>

namespace {

    struct disposer {
        size_t   m_nCallCount;

        disposer()
            : m_nCallCount( 0 )
        {}

        template <typename T>
        void operator()( T& )
        {
            ++m_nCallCount;
        }
    };

    class IntrusiveMSPQueue : public cds_test::PQueueTest
    {
        typedef cds_test::PQueueTest base_class;
    protected:
        template <class PQueue>
        void test( PQueue& pq )
        {
            data_array<value_type> arr( pq.capacity() );
            value_type * pFirst = arr.begin();
            value_type * pLast = arr.end();

            ASSERT_TRUE( pq.empty() );
            ASSERT_FALSE( pq.full() );
            ASSERT_EQ( pq.size(), 0 );
            ASSERT_EQ( pq.capacity(), base_class::c_nCapacity - 1 );

            size_t nSize = 0;

            // Push test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                ASSERT_TRUE( pq.push( *p ) );
                ASSERT_FALSE( pq.empty() );
                ASSERT_EQ( pq.size(), ++nSize );
            }

            ASSERT_TRUE( pq.full() );
            ASSERT_EQ( pq.size(), pq.capacity() );

            // The queue is full
            {
                value_type k( base_class::c_nMinValue + key_type( base_class::c_nCapacity ));
                ASSERT_FALSE( pq.push( k ) );
                ASSERT_TRUE( pq.full() );
                ASSERT_EQ( pq.size(), pq.capacity() );
            }

            // Pop test
            key_type nPrev = base_class::c_nMinValue + key_type( pq.capacity() ) - 1;
            value_type * p = pq.pop();
            ASSERT_TRUE( p != nullptr );
            EXPECT_EQ( p->k, nPrev );

            ASSERT_EQ( pq.size(), pq.capacity() - 1 );
            ASSERT_FALSE( pq.full() );
            ASSERT_FALSE( pq.empty() );

            nSize = pq.size();
            while ( pq.size() > 1 ) {
                p = pq.pop();
                ASSERT_TRUE( p != nullptr );
                EXPECT_EQ( p->k, nPrev - 1 );
                nPrev = p->k;
                --nSize;
                ASSERT_EQ( pq.size(), nSize );
            }

            ASSERT_FALSE( pq.full() );
            ASSERT_FALSE( pq.empty() );
            ASSERT_EQ( pq.size(), 1 );

            p = pq.pop();
            ASSERT_TRUE( p != nullptr );
            EXPECT_EQ( p->k, base_class::c_nMinValue );

            ASSERT_FALSE( pq.full() );
            ASSERT_TRUE( pq.empty() );
            ASSERT_EQ( pq.size(), 0 );

            // Clear test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                ASSERT_TRUE( pq.push( *p ) );
            }
            EXPECT_FALSE( pq.empty() );
            EXPECT_TRUE( pq.full() );
            EXPECT_EQ( pq.size(), pq.capacity() );
            pq.clear();
            EXPECT_TRUE( pq.empty() );
            EXPECT_FALSE( pq.full() );
            EXPECT_EQ( pq.size(), 0 );

            // clear_with test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                ASSERT_TRUE( pq.push( *p ) );
            }
            ASSERT_FALSE( pq.empty() );
            ASSERT_TRUE( pq.full() );
            ASSERT_EQ( pq.size(), pq.capacity() );

            {
                disposer disp;
                pq.clear_with( std::ref( disp ) );
                ASSERT_TRUE( pq.empty() );
                ASSERT_FALSE( pq.full() );
                ASSERT_EQ( pq.size(), 0 );
                ASSERT_EQ( disp.m_nCallCount, pq.capacity() );
            }
        }
    };

    typedef cds::opt::v::initialized_dynamic_buffer< char > dyn_buffer_type;
    typedef cds::opt::v::initialized_static_buffer< char, IntrusiveMSPQueue::c_nCapacity > static_buffer_type;

    TEST_F( IntrusiveMSPQueue, dynamic )
    {
        struct traits : public cds::intrusive::mspriority_queue::traits
        {
            typedef dyn_buffer_type buffer;
        };
        typedef cds::intrusive::MSPriorityQueue< value_type, traits > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( IntrusiveMSPQueue, dynamic_cmp )
    {
        struct traits : public cds::intrusive::mspriority_queue::traits
        {
            typedef dyn_buffer_type buffer;
            typedef IntrusiveMSPQueue::compare compare;
        };
        typedef cds::intrusive::MSPriorityQueue< value_type, traits > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( IntrusiveMSPQueue, dynamic_less )
    {
        typedef cds::intrusive::MSPriorityQueue< value_type,
            cds::intrusive::mspriority_queue::make_traits<
                cds::opt::buffer< dyn_buffer_type >
                ,cds::opt::less< std::less<value_type> >
            >::type
        > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( IntrusiveMSPQueue, dynamic_cmp_less )
    {
        typedef cds::intrusive::MSPriorityQueue< value_type,
            cds::intrusive::mspriority_queue::make_traits<
                cds::opt::buffer< dyn_buffer_type >
                ,cds::opt::less< std::less<value_type> >
                ,cds::opt::compare< IntrusiveMSPQueue::compare >
            >::type
        > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( IntrusiveMSPQueue, dynamic_mutex )
    {
        struct traits : public cds::intrusive::mspriority_queue::traits
        {
            typedef dyn_buffer_type buffer;
            typedef IntrusiveMSPQueue::compare compare;
            typedef std::mutex lock_type;
        };
        typedef cds::intrusive::MSPriorityQueue< value_type, traits > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( IntrusiveMSPQueue, stat )
    {
        struct traits : public cds::intrusive::mspriority_queue::traits
        {
            typedef static_buffer_type buffer;
        };
        typedef cds::intrusive::MSPriorityQueue< value_type, traits > pqueue;

        std::unique_ptr<pqueue> pq( new pqueue(0));
        test( *pq );
    }

    TEST_F( IntrusiveMSPQueue, stat_cmp )
    {
        typedef cds::intrusive::MSPriorityQueue< value_type,
            cds::intrusive::mspriority_queue::make_traits<
                cds::opt::buffer< static_buffer_type >
                ,cds::opt::compare< compare >
            >::type
        > pqueue;

        std::unique_ptr<pqueue> pq( new pqueue( 0 ) );
        test( *pq );
    }

    TEST_F( IntrusiveMSPQueue, stat_less )
    {
        typedef cds::intrusive::MSPriorityQueue< value_type,
            cds::intrusive::mspriority_queue::make_traits<
                cds::opt::buffer< static_buffer_type >
                ,cds::opt::less< less >
            >::type
        > pqueue;

        std::unique_ptr<pqueue> pq( new pqueue( 0 ) );
        test( *pq );
    }

    TEST_F( IntrusiveMSPQueue, stat_mutex )
    {
        struct traits : public cds::intrusive::mspriority_queue::traits
        {
            typedef static_buffer_type buffer;
            typedef IntrusiveMSPQueue::compare compare;
            typedef std::mutex lock_type;
        };
        typedef cds::intrusive::MSPriorityQueue< value_type, traits > pqueue;

        std::unique_ptr<pqueue> pq( new pqueue( 0 ) );
        test( *pq );
    }

} // namespace
