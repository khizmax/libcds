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

#include "intrusive_queue_type.h"
#include <vector>
#include <algorithm>

// Multi-threaded random queue test
namespace {

    static size_t s_nReaderThreadCount = 4;
    static size_t s_nWriterThreadCount = 4;
    static size_t s_nQueueSize = 4000000;

    static unsigned int s_nFCPassCount = 8;
    static unsigned int s_nFCCompactFactor = 64;

    static atomics::atomic< size_t > s_nProducerCount(0);
    static size_t s_nThreadPushCount;
    static CDS_CONSTEXPR const size_t c_nBadConsumer = 0xbadc0ffe;

    struct empty {};

    template <typename Base = empty >
    struct value_type: public Base
    {
        size_t      nNo;
        size_t      nWriterNo;
        size_t      nConsumer;
    };

    class intrusive_queue_push_pop: public cds_test::stress_fixture
    {
        typedef cds_test::stress_fixture base_class;

    protected:
        enum {
            producer_thread,
            consumer_thread
        };

        template <class Queue>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Producer( cds_test::thread_pool& pool, Queue& q )
                : base_class( pool, producer_thread )
                , m_Queue( q )
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
                size_t i = 0;
                for ( typename Queue::value_type * p = m_pStart; p < m_pEnd; ) {
                    p->nNo = i;
                    p->nWriterNo = id();
                    CDS_TSAN_ANNOTATE_HAPPENS_BEFORE( &p->nWriterNo );
                    if ( m_Queue.push( *p )) {
                        ++p;
                        ++i;
                    }
                    else
                        ++m_nPushFailed;
                }
                s_nProducerCount.fetch_sub( 1, atomics::memory_order_release );
            }

        public:
            Queue&              m_Queue;
            size_t              m_nPushFailed = 0;

