//$$CDS-header$$

#include "cppunit/thread.h"
#include "queue/intrusive_queue_type.h"
#include "queue/intrusive_queue_defs.h"
#include <vector>
#include <algorithm>

// Multi-threaded random queue test
namespace queue {

#define TEST_CASE( Q, HOOK )    void Q() { test< Types< Value<HOOK> >::Q >(); }
#define TEST_BOUNDED( Q )       void Q() { test_bounded< Types< Value<> >::Q >(); }
#define TEST_FCQUEUE( Q, HOOK ) void Q() { test_fcqueue< Types< Value<HOOK> >::Q >(); }
#define TEST_SEGMENTED( Q )     void Q() { test_segmented< Types< Value<> >::Q >(); }
#define TEST_BOOST( Q, HOOK )   void Q() { test_boost< Types< Value<HOOK> >::Q >(); }

    namespace {
        static size_t s_nReaderThreadCount = 4;
        static size_t s_nWriterThreadCount = 4;
        static size_t s_nQueueSize = 4000000;
        static unsigned int s_nFCPassCount = 8;
        static unsigned int s_nFCCompactFactor = 64;

        struct empty {};

        template <typename Base = empty >
        struct Value: public Base
        {
            size_t      nNo;
            size_t      nWriterNo;
            size_t      nConsumer;
        };
    }

    class IntrusiveQueue_ReaderWriter: public CppUnitMini::TestCase
    {
        template <class Queue>
        class Producer: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Producer( *this );
            }
        public:
            Queue&              m_Queue;
            double              m_fTime;
            size_t              m_nPushFailed;

            // Interval in m_arrValue
            typename Queue::value_type *       m_pStart;
            typename Queue::value_type *       m_pEnd;

        public:
            Producer( CppUnitMini::ThreadPool& pool, Queue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {}
            Producer( Producer& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {}

            IntrusiveQueue_ReaderWriter&  getTest()
            {
                return static_cast<IntrusiveQueue_ReaderWriter&>( m_Pool.m_Test );
            }

            virtual void init()
            {
                cds::threading::Manager::attachThread();
            }
            virtual void fini()
            {
                cds::threading::Manager::detachThread();
            }

            virtual void test()
            {
                m_nPushFailed = 0;

                m_fTime = m_Timer.duration();

                size_t i = 0;
                for ( typename Queue::value_type * p = m_pStart; p < m_pEnd; ) {
                    p->nNo = i;
                    p->nWriterNo = m_nThreadNo;
                    if ( m_Queue.push( *p )) {
                        ++p;
                        ++i;
                    }
                    else
                        ++m_nPushFailed;
                }

                m_fTime = m_Timer.duration() - m_fTime;
                getTest().m_nProducerCount.fetch_sub( 1, atomics::memory_order_release );
            }
        };

        template <class Queue>
        class Consumer: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Consumer( *this );
            }
        public:
            Queue&              m_Queue;
            double              m_fTime;
            size_t              m_nPopEmpty;
            size_t              m_nPopped;
            size_t              m_nBadWriter;

            typedef std::vector<size_t> TPoppedData;
            typedef std::vector<size_t>::iterator       data_iterator;
            typedef std::vector<size_t>::const_iterator const_data_iterator;

            std::vector<TPoppedData>        m_WriterData;

        private:
            void initPoppedData()
            {
                const size_t nWriterCount = s_nWriterThreadCount;
                const size_t nWriterPushCount = getTest().m_nThreadPushCount;
                m_WriterData.resize( nWriterCount );
                for ( size_t i = 0; i < nWriterCount; ++i )
                    m_WriterData[i].reserve( nWriterPushCount );
            }

