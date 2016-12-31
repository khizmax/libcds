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

// Multi-threaded queue push/pop test
namespace {

    static size_t s_nConsumerThreadCount = 4;
    static size_t s_nProducerThreadCount = 4;
    static size_t s_nQueueSize = 4000000;

    static std::atomic<size_t> s_nProducerDone( 0 );

    class queue_push_pop: public cds_test::stress_fixture
    {
    protected:
        struct value_type
        {
            size_t      nNo;
            size_t      nWriterNo;
        };

        enum {
            producer_thread,
            consumer_thread
        };

        template <class Queue>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Producer( cds_test::thread_pool& pool, Queue& queue, size_t nPushCount )
                : base_class( pool, producer_thread )
                , m_Queue( queue )
                , m_nPushFailed( 0 )
                , m_nPushCount( nPushCount )
            {}

            Producer( Producer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
                , m_nPushFailed( 0 )
                , m_nPushCount( src.m_nPushCount )
            {}

            virtual thread * clone()
            {
                return new Producer( *this );
            }

            virtual void test()
            {
                size_t const nPushCount = m_nPushCount;
                value_type v;
                v.nWriterNo = id();
                v.nNo = 0;
                m_nPushFailed = 0;

                while ( v.nNo < nPushCount ) {
                    if ( m_Queue.push( v ))
                        ++v.nNo;
                    else
                        ++m_nPushFailed;
                }

                s_nProducerDone.fetch_add( 1 );
            }

        public:
            Queue&              m_Queue;
            size_t              m_nPushFailed;
            size_t const        m_nPushCount;
        };

        template <class Queue>
        class Consumer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Queue&              m_Queue;
            size_t const        m_nPushPerProducer;
            size_t              m_nPopEmpty;
            size_t              m_nPopped;
            size_t              m_nBadWriter;

            typedef std::vector<size_t> popped_data;
            typedef std::vector<size_t>::iterator       data_iterator;
            typedef std::vector<size_t>::const_iterator const_data_iterator;

            std::vector<popped_data>        m_WriterData;

        private:
            void initPoppedData()
            {
                const size_t nProducerCount = s_nProducerThreadCount;
                m_WriterData.resize( nProducerCount );
                for ( size_t i = 0; i < nProducerCount; ++i )
                    m_WriterData[i].reserve( m_nPushPerProducer );
            }

        public:
            Consumer( cds_test::thread_pool& pool, Queue& queue, size_t nPushPerProducer )
                : base_class( pool, consumer_thread )
                , m_Queue( queue )
                , m_nPushPerProducer( nPushPerProducer )
                , m_nPopEmpty( 0 )
                , m_nPopped( 0 )
                , m_nBadWriter( 0 )
            {
                initPoppedData();
            }
            Consumer( Consumer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
                , m_nPushPerProducer( src.m_nPushPerProducer )
                , m_nPopEmpty( 0 )
                , m_nPopped( 0 )
                , m_nBadWriter( 0 )
            {
                initPoppedData();
            }

            virtual thread * clone()
            {
                return new Consumer( *this );
            }

            virtual void test()
            {
                m_nPopEmpty = 0;
                m_nPopped = 0;
                m_nBadWriter = 0;
                const size_t nTotalWriters = s_nProducerThreadCount;
                value_type v;
                while ( true ) {
                    if ( m_Queue.pop( v )) {
                        ++m_nPopped;
                        if ( v.nWriterNo < nTotalWriters )
                            m_WriterData[ v.nWriterNo ].push_back( v.nNo );
                        else
                            ++m_nBadWriter;
                    }
                    else {
                        ++m_nPopEmpty;

                        if ( s_nProducerDone.load() >= nTotalWriters ) {
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
        void analyze( Queue& q, size_t /*nLeftOffset*/ = 0, size_t nRightOffset = 0 )
        {
            cds_test::thread_pool& pool = get_pool();

            typedef Consumer<Queue> Consumer;
            typedef Producer<Queue> Producer;

            size_t nPostTestPops = 0;
            {
                value_type v;
                while ( q.pop( v ))
                    ++nPostTestPops;
            }

            size_t nTotalPops = 0;
            size_t nPopFalse = 0;
            size_t nPoppedItems = 0;
            size_t nPushFailed = 0;

            std::vector< Consumer * > arrConsumer;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get(i);
                if ( thr.type() == consumer_thread ) {
                    Consumer& consumer = static_cast<Consumer&>( thr );
                    nTotalPops += consumer.m_nPopped;
                    nPopFalse += consumer.m_nPopEmpty;
                    arrConsumer.push_back( &consumer );
                    EXPECT_EQ( consumer.m_nBadWriter, 0u ) << "consumer_thread_no " << i;

                    size_t nPopped = 0;
                    for ( size_t n = 0; n < s_nProducerThreadCount; ++n )
                        nPopped += consumer.m_WriterData[n].size();

                    nPoppedItems += nPopped;
                }
                else {
                    assert( thr.type() == producer_thread );

                    Producer& producer = static_cast<Producer&>( thr );
                    nPushFailed += producer.m_nPushFailed;
                    EXPECT_EQ( producer.m_nPushFailed, 0u ) << "producer_thread_no " << i;
                }
            }
            EXPECT_EQ( nTotalPops, nPoppedItems );

            EXPECT_EQ( nTotalPops + nPostTestPops, s_nQueueSize ) << "nTotalPops=" << nTotalPops << ", nPostTestPops=" << nPostTestPops;
            EXPECT_TRUE( q.empty());

            // Test consistency of popped sequence
            for ( size_t nWriter = 0; nWriter < s_nProducerThreadCount; ++nWriter ) {
                std::vector<size_t> arrData;
                arrData.reserve( m_nThreadPushCount );
                for ( size_t nReader = 0; nReader < arrConsumer.size(); ++nReader ) {
                    auto it = arrConsumer[nReader]->m_WriterData[nWriter].begin();
                    auto itEnd = arrConsumer[nReader]->m_WriterData[nWriter].end();
                    if ( it != itEnd ) {
                        auto itPrev = it;
                        for ( ++it; it != itEnd; ++it ) {
                            EXPECT_LT( *itPrev, *it + nRightOffset ) << "consumer=" << nReader << ", producer=" << nWriter;
                            itPrev = it;
                        }
                    }

                    for ( it = arrConsumer[nReader]->m_WriterData[nWriter].begin(); it != itEnd; ++it )
                        arrData.push_back( *it );
                }

                std::sort( arrData.begin(), arrData.end());
                for ( size_t i=1; i < arrData.size(); ++i ) {
                    EXPECT_EQ( arrData[i - 1] + 1, arrData[i] ) << "producer=" << nWriter;
                }

                EXPECT_EQ( arrData[0], 0u ) << "producer=" << nWriter;
                EXPECT_EQ( arrData[arrData.size() - 1], m_nThreadPushCount - 1 ) << "producer=" << nWriter;
            }
        }

        template <class Queue>
        void test_queue( Queue& q )
        {
            m_nThreadPushCount = s_nQueueSize / s_nProducerThreadCount;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new Producer<Queue>( pool, q, m_nThreadPushCount ), s_nProducerThreadCount );
            pool.add( new Consumer<Queue>( pool, q, m_nThreadPushCount ), s_nConsumerThreadCount );

            s_nProducerDone.store( 0 );
            s_nQueueSize = m_nThreadPushCount * s_nProducerThreadCount;

            propout() << std::make_pair( "producer_count", s_nProducerThreadCount )
                << std::make_pair( "consumer_count", s_nConsumerThreadCount )
                << std::make_pair( "push_count", s_nQueueSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );
        }

        template <class Queue>
        void test( Queue& q )
        {
            test_queue( q );
            analyze( q );
            propout() << q.statistics();
        }

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "queue_push_pop" );

            s_nConsumerThreadCount = cfg.get_size_t( "ConsumerCount", s_nConsumerThreadCount );
            s_nProducerThreadCount = cfg.get_size_t( "ProducerCount", s_nProducerThreadCount );
            s_nQueueSize = cfg.get_size_t( "QueueSize", s_nQueueSize );

            if ( s_nConsumerThreadCount == 0u )
                s_nConsumerThreadCount = 1;
            if ( s_nProducerThreadCount == 0u )
                s_nProducerThreadCount = 1;
            if ( s_nQueueSize == 0u )
                s_nQueueSize = 1000;
        }

        //static void TearDownTestCase();
    };