            // Interval in m_arrValue
            typename Queue::value_type *       m_pStart;
            typename Queue::value_type *       m_pEnd;
        };

        template <class Queue>
        class Consumer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Queue&              m_Queue;
            size_t              m_nPopEmpty = 0;
            size_t              m_nPopped = 0;
            size_t              m_nBadWriter = 0;

            typedef std::vector<size_t> TPoppedData;
            typedef std::vector<size_t>::iterator       data_iterator;
            typedef std::vector<size_t>::const_iterator const_data_iterator;

            std::vector<TPoppedData>        m_WriterData;

        private:
            void initPoppedData()
            {
                const size_t nWriterCount = s_nWriterThreadCount;
                const size_t nWriterPushCount = s_nThreadPushCount;
                m_WriterData.resize( nWriterCount );
                for ( size_t i = 0; i < nWriterCount; ++i )
                    m_WriterData[i].reserve( nWriterPushCount );
            }

        public:
            Consumer( cds_test::thread_pool& pool, Queue& q )
                : base_class( pool, consumer_thread )
                , m_Queue( q )
            {
                initPoppedData();
            }
            Consumer( Consumer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
            {
                initPoppedData();
            }

            virtual thread * clone()
            {
                return new Consumer( *this );
            }

            virtual void test()
            {
                size_t const nTotalWriters = s_nWriterThreadCount;

                while ( true ) {
                    typename Queue::value_type * p = m_Queue.pop();
                    if ( p ) {
                        p->nConsumer = id();
                        ++m_nPopped;
                        CDS_TSAN_ANNOTATE_HAPPENS_AFTER( &p->nWriterNo );
                        if ( p->nWriterNo < nTotalWriters )
                            m_WriterData[ p->nWriterNo ].push_back( p->nNo );
                        else
                            ++m_nBadWriter;
                    }
                    else {
                        ++m_nPopEmpty;
                        if ( s_nProducerCount.load( atomics::memory_order_acquire ) == 0 && m_Queue.empty())
                            break;
                    }
                }
            }
        };

        template <typename T>
        class value_array
        {
            std::unique_ptr<T[]> m_pArr;
        public:
            value_array( size_t nSize )
                : m_pArr( new T[nSize] )
            {}

            T * get() const { return m_pArr.get(); }
        };

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "queue_random" );

            s_nReaderThreadCount = cfg.get_size_t( "ReaderCount", s_nReaderThreadCount );
            s_nWriterThreadCount = cfg.get_size_t( "WriterCount", s_nWriterThreadCount );
            s_nQueueSize = cfg.get_size_t( "QueueSize", s_nQueueSize );

            s_nFCPassCount = cfg.get_uint( "FCPassCount", s_nFCPassCount );
            s_nFCCompactFactor = cfg.get_uint( "FCCompactFactor", s_nFCCompactFactor );

            if ( s_nReaderThreadCount == 0u )
                s_nReaderThreadCount = 1;
            if ( s_nWriterThreadCount == 0u )
                s_nWriterThreadCount = 1;
            if ( s_nQueueSize == 0u )
                s_nQueueSize = 1000;
        }

        //static void TearDownTestCase();

    protected:
        template <class Queue>
        void analyze( Queue& testQueue, size_t /*nLeftOffset*/, size_t nRightOffset )
        {
            typedef Consumer<Queue> Reader;
            typedef typename Reader::const_data_iterator    ReaderIterator;

            size_t nPostTestPops = 0;
            while ( testQueue.pop())
                ++nPostTestPops;

            size_t nTotalPops = 0;
            size_t nPopFalse = 0;
            size_t nPoppedItems = 0;
            size_t nPushFailed = 0;

            std::vector< Reader * > arrReaders;

            cds_test::thread_pool& pool = get_pool();
            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                if ( thr.type() == consumer_thread ) {
                    Consumer<Queue>& consumer = static_cast<Consumer<Queue>&>( thr );
                    nTotalPops += consumer.m_nPopped;
                    nPopFalse += consumer.m_nPopEmpty;
                    arrReaders.push_back( &consumer );
                    EXPECT_EQ( consumer.m_nBadWriter, 0u ) << "consumer " << (i - s_nWriterThreadCount);

                    size_t nPopped = 0;
                    for ( size_t n = 0; n < s_nWriterThreadCount; ++n )
                        nPopped += consumer.m_WriterData[n].size();

                    {
                        std::stringstream s;
                        s << "consumer" << (i - s_nWriterThreadCount) << "_popped";
                        propout() << std::make_pair( s.str().c_str(), nPopped );
                    }
                    nPoppedItems += nPopped;
                }
                else {
                    Producer<Queue>& producer = static_cast<Producer<Queue>&>( thr );
                    nPushFailed += producer.m_nPushFailed;
                    if ( !std::is_base_of<cds::bounded_container, Queue>::value ) {
                        EXPECT_EQ( producer.m_nPushFailed, 0u ) << "producer " << i;
                    }
                }
            }
            EXPECT_EQ( nTotalPops, nPoppedItems );

            propout() << std::make_pair( "success_pop", nTotalPops )
                << std::make_pair( "empty_pop", nPopFalse )
                << std::make_pair( "failed_push", nPushFailed );

            size_t nQueueSize = s_nThreadPushCount * s_nWriterThreadCount;
            EXPECT_EQ( nTotalPops + nPostTestPops, nQueueSize );
            EXPECT_TRUE( testQueue.empty());

            // Test that all items have been popped
            // Test FIFO order
            for ( size_t nWriter = 0; nWriter < s_nWriterThreadCount; ++nWriter ) {
                std::vector<size_t> arrData;
                arrData.reserve( s_nThreadPushCount );
                for ( size_t nReader = 0; nReader < arrReaders.size(); ++nReader ) {
                    ReaderIterator it = arrReaders[nReader]->m_WriterData[nWriter].begin();
                    ReaderIterator itEnd = arrReaders[nReader]->m_WriterData[nWriter].end();
                    if ( it != itEnd ) {
                        ReaderIterator itPrev = it;
                        for ( ++it; it != itEnd; ++it ) {
                            EXPECT_LT( *itPrev, *it + nRightOffset )
                                << "Reader " << nReader << ", Writer " << nWriter << ": prev=" << *itPrev << ", cur=" << *it;
                            itPrev = it;
                        }
                    }

                    for ( it = arrReaders[nReader]->m_WriterData[nWriter].begin(); it != itEnd; ++it )
                        arrData.push_back( *it );
                }
                std::sort( arrData.begin(), arrData.end());
                for ( size_t i=1; i < arrData.size(); ++i ) {
                    if ( arrData[i-1] + 1 != arrData[i] ) {
                        EXPECT_EQ( arrData[i-1] + 1,  arrData[i] ) << "Writer " << nWriter << ": [" << (i-1) << "]=" << arrData[i-1]
                            << ", [" << i << "]=" << arrData[i];
                    }
                }

                EXPECT_EQ( arrData[0], 0u ) << "Writer " << nWriter;
                EXPECT_EQ( arrData[arrData.size() - 1], s_nThreadPushCount - 1 ) << "Writer " << nWriter;
            }
        }

        template <class Queue>
        void test( Queue& q, value_array<typename Queue::value_type>& arrValue, size_t nLeftOffset, size_t nRightOffset )
        {
            s_nThreadPushCount = s_nQueueSize / s_nWriterThreadCount;
            s_nQueueSize = s_nThreadPushCount * s_nWriterThreadCount;
            propout() << std::make_pair( "producer_count", s_nWriterThreadCount )
                << std::make_pair( "consumer_count", s_nReaderThreadCount )
                << std::make_pair( "queue_size", s_nQueueSize );

            typename Queue::value_type * pValStart = arrValue.get();
            typename Queue::value_type * pValEnd = pValStart + s_nQueueSize;

            cds_test::thread_pool& pool = get_pool();
            s_nProducerCount.store( s_nWriterThreadCount, atomics::memory_order_release );

            // Writers must be first
            pool.add( new Producer<Queue>( pool, q ), s_nWriterThreadCount );
            {
                for ( typename Queue::value_type * it = pValStart; it != pValEnd; ++it ) {
                    it->nNo = 0;
                    it->nWriterNo = 0;
                    it->nConsumer = c_nBadConsumer;
                }

                typename Queue::value_type * pStart = pValStart;
                for ( size_t i = 0; i < pool.size(); ++i ) {
                    Producer<Queue>& producer = static_cast<Producer<Queue>&>( pool.get( i ));
                    producer.m_pStart = pStart;
                    pStart += s_nThreadPushCount;
                    producer.m_pEnd = pStart;
                }
            }
            pool.add( new Consumer<Queue>( pool, q ), s_nReaderThreadCount );

            std::chrono::milliseconds duration = pool.run();
            propout() << std::make_pair( "duration", duration );

            // Check that all values have been dequeued
            {
                size_t nBadConsumerCount = 0;
                typename Queue::value_type * pEnd = pValStart + s_nQueueSize;
                for ( typename Queue::value_type * it = pValStart; it != pEnd; ++it  ) {
                    if ( it->nConsumer == c_nBadConsumer )
                        ++nBadConsumerCount;
                }
                EXPECT_EQ( nBadConsumerCount, 0u );
            }

            analyze( q, nLeftOffset, nRightOffset );

            propout() << q.statistics();
        }
    };

