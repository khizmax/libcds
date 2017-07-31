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
                size_t const nPushCount = s_nPushCount;

                for ( size_t i = 0; i < nPushCount; ++i ) {
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
            }

        public:
            Queue&              m_Queue;
            size_t              m_nPushFailed = 0;
            size_t              m_nPushed = 0;
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
            size_t              m_nPopFrontFailed = 0;

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
                while ( true ) {
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
            test_queue( q );
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

#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue( s_nBufferSize ); \
        test( queue ); \
    }

    CDSSTRESS_WeakRingBuffer_void( spsc_buffer )

#undef CDSSTRESS_Queue_F

} // namespace
