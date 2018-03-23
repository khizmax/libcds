// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "queue_type.h"

#include <vector>
#include <algorithm>
#include <type_traits>
#include <cmath>

// Single producer/single consumer buffer push/pop test
namespace {

    static size_t s_nBufferSize = 1024*1024;
    static size_t s_nPushCount = 1000000;

    static std::atomic<size_t> s_nProducerDone( 0 );

    class spsc_buffer: public cds_test::stress_fixture
    {
    protected:
        typedef size_t value_type;

        enum {
            producer_thread,
            consumer_thread
        };

        class empty_functor
        {
        public:
            void operator()()
            {}

            double result()
            {
                return 0.0;
            }
        };

        class payload_functor
        {
        public:
            void operator()()
            {
                std::random_device rd;
                std::mt19937 gen( rd());
                std::uniform_int_distribution<unsigned> dis( 0, 64 * 1024* 1024 );
                quad_sum += std::sqrt( static_cast<double>( dis(gen)));
            }

            double result()
            {
                return quad_sum;
            }

        private:
            double quad_sum = 0.0;
        };

        template <class Queue, class Payload = empty_functor>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            typedef Payload payload_type;

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
                size_t const nPushCount = s_nPushCount;
                payload_type func;

                for ( size_t i = 0; i < nPushCount; ++i ) {
                    func();
                    size_t len = rand( 1024 ) + 64;
                    void* buf = m_Queue.back( len );
                    if ( buf ) {
                        memset( buf, len % 256, len );
                        m_Queue.push_back();
                        m_nPushed += len;
                    }
                    else
                        ++m_nPushFailed;
                }

                s_nProducerDone.fetch_add( 1 );
                m_PayloadResult = func.result();
            }

        public:
            Queue&              m_Queue;
            size_t              m_nPushFailed = 0;
            size_t              m_nPushed = 0;

            double              m_PayloadResult = 0.0;
        };

        template <class Queue, class Payload = empty_functor>
        class Consumer: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            typedef Payload payload_type;

        public:
            Queue&              m_Queue;
            size_t              m_nPopEmpty = 0;
            size_t              m_nPopped = 0;
            size_t              m_nBadValue = 0;
            size_t              m_nPopFrontFailed = 0;

            double              m_PayloadResult = 0.0;

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
                payload_type func;

                while ( true ) {
                    func();

                    auto buf = m_Queue.front();
                    if ( buf.first ) {
                        m_nPopped += buf.second;

                        uint8_t val = static_cast<uint8_t>( buf.second % 256 );
                        uint8_t const* p = reinterpret_cast<uint8_t*>( buf.first );
                        for ( uint8_t const* pEnd = p + buf.second; p < pEnd; ++p ) {
                            if ( *p != val ) {
                                ++m_nBadValue;
                                break;
                            }
                        }

                        if ( !m_Queue.pop_front())
                            ++m_nPopFrontFailed;
                    }
                    else {
                        ++m_nPopEmpty;
                        if ( s_nProducerDone.load() != 0 ) {
                            if ( m_Queue.empty())
                                break;
                        }
                    }
                }

                m_PayloadResult = func.result();
            }
        };

    protected:
        size_t m_nThreadPushCount;

    protected:
        template <class ProducerPayload, class ConsumerPayload, class Queue >
        void test_queue( Queue& q )
        {
            cds_test::thread_pool& pool = get_pool();
            auto producer = new Producer<Queue, ProducerPayload>( pool, q );
            auto consumer = new Consumer<Queue, ConsumerPayload>( pool, q );

            pool.add( producer, 1 );
            pool.add( consumer, 1 );

            s_nProducerDone.store( 0 );

            propout() << std::make_pair( "buffer_size", s_nBufferSize )
                      << std::make_pair( "push_count", s_nPushCount );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            // analyze result
            EXPECT_EQ( consumer->m_nBadValue, 0u );
            EXPECT_EQ( consumer->m_nPopFrontFailed, 0u );
            EXPECT_EQ( consumer->m_nPopped, producer->m_nPushed );

            propout()
                << std::make_pair( "producer_push_length", producer->m_nPushed )
                << std::make_pair( "producer_push_failed", producer->m_nPushFailed )
                << std::make_pair( "consumer_pop_length", consumer->m_nPopped )
                << std::make_pair( "consumer_pop_empty", consumer->m_nPopEmpty )
                << std::make_pair( "consumer_bad_value", consumer->m_nBadValue )
                << std::make_pair( "consumer_pop_front_failed", consumer->m_nPopFrontFailed );
        }

        template <class Queue>
        void test( Queue& q )
        {
            test_queue<empty_functor, empty_functor>( q );
            propout() << q.statistics();
        }

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "spsc_buffer" );

            s_nBufferSize = cfg.get_size_t( "BufferSize", s_nBufferSize );
            s_nPushCount = cfg.get_size_t( "PushCount", s_nPushCount );

            if ( s_nBufferSize < 1024 * 64 )
                s_nBufferSize = 1024 * 64;
            if ( s_nPushCount == 0u )
                s_nPushCount = 1024;
        }
    };

    class spsc_buffer_slow_producer: public spsc_buffer
    {
    public:
        template <class Queue>
        void test( Queue& q )
        {
            test_queue<payload_functor, empty_functor>( q );
            propout() << q.statistics();
        }
    };

    class spsc_buffer_slow_consumer: public spsc_buffer
    {
    public:
        template <class Queue>
        void test( Queue& q )
        {
            test_queue<empty_functor, payload_functor>( q );
            propout() << q.statistics();
        }
    };

#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue( s_nBufferSize ); \
        test( queue ); \
    }

    CDSSTRESS_WeakRingBuffer_void( spsc_buffer )
    CDSSTRESS_WeakRingBuffer_void( spsc_buffer_slow_producer )
    CDSSTRESS_WeakRingBuffer_void( spsc_buffer_slow_consumer )

#undef CDSSTRESS_Queue_F

} // namespace