#define CDSSTRESS_QUEUE_F( QueueType, NodeType ) \
    TEST_F( intrusive_queue_push_pop, QueueType ) \
    { \
        typedef value_type<NodeType> node_type; \
        typedef typename queue::Types< node_type >::QueueType queue_type; \
        value_array<typename queue_type::value_type> arrValue( s_nQueueSize ); \
        { \
            queue_type q; \
            test( q, arrValue, 0, 0 ); \
        } \
        queue_type::gc::force_dispose(); \
    }

    CDSSTRESS_QUEUE_F( MSQueue_HP,       cds::intrusive::msqueue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( MSQueue_HP_ic,    cds::intrusive::msqueue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( MSQueue_HP_stat,  cds::intrusive::msqueue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( MSQueue_DHP,      cds::intrusive::msqueue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( MSQueue_DHP_ic,   cds::intrusive::msqueue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( MSQueue_DHP_stat, cds::intrusive::msqueue::node<cds::gc::DHP> )

    CDSSTRESS_QUEUE_F( MoirQueue_HP,       cds::intrusive::msqueue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( MoirQueue_HP_ic,    cds::intrusive::msqueue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( MoirQueue_HP_stat,  cds::intrusive::msqueue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( MoirQueue_DHP,      cds::intrusive::msqueue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( MoirQueue_DHP_ic,   cds::intrusive::msqueue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( MoirQueue_DHP_stat, cds::intrusive::msqueue::node<cds::gc::DHP> )

    CDSSTRESS_QUEUE_F( OptimisticQueue_HP,       cds::intrusive::optimistic_queue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( OptimisticQueue_HP_ic,    cds::intrusive::optimistic_queue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( OptimisticQueue_HP_stat,  cds::intrusive::optimistic_queue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( OptimisticQueue_DHP,      cds::intrusive::optimistic_queue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( OptimisticQueue_DHP_ic,   cds::intrusive::optimistic_queue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( OptimisticQueue_DHP_stat, cds::intrusive::optimistic_queue::node<cds::gc::DHP> )

    CDSSTRESS_QUEUE_F( BasketQueue_HP,       cds::intrusive::basket_queue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( BasketQueue_HP_ic,    cds::intrusive::basket_queue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( BasketQueue_HP_stat,  cds::intrusive::basket_queue::node<cds::gc::HP> )
    CDSSTRESS_QUEUE_F( BasketQueue_DHP,      cds::intrusive::basket_queue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( BasketQueue_DHP_ic,   cds::intrusive::basket_queue::node<cds::gc::DHP> )
    CDSSTRESS_QUEUE_F( BasketQueue_DHP_stat, cds::intrusive::basket_queue::node<cds::gc::DHP> )
#undef CDSSTRESS_QUEUE_F


#define CDSSTRESS_QUEUE_F( QueueType, NodeType ) \
    TEST_F( intrusive_queue_push_pop, QueueType ) \
    { \
        typedef value_type<NodeType> node_type; \
        typedef typename queue::Types< node_type >::QueueType queue_type; \
        value_array<typename queue_type::value_type> arrValue( s_nQueueSize ); \
        queue_type q( s_nFCCompactFactor, s_nFCPassCount ); \
        test( q, arrValue, 0, 0 ); \
    }

    CDSSTRESS_QUEUE_F(FCQueue_list_delay2,                      boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_delay2_elimination,          boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_delay2_elimination_stat,     boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_expbackoff_elimination,      boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_expbackoff_elimination_stat, boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_wait_ss,                     boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_wait_ss_stat,                boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_wait_sm,                     boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_wait_sm_stat,                boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_wait_mm,                     boost::intrusive::list_base_hook<> )
    CDSSTRESS_QUEUE_F(FCQueue_list_wait_mm_stat,                boost::intrusive::list_base_hook<> )
#undef CDSSTRESS_QUEUE_F


#define CDSSTRESS_QUEUE_F( QueueType ) \
    TEST_F( intrusive_queue_push_pop, QueueType ) \
    { \
        typedef typename queue::Types< value_type<> >::QueueType queue_type; \
        value_array<typename queue_type::value_type> arrValue( s_nQueueSize ); \
        queue_type q( s_nQueueSize ); \
        test( q, arrValue, 0, 0 ); \
    }

    CDSSTRESS_QUEUE_F( VyukovMPMCCycleQueue_dyn )
    CDSSTRESS_QUEUE_F( VyukovMPMCCycleQueue_dyn_ic )
#undef CDSSTRESS_QUEUE_F


    // ********************************************************************
    // SegmentedQueue test

    class intrusive_segmented_queue_push_pop
        : public intrusive_queue_push_pop
        , public ::testing::WithParamInterface< size_t >
    {
        typedef intrusive_queue_push_pop base_class;

    protected:
        template <typename Queue>
        void test()
        {
            value_array<typename Queue::value_type> arrValue( s_nQueueSize ); \
            {
                size_t quasi_factor = GetParam();

                Queue q( quasi_factor );
                propout() << std::make_pair( "quasi_factor", quasi_factor );

                base_class::test( q, arrValue, quasi_factor * 2, quasi_factor );
            }
            Queue::gc::force_dispose();
        }

    public:
        static std::vector< size_t > get_test_parameters()
        {
            cds_test::config const& cfg = cds_test::stress_fixture::get_config( "intrusive_queue_push_pop" );
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

#define CDSSTRESS_QUEUE_F( type_name ) \
    TEST_P( intrusive_segmented_queue_push_pop, type_name ) \
    { \
        typedef typename queue::Types<value_type<>>::type_name queue_type; \
        test< queue_type >(); \
    }

    CDSSTRESS_QUEUE_F( SegmentedQueue_HP_spin )
    //CDSSTRESS_QUEUE_F( SegmentedQueue_HP_spin_padding )
    CDSSTRESS_QUEUE_F( SegmentedQueue_HP_spin_stat )
    CDSSTRESS_QUEUE_F( SegmentedQueue_HP_mutex )
    //CDSSTRESS_QUEUE_F( SegmentedQueue_HP_mutex_padding )
    CDSSTRESS_QUEUE_F( SegmentedQueue_HP_mutex_stat )
    CDSSTRESS_QUEUE_F( SegmentedQueue_DHP_spin )
    //CDSSTRESS_QUEUE_F( SegmentedQueue_DHP_spin_padding )
    CDSSTRESS_QUEUE_F( SegmentedQueue_DHP_spin_stat )
    CDSSTRESS_QUEUE_F( SegmentedQueue_DHP_mutex )
    //CDSSTRESS_QUEUE_F( SegmentedQueue_DHP_mutex_padding )
    CDSSTRESS_QUEUE_F( SegmentedQueue_DHP_mutex_stat )

    INSTANTIATE_TEST_CASE_P( SQ,
        intrusive_segmented_queue_push_pop,
        ::testing::ValuesIn( intrusive_segmented_queue_push_pop::get_test_parameters()));

} // namespace