    CDSSTRESS_MSQueue( queue_push_pop )
    CDSSTRESS_MoirQueue( queue_push_pop )
    CDSSTRESS_BasketQueue( queue_push_pop )
    CDSSTRESS_OptimsticQueue( queue_push_pop )
    CDSSTRESS_FCQueue( queue_push_pop )
    CDSSTRESS_FCDeque( queue_push_pop )
    CDSSTRESS_RWQueue( queue_push_pop )
    CDSSTRESS_StdQueue( queue_push_pop )

#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F( test_fixture, type_name, level ) \
    TEST_F( test_fixture, type_name ) \
    { \
        if ( !check_detail_level( level )) return; \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue( s_nQueueSize ); \
        test( queue ); \
    }

    CDSSTRESS_VyukovQueue( queue_push_pop )

#undef CDSSTRESS_Queue_F


    // ********************************************************************
    // SegmentedQueue test

    class segmented_queue_push_pop
        : public queue_push_pop
        , public ::testing::WithParamInterface< size_t >
    {
        typedef queue_push_pop base_class;

    protected:

        template <typename Queue>
        void test()
        {
            size_t quasi_factor = GetParam();

            Queue q( quasi_factor );
            propout() << std::make_pair( "quasi_factor", quasi_factor );
            base_class::test_queue( q );
            analyze( q, quasi_factor * 2, quasi_factor );
            propout() << q.statistics();
        }

    public:
        static std::vector< size_t > get_test_parameters()
        {
            cds_test::config const& cfg = cds_test::stress_fixture::get_config( "queue_push_pop" );
            bool bIterative = cfg.get_bool( "SegmentedQueue_Iterate", false );
            size_t quasi_factor = cfg.get_size_t( "SegmentedQueue_SegmentSize", 256 );

            std::vector<size_t> args;
            if ( bIterative && quasi_factor > 4 ) {
                for ( size_t qf = 4; qf <= quasi_factor; qf *= 2 )
                    args.push_back( qf );
            }
            else {
                if ( quasi_factor > 2 )
                    args.push_back( quasi_factor );
                else
                    args.push_back( 2 );
            }

            return args;
        }
    };

#define CDSSTRESS_Queue_F( test_fixture, type_name, level ) \
    TEST_P( test_fixture, type_name ) \
    { \
        if ( !check_detail_level( level )) return; \
        typedef typename queue::Types<value_type>::type_name queue_type; \
        test< queue_type >(); \
    }

    CDSSTRESS_SegmentedQueue( segmented_queue_push_pop )

    INSTANTIATE_TEST_CASE_P( SQ,
        segmented_queue_push_pop,
        ::testing::ValuesIn( segmented_queue_push_pop::get_test_parameters()));

} // namespace