        public:
            Consumer( CppUnitMini::ThreadPool& pool, Queue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {
                initPoppedData();
            }
            Consumer( Consumer& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {
                initPoppedData();
            }

            IntrusiveQueue_ReaderWriter&  getTest()
            {
                return static_cast<IntrusiveQueue_ReaderWriter&>( m_Pool.m_Test );
            }

            virtual void init()
            {
                cds::threading::Manager::attachThread();
            }
            virtual void fini()
            {
                cds::threading::Manager::detachThread();
            }

            virtual void test()
            {
                m_nPopEmpty = 0;
                m_nPopped = 0;
                m_nBadWriter = 0;
                const size_t nTotalWriters = s_nWriterThreadCount;

                m_fTime = m_Timer.duration();

                while ( true ) {
                    typename Queue::value_type * p = m_Queue.pop();
                    if ( p ) {
                        p->nConsumer = m_nThreadNo;
                        ++m_nPopped;
                        if ( p->nWriterNo < nTotalWriters )
                            m_WriterData[ p->nWriterNo ].push_back( p->nNo );
                        else
                            ++m_nBadWriter;
                    }
                    else {
                        ++m_nPopEmpty;
                        if ( getTest().m_nProducerCount.load( atomics::memory_order_acquire ) == 0 && m_Queue.empty() )
                            break;
                    }
                }

                m_fTime = m_Timer.duration() - m_fTime;
            }
        };

        template <typename T>
        class value_array
        {
            T * m_pArr;
        public:
            value_array( size_t nSize )
                : m_pArr( new T[nSize] )
            {}

            ~value_array()
            {
                delete [] m_pArr;
            }

            T * get() const { return m_pArr; }
        };


    protected:
        size_t                  m_nThreadPushCount;
        atomics::atomic<size_t>     m_nProducerCount;
        static CDS_CONSTEXPR const size_t c_nBadConsumer = 0xbadc0ffe;

    protected:
        template <class Queue>
        void analyze( CppUnitMini::ThreadPool& pool, Queue& testQueue, size_t /*nLeftOffset*/, size_t nRightOffset )
        {
            typedef Consumer<Queue> Reader;
            typedef Producer<Queue> Writer;
            typedef typename Reader::const_data_iterator    ReaderIterator;

            size_t nPostTestPops = 0;
            while ( testQueue.pop() )
                ++nPostTestPops;

            double fTimeWriter = 0;
            double fTimeReader = 0;
            size_t nTotalPops = 0;
            size_t nPopFalse = 0;
            size_t nPoppedItems = 0;
            size_t nPushFailed = 0;

            std::vector< Reader * > arrReaders;

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Reader * pReader = dynamic_cast<Reader *>( *it );
                if ( pReader ) {
                    fTimeReader += pReader->m_fTime;
                    nTotalPops += pReader->m_nPopped;
                    nPopFalse += pReader->m_nPopEmpty;
                    arrReaders.push_back( pReader );
                    CPPUNIT_CHECK_EX( pReader->m_nBadWriter == 0, "reader " << pReader->m_nThreadNo << " bad writer event count=" << pReader->m_nBadWriter );

                    size_t nPopped = 0;
                    for ( size_t n = 0; n < s_nWriterThreadCount; ++n )
                        nPopped += pReader->m_WriterData[n].size();

                    CPPUNIT_MSG( "    Reader " << pReader->m_nThreadNo - s_nWriterThreadCount << " popped count=" << nPopped );
                    nPoppedItems += nPopped;
                }
                else {
                    Writer * pWriter = dynamic_cast<Writer *>( *it );
                    CPPUNIT_ASSERT( pWriter != nullptr );
                    fTimeWriter += pWriter->m_fTime;
                    nPushFailed += pWriter->m_nPushFailed;
                    if ( !boost::is_base_of<cds::bounded_container, Queue>::value ) {
                        CPPUNIT_CHECK_EX( pWriter->m_nPushFailed == 0,
                            "writer " << pWriter->m_nThreadNo << " push failed count=" << pWriter->m_nPushFailed );
                    }
                }
            }
            CPPUNIT_CHECK_EX( nTotalPops == nPoppedItems, "nTotalPops=" << nTotalPops << ", nPoppedItems=" << nPoppedItems );

            CPPUNIT_MSG( "    Readers: duration=" << fTimeReader / s_nReaderThreadCount << ", success pop=" << nTotalPops << ", failed pops=" << nPopFalse );
            CPPUNIT_MSG( "    Writers: duration=" << fTimeWriter / s_nWriterThreadCount << ", failed push=" << nPushFailed );

            size_t nQueueSize = m_nThreadPushCount * s_nWriterThreadCount;
            CPPUNIT_CHECK_EX( nTotalPops + nPostTestPops == nQueueSize, "popped=" << nTotalPops + nPostTestPops << " must be " << nQueueSize );
            CPPUNIT_CHECK( testQueue.empty() );

            // Test that all items have been popped
            // Test FIFO order
            CPPUNIT_MSG( "   Test consistency of popped sequence..." );
            size_t nErrors = 0;
            for ( size_t nWriter = 0; nWriter < s_nWriterThreadCount; ++nWriter ) {
                std::vector<size_t> arrData;
                arrData.reserve( m_nThreadPushCount );
                nErrors = 0;
                for ( size_t nReader = 0; nReader < arrReaders.size(); ++nReader ) {
                    ReaderIterator it = arrReaders[nReader]->m_WriterData[nWriter].begin();
                    ReaderIterator itEnd = arrReaders[nReader]->m_WriterData[nWriter].end();
                    if ( it != itEnd ) {
                        ReaderIterator itPrev = it;
                        for ( ++it; it != itEnd; ++it ) {
                            CPPUNIT_CHECK_EX( *itPrev < *it + nRightOffset,
                                "Reader " << nReader << ", Writer " << nWriter << ": prev=" << *itPrev << ", cur=" << *it );
                            if ( ++nErrors > 10 )
                                return;
                            itPrev = it;
                        }
                    }

                    for ( it = arrReaders[nReader]->m_WriterData[nWriter].begin(); it != itEnd; ++it )
                        arrData.push_back( *it );
                }
                std::sort( arrData.begin(), arrData.end() );
                nErrors = 0;
                for ( size_t i=1; i < arrData.size(); ++i ) {
                    if ( arrData[i-1] + 1 != arrData[i] ) {
                        CPPUNIT_CHECK_EX( arrData[i-1] + 1 == arrData[i], "Writer " << nWriter << ": [" << (i-1) << "]=" << arrData[i-1] << ", [" << i << "]=" << arrData[i] );
                        if ( ++nErrors > 10 )
                            return;
                    }
                }

                CPPUNIT_CHECK_EX( arrData[0] == 0, "Writer " << nWriter << "[0] != 0" );
                CPPUNIT_CHECK_EX( arrData[arrData.size() - 1] == m_nThreadPushCount - 1, "Writer " << nWriter << "[last] != " << m_nThreadPushCount - 1 );
            }
        }

