// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
