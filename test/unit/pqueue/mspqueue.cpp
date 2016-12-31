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

#include "test_data.h"
#include <cds/container/mspriority_queue.h>

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

    class MSPQueue : public cds_test::PQueueTest
    {
        typedef cds_test::PQueueTest base_class;
    protected:
        template <class PQueue>
        void test( PQueue& pq )
        {
            data_array<value_type> arr( pq.capacity());
            value_type * pFirst = arr.begin();
            value_type * pLast = pFirst + pq.capacity();

            ASSERT_TRUE( pq.empty());
            ASSERT_EQ( pq.size(), 0u );
            ASSERT_EQ( pq.capacity(), size_t( base_class::c_nCapacity - 1 ));

            size_t nSize = 0;

            // Push test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                switch ( pq.size() & 3 ) {
                case 0:
                    ASSERT_TRUE( pq.push_with( [p]( value_type& dest ) { dest = *p; } ));
                    break;
                case 1:
                    ASSERT_TRUE( pq.emplace( p->k, p->v ));
                    break;
                case 2:
                    ASSERT_TRUE( pq.emplace( std::make_pair( p->k, p->v )));
                    break;
                default:
                    ASSERT_TRUE( pq.push( *p ));
                }
                ASSERT_TRUE( !pq.empty());
                ASSERT_TRUE( pq.size() == ++nSize );
            }

            ASSERT_TRUE( pq.full());
            ASSERT_EQ( pq.size(), pq.capacity());

            // The queue is full
            key_type k = base_class::c_nMinValue + key_type( base_class::c_nCapacity );
            ASSERT_TRUE( !pq.push( k ));
            ASSERT_TRUE( pq.full());
            ASSERT_EQ( pq.size(), pq.capacity());

            // Pop test
            key_type nPrev = base_class::c_nMinValue + key_type( pq.capacity()) - 1;
            value_type kv( 0 );
            key_type   key;
            ASSERT_TRUE( pq.pop( kv ));
            EXPECT_EQ( kv.k, nPrev );

            ASSERT_EQ( pq.size(), pq.capacity() - 1 );
            ASSERT_TRUE( !pq.full());
            ASSERT_TRUE( !pq.empty());

            nSize = pq.size();
            while ( pq.size() > 1 ) {
                if ( pq.size() & 1 ) {
                    ASSERT_TRUE( pq.pop( kv ));
                    EXPECT_EQ( kv.k, nPrev - 1 );
                    nPrev = kv.k;
                }
                else {
                    ASSERT_TRUE( pq.pop_with( [&key]( value_type& src ) { key = src.k;  } ));
                    EXPECT_EQ( key, nPrev - 1 );
                    nPrev = key;
                }

                --nSize;
                ASSERT_EQ( pq.size(), nSize );
            }

            ASSERT_TRUE( !pq.full());
            ASSERT_TRUE( !pq.empty());
            ASSERT_EQ( pq.size(), 1u );

            ASSERT_TRUE( pq.pop( kv ));
            EXPECT_EQ( kv.k, base_class::c_nMinValue );

            ASSERT_TRUE( !pq.full());
            ASSERT_TRUE( pq.empty());
            ASSERT_EQ( pq.size(), 0u );

            // Clear test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                ASSERT_TRUE( pq.push( *p ));
            }
            ASSERT_TRUE( !pq.empty());
            ASSERT_TRUE( pq.full());
            ASSERT_EQ( pq.size(), pq.capacity());
            pq.clear();
            ASSERT_TRUE( pq.empty());
            ASSERT_TRUE( !pq.full());
            ASSERT_EQ( pq.size(), 0u );

            // clear_with test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                ASSERT_TRUE( pq.push( *p ));
            }
            ASSERT_TRUE( !pq.empty());
            ASSERT_TRUE( pq.full());
            ASSERT_EQ( pq.size(), pq.capacity());

            {
                disposer disp;
                pq.clear_with( std::ref( disp ));
                ASSERT_TRUE( pq.empty());
                ASSERT_TRUE( !pq.full());
                ASSERT_EQ( pq.size(), 0u );
                ASSERT_EQ( disp.m_nCallCount, pq.capacity());
            }
        }
    };

    typedef cds::opt::v::initialized_dynamic_buffer< char > dyn_buffer_type;
    typedef cds::opt::v::initialized_static_buffer< char, MSPQueue::c_nCapacity > static_buffer_type;

    TEST_F( MSPQueue, dynamic )
    {
        typedef cds::container::MSPriorityQueue< MSPQueue::value_type,
            cds::container::mspriority_queue::make_traits<
                cds::opt::buffer< dyn_buffer_type >
            >::type
        > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( MSPQueue, dynamic_cmp )
    {
        typedef cds::container::MSPriorityQueue< value_type,
            cds::container::mspriority_queue::make_traits<
                cds::opt::buffer< dyn_buffer_type >
                , cds::opt::compare< compare >
            >::type
        > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( MSPQueue, dynamic_less )
    {
        typedef cds::container::MSPriorityQueue< value_type,
            cds::container::mspriority_queue::make_traits<
                cds::opt::buffer< dyn_buffer_type >
                ,cds::opt::less< less >
            >::type
        > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }
    TEST_F( MSPQueue, dynamic_cmp_less )
    {
        struct pqueue_traits : public cds::container::mspriority_queue::traits
        {
            typedef dyn_buffer_type buffer;
            typedef MSPQueue::less less;
            typedef MSPQueue::compare compare;
        };
        typedef cds::container::MSPriorityQueue< value_type, pqueue_traits > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( MSPQueue, dynamic_mutex )
    {
        typedef cds::container::MSPriorityQueue< value_type,
            cds::container::mspriority_queue::make_traits<
                cds::opt::buffer< dyn_buffer_type >
                ,cds::opt::compare< compare >
                ,cds::opt::lock_type<std::mutex>
            >::type
        > pqueue;

        pqueue pq( c_nCapacity );
        test( pq );
    }

    TEST_F( MSPQueue, stat )
    {
        typedef cds::container::MSPriorityQueue< MSPQueue::value_type,
            cds::container::mspriority_queue::make_traits<
            cds::opt::buffer< static_buffer_type >
            >::type
        > pqueue;

        std::unique_ptr< pqueue > pq( new pqueue(0));
        test( *pq );
    }

    TEST_F( MSPQueue, stat_cmp )
    {
        typedef cds::container::MSPriorityQueue< value_type,
            cds::container::mspriority_queue::make_traits<
                cds::opt::buffer< static_buffer_type >
                ,cds::opt::compare< compare >
            >::type
        > pqueue;

        std::unique_ptr< pqueue > pq( new pqueue(0));
        test( *pq );
    }

    TEST_F( MSPQueue, stat_less )
    {
        typedef cds::container::MSPriorityQueue< value_type,
            cds::container::mspriority_queue::make_traits<
                cds::opt::buffer< static_buffer_type >
                ,cds::opt::less< less >
            >::type
        > pqueue;

        std::unique_ptr< pqueue > pq( new pqueue(0));
        test( *pq );
    }

    TEST_F( MSPQueue, stat_mutex )
    {
        typedef cds::container::MSPriorityQueue< value_type,
            cds::container::mspriority_queue::make_traits<
                cds::opt::buffer< static_buffer_type >
                ,cds::opt::less< less >
                ,cds::opt::lock_type<std::mutex>
            >::type
        > pqueue;

        std::unique_ptr< pqueue > pq( new pqueue(0));
        test( *pq );
    }

} // namespace