        template <class Queue>
        void test_with( Queue& testQueue, value_array<typename Queue::value_type>& arrValue, size_t nLeftOffset, size_t nRightOffset )
        {
            m_nThreadPushCount = s_nQueueSize / s_nWriterThreadCount;
            CPPUNIT_MSG( "    reader count=" << s_nReaderThreadCount << " writer count=" << s_nWriterThreadCount
                << " item count=" << m_nThreadPushCount * s_nWriterThreadCount << "..." );

            typename Queue::value_type * pValStart = arrValue.get();
            typename Queue::value_type * pValEnd = pValStart + s_nQueueSize;

            CppUnitMini::ThreadPool pool( *this );

            m_nProducerCount.store( s_nWriterThreadCount, atomics::memory_order_release );

            // Writers must be first
            pool.add( new Producer<Queue>( pool, testQueue ), s_nWriterThreadCount );
            {
                for ( typename Queue::value_type * it = pValStart; it != pValEnd; ++it ) {
                    it->nNo = 0;
                    it->nWriterNo = 0;
                    it->nConsumer = c_nBadConsumer;
                }

                typename Queue::value_type * pStart = pValStart;
                for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                    static_cast<Producer<Queue>* >( *it )->m_pStart = pStart;
                    pStart += m_nThreadPushCount;
                    static_cast<Producer<Queue>* >( *it )->m_pEnd = pStart;
                }
            }
            pool.add( new Consumer<Queue>( pool, testQueue ), s_nReaderThreadCount );

            pool.run();

