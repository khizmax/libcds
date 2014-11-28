//$$CDS-header$$

#include "cppunit/thread.h"
#include "queue/queue_type.h"
#include "queue/queue_defs.h"

#include <vector>
#include <algorithm>

// Multi-threaded random queue test
namespace queue {

#define TEST_CASE( Q, V )       void Q() { test< Types<V>::Q >(); }
#define TEST_BOUNDED( Q, V )    TEST_CASE( Q, V )
#define TEST_SEGMENTED( Q, V )  void Q() { test_segmented< Types< V >::Q >(); }

    namespace {
        static size_t s_nReaderThreadCount = 4;
        static size_t s_nWriterThreadCount = 4;
        static size_t s_nQueueSize = 4000000;

        struct Value {
            size_t      nNo;
            size_t      nWriterNo;
        };
    }

    class Queue_ReaderWriter: public CppUnitMini::TestCase
    {
        template <class Queue>
        class WriterThread: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new WriterThread( *this );
            }
        public:
            Queue&              m_Queue;
            double              m_fTime;
            size_t              m_nPushFailed;

        public:
            WriterThread( CppUnitMini::ThreadPool& pool, Queue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {}
            WriterThread( WriterThread& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {}

            Queue_ReaderWriter&  getTest()
            {
                return reinterpret_cast<Queue_ReaderWriter&>( m_Pool.m_Test );
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
                size_t nPushCount = getTest().m_nThreadPushCount;
                Value v;
                v.nWriterNo = m_nThreadNo;
                v.nNo = 0;
                m_nPushFailed = 0;

                m_fTime = m_Timer.duration();

                while ( v.nNo < nPushCount ) {
                    if ( m_Queue.push( v ))
                        ++v.nNo;
                    else
                        ++m_nPushFailed;
                }

                m_fTime = m_Timer.duration() - m_fTime;
                getTest().m_nWriterDone.fetch_add( 1 );
            }
        };

        template <class Queue>
        class ReaderThread: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new ReaderThread( *this );
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
            ReaderThread( CppUnitMini::ThreadPool& pool, Queue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {
                initPoppedData();
            }
            ReaderThread( ReaderThread& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {
                initPoppedData();
            }

            Queue_ReaderWriter&  getTest()
            {
                return reinterpret_cast<Queue_ReaderWriter&>( m_Pool.m_Test );
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
                Value v;

                m_fTime = m_Timer.duration();

                while ( true ) {
                    if ( m_Queue.pop( v ) ) {
                        ++m_nPopped;
                        if ( /*v.nWriterNo >= 0 &&*/ v.nWriterNo < nTotalWriters )
                            m_WriterData[ v.nWriterNo ].push_back( v.nNo );
                        else
                            ++m_nBadWriter;
                    }
                    else
                        ++m_nPopEmpty;

                    if ( m_Queue.empty() ) {
                        if ( getTest().m_nWriterDone.load() >= nTotalWriters ) {
                            if ( m_Queue.empty() )
                                    break;
                        }
                    }
                }

                m_fTime = m_Timer.duration() - m_fTime;
            }
        };

    protected:
        size_t                  m_nThreadPushCount;
        atomics::atomic<size_t>     m_nWriterDone;

    protected:
        template <class Queue>
        void analyze( CppUnitMini::ThreadPool& pool, Queue& testQueue, size_t /*nLeftOffset*/ = 0, size_t nRightOffset = 0  )
        {
            typedef ReaderThread<Queue> Reader;
            typedef WriterThread<Queue> Writer;
            typedef typename Reader::const_data_iterator    ReaderIterator;

            size_t nPostTestPops = 0;
            {
                Value v;
                while ( testQueue.pop( v ))
                    ++nPostTestPops;
            }

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
                            CPPUNIT_CHECK_EX( *itPrev < *it + nRightOffset, "Reader " << nReader << ", Writer " << nWriter << ": prev=" << *itPrev << ", cur=" << *it );
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
        void test()
        {
            m_nThreadPushCount = s_nQueueSize / s_nWriterThreadCount;
            CPPUNIT_MSG( "    reader count=" << s_nReaderThreadCount << " writer count=" << s_nWriterThreadCount
                << " item count=" << m_nThreadPushCount * s_nWriterThreadCount << "..." );

            Queue testQueue;
            CppUnitMini::ThreadPool pool( *this );

            m_nWriterDone.store( 0 );

            // Writers must be first
            pool.add( new WriterThread<Queue>( pool, testQueue ), s_nWriterThreadCount );
            pool.add( new ReaderThread<Queue>( pool, testQueue ), s_nReaderThreadCount );

            //CPPUNIT_MSG( "   Reader/Writer test, reader count=" << s_nReaderThreadCount << " writer count=" << s_nWriterThreadCount << "..." );
            pool.run();

            analyze( pool, testQueue );
            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <class Queue>
        void test_segmented()
        {
            m_nThreadPushCount = s_nQueueSize / s_nWriterThreadCount;
            CPPUNIT_MSG( "    reader count=" << s_nReaderThreadCount << " writer count=" << s_nWriterThreadCount
                << " item count=" << m_nThreadPushCount * s_nWriterThreadCount << "..." );

            for ( size_t nSegmentSize = 4; nSegmentSize <= 256; nSegmentSize *= 4 ) {
                CPPUNIT_MSG( "Segment size: " << nSegmentSize );

                Queue q( nSegmentSize );
                CppUnitMini::ThreadPool pool( *this );

                m_nWriterDone.store( 0 );

                // Writers must be first
                pool.add( new WriterThread<Queue>( pool, q ), s_nWriterThreadCount );
                pool.add( new ReaderThread<Queue>( pool, q ), s_nReaderThreadCount );

                pool.run();

                analyze( pool, q, nSegmentSize * 2, nSegmentSize );
                CPPUNIT_MSG( q.statistics() );
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nReaderThreadCount = cfg.getULong("ReaderCount", 4 );
            s_nWriterThreadCount = cfg.getULong("WriterCount", 4 );
            s_nQueueSize = cfg.getULong("QueueSize", 10000000 );
        }

    protected:
        CDSUNIT_DECLARE_MoirQueue( Value )
        CDSUNIT_DECLARE_MSQueue( Value )
        CDSUNIT_DECLARE_OptimisticQueue( Value )
        CDSUNIT_DECLARE_BasketQueue( Value )
        CDSUNIT_DECLARE_FCQueue( Value )
        CDSUNIT_DECLARE_FCDeque( Value )
        CDSUNIT_DECLARE_SegmentedQueue( Value )
        CDSUNIT_DECLARE_RWQueue( Value )
        CDSUNIT_DECLARE_TsigasCysleQueue( Value )
        CDSUNIT_DECLARE_VyukovMPMCCycleQueue( Value )
        CDSUNIT_DECLARE_StdQueue( Value )

        CPPUNIT_TEST_SUITE(Queue_ReaderWriter)
            CDSUNIT_TEST_MoirQueue
            CDSUNIT_TEST_MSQueue
            CDSUNIT_TEST_OptimisticQueue
            CDSUNIT_TEST_BasketQueue
            CDSUNIT_TEST_FCQueue
            CDSUNIT_TEST_FCDeque
            CDSUNIT_TEST_SegmentedQueue
            CDSUNIT_TEST_RWQueue
            CDSUNIT_TEST_TsigasCysleQueue
            CDSUNIT_TEST_VyukovMPMCCycleQueue
            CDSUNIT_TEST_StdQueue
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(queue::Queue_ReaderWriter);
