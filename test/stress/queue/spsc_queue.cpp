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

#include "queue_type.h"

#include <vector>
#include <algorithm>
#include <type_traits>

// Single producer/single consumer queue push/pop test
namespace {

    static size_t s_nQueueSize = 1024;
    static size_t s_nPassCount = 1024;

    static std::atomic<size_t> s_nProducerDone( 0 );

    class spsc_queue: public cds_test::stress_fixture
    {
    protected:
       typedef size_t value_type;

        enum {
            producer_thread,
            consumer_thread
        };

        template <class Queue>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Producer( cds_test::thread_pool& pool, Queue& queue )
                : base_class( pool, producer_thread )
                , m_Queue( queue )
            {}

            Producer( Producer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
            {}

            virtual thread * clone()
            {
                return new Producer( *this );
            }

            virtual void test()
            {
                size_t const nPushCount = s_nQueueSize * s_nPassCount;
                m_nPushFailed = 0;

                for ( value_type v = 0; v < nPushCount; ++v ) {
                    if ( !m_Queue.push( v )) {
                        ++m_nPushFailed;
                        --v;
                    }
                }

                s_nProducerDone.fetch_add( 1 );
            }

        public:
            Queue&              m_Queue;
            size_t              m_nPushFailed = 0;
        };

        template <class Queue>
        class Consumer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Queue&              m_Queue;
            size_t              m_nPopEmpty = 0;
            size_t              m_nPopped = 0;
            size_t              m_nBadValue = 0;

        public:
            Consumer( cds_test::thread_pool& pool, Queue& queue )
                : base_class( pool, consumer_thread )
                , m_Queue( queue )
            {}
            Consumer( Consumer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
            {}

            virtual thread * clone()
            {
                return new Consumer( *this );
            }

            virtual void test()
            {
                value_type v;
                value_type prev = 0 - 1;

                while ( true ) {
                    if ( m_Queue.pop( v )) {
                        ++m_nPopped;
                        if ( v != prev + 1 )
                            ++m_nBadValue;
                        prev = v;
                    }
                    else {
                        ++m_nPopEmpty;

                        if ( s_nProducerDone.load() != 0 ) {
                            if ( m_Queue.empty())
                                break;
                        }
                    }
                }
            }
        };

    protected:
        size_t m_nThreadPushCount;

    protected:
        template <class Queue>
        void test_queue( Queue& q )
        {
            cds_test::thread_pool& pool = get_pool();
            auto producer = new Producer<Queue>( pool, q );
            auto consumer = new Consumer<Queue>( pool, q );

            pool.add( producer, 1 );
            pool.add( consumer, 1 );

            s_nProducerDone.store( 0 );

            propout() << std::make_pair( "queue_size", s_nQueueSize )
                      << std::make_pair( "pass_count", s_nPassCount );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            // analyze result
            EXPECT_EQ( consumer->m_nBadValue, 0u );
            EXPECT_EQ( consumer->m_nPopped, s_nQueueSize * s_nPassCount );

            propout()
                << std::make_pair( "producer_push_count", s_nQueueSize * s_nPassCount )
                << std::make_pair( "producer_push_failed", producer->m_nPushFailed )
                << std::make_pair( "consumer_pop_count", consumer->m_nPopped )
                << std::make_pair( "consumer_pop_empty", consumer->m_nPopEmpty )
                << std::make_pair( "consumer_bad_value", consumer->m_nBadValue );
        }

        template <class Queue>
        void test( Queue& q )
        {
            test_queue( q );
            propout() << q.statistics();
        }

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "spsc_queue" );

            s_nQueueSize = cfg.get_size_t( "QueueSize", s_nQueueSize );
            s_nPassCount = cfg.get_size_t( "PassCount", s_nPassCount );

            if ( s_nQueueSize == 0u )
                s_nQueueSize = 1024;
            if ( s_nPassCount == 0u )
                s_nPassCount = 1024;
        }
    };

    //CDSSTRESS_MSQueue( spsc_queue )
    //CDSSTRESS_MoirQueue( spsc_queue )
    //CDSSTRESS_BasketQueue( spsc_queue )
    //CDSSTRESS_OptimsticQueue( spsc_queue )
    //CDSSTRESS_FCQueue( spsc_queue )
    //CDSSTRESS_FCDeque( spsc_queue )
    //CDSSTRESS_RWQueue( spsc_queue )
    //CDSSTRESS_StdQueue( spsc_queue )

#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue( s_nQueueSize ); \
        test( queue ); \
    }

    CDSSTRESS_WeakRingBuffer( spsc_queue )
    CDSSTRESS_VyukovQueue( spsc_queue )
    CDSSTRESS_VyukovSingleConsumerQueue( spsc_queue )

#undef CDSSTRESS_Queue_F

} // namespace