            // Check that all values have been dequeued
            {
                size_t nBadConsumerCount = 0;
                size_t nQueueSize = m_nThreadPushCount * s_nWriterThreadCount;
                typename Queue::value_type * pEnd = pValStart + nQueueSize;
                for ( typename Queue::value_type * it = pValStart; it != pEnd; ++it  ) {
                    if ( it->nConsumer == c_nBadConsumer )
                        ++nBadConsumerCount;
                }
                CPPUNIT_CHECK_EX( nBadConsumerCount == 0, "nBadConsumerCount=" << nBadConsumerCount );
            }

            analyze( pool, testQueue, nLeftOffset, nRightOffset );
            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <typename Queue>
        void test()
        {
            value_array<typename Queue::value_type> arrValue( s_nQueueSize );
            {
                {
                    Queue q;
                    test_with( q, arrValue, 0, 0 );
                }
                Queue::gc::force_dispose();
            }
        }

        template <typename Queue>
        void test_boost()
        {
            value_array<typename Queue::value_type> arrValue( s_nQueueSize );
            {
                Queue q;
                test_with(q, arrValue, 0, 0);
            }
        }

        template <typename Queue>
        void test_bounded()
        {
            value_array<typename Queue::value_type> arrValue( s_nQueueSize );
            Queue q;
            test_with(q, arrValue, 0, 0);
        }

        template <typename Queue>
        void test_fcqueue()
        {
            value_array<typename Queue::value_type> arrValue( s_nQueueSize );
            CPPUNIT_MSG( "Combining pass count: " << s_nFCPassCount << ", compact factor: " << s_nFCCompactFactor );
            Queue q( s_nFCCompactFactor, s_nFCPassCount );
            test_with(q, arrValue, 0, 0);
        }

        template <typename Queue>
        void test_segmented()
        {
            value_array<typename Queue::value_type> arrValue( s_nQueueSize );
            for ( size_t nSegmentSize = 4; nSegmentSize <= 256; nSegmentSize *= 4 ) {
                CPPUNIT_MSG( "Segment size: " << nSegmentSize );
                {
                    Queue q( nSegmentSize );
                    test_with( q, arrValue, nSegmentSize * 2, nSegmentSize );
                }
                Queue::gc::force_dispose();
            }
        }

        template <typename Queue>
        void test_spqueue()
        {
            value_array<typename Queue::value_type> arrValue( s_nQueueSize );
            for ( size_t nArraySize = 2; nArraySize <= 64; nArraySize *= 2 ) {
                CPPUNIT_MSG( "Array size: " << nArraySize );
                {
                    Queue q( nArraySize );
                    test_with( q, arrValue, 0, 0 );
                }
                Queue::gc::force_dispose();
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nReaderThreadCount = cfg.getULong("ReaderCount", 4 );
            s_nWriterThreadCount = cfg.getULong("WriterCount", 4 );
            s_nQueueSize = cfg.getULong("QueueSize", 10000000 );
            s_nFCPassCount = cfg.getUInt("FCPassCount", 8);
            s_nFCCompactFactor = cfg.getUInt("FCCompactFactor", 64);
        }

    protected:
        CDSUNIT_DECLARE_MSQueue
        CDSUNIT_DECLARE_MoirQueue
        CDSUNIT_DECLARE_OptimisticQueue
        CDSUNIT_DECLARE_BasketQueue
        CDSUNIT_DECLARE_FCQueue
        CDSUNIT_DECLARE_SegmentedQueue
        CDSUNIT_DECLARE_TsigasCycleQueue
        CDSUNIT_DECLARE_VyukovMPMCCycleQueue
        CDSUNIT_DECLARE_BoostSList


        CPPUNIT_TEST_SUITE(IntrusiveQueue_ReaderWriter)
            CDSUNIT_TEST_MSQueue
            CDSUNIT_TEST_MoirQueue
            CDSUNIT_TEST_OptimisticQueue
            CDSUNIT_TEST_BasketQueue
            CDSUNIT_TEST_FCQueue
            CDSUNIT_TEST_SegmentedQueue
            CDSUNIT_TEST_TsigasCycleQueue
            CDSUNIT_TEST_VyukovMPMCCycleQueue
            CDSUNIT_TEST_BoostSList
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(queue::IntrusiveQueue_ReaderWriter);
