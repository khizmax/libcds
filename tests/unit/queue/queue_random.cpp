//$$CDS-header$$

#include "cppunit/thread.h"
#include "queue/queue_type.h"
#include "queue/queue_defs.h"


#include <vector>
#include <boost/type_traits/is_base_of.hpp>

// Multi-threaded queue test for random push/pop operation
namespace queue {

#define TEST_CASE( Q, V )       void Q() { test< Types<V>::Q >(); }
#define TEST_BOUNDED( Q, V )    TEST_CASE( Q, V )
#define TEST_SEGMENTED( Q, V )  void Q() { test_segmented< Types< V >::Q >(); }

    namespace ns_Queue_Random {
        static size_t s_nThreadCount = 16;
        static size_t s_nQueueSize = 10000000;

        struct SimpleValue {
            size_t      nNo;
            size_t      nThread;

            SimpleValue() {}
            SimpleValue( size_t n ): nNo(n) {}
            size_t getNo() const { return  nNo; }
        };
    }

    using namespace ns_Queue_Random;

    class Queue_Random: public CppUnitMini::TestCase
    {
        typedef CppUnitMini::TestCase base_class;

        template <class Queue>
        class Thread: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Thread( *this );
            }
        public:
            Queue&              m_Queue;
            double              m_fTime;

            size_t  m_nPushCount;
            size_t  m_nPopCount;
            size_t  m_nEmptyPop;

            size_t  m_nUndefWriter;
            size_t  m_nRepeatValue;
            size_t  m_nPushError        ;    // push error count

            std::vector<size_t> m_arrLastRead;
            std::vector<size_t> m_arrPopCountPerThread;

            size_t const m_nSpread;

        public:
            Thread( CppUnitMini::ThreadPool& pool, Queue& q, size_t nSpread = 0 )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
                , m_nSpread( nSpread )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
                , m_nSpread( src.m_nSpread )
            {}

            Queue_Random&  getTest()
            {
                return reinterpret_cast<Queue_Random&>( m_Pool.m_Test );
            }

            virtual void init()
            {
                cds::threading::Manager::attachThread();
                m_nPushCount =
                    m_nPopCount =
                    m_nEmptyPop =
                    m_nUndefWriter =
                    m_nRepeatValue =
                    m_nPushError = 0;

                m_arrLastRead.resize( s_nThreadCount, 0 );
                m_arrPopCountPerThread.resize( s_nThreadCount, 0 );
            }
            virtual void fini()
            {
                cds::threading::Manager::detachThread();
            }

            virtual void test()
            {
                size_t const nThreadCount = s_nThreadCount;
                size_t const nTotalPush = getTest().m_nThreadPushCount;

                SimpleValue node;

                m_fTime = m_Timer.duration();

                bool bNextPop = false;
                while ( m_nPushCount < nTotalPush ) {
                    if ( !bNextPop && (rand() & 3) != 3 ) {
                        // push
                        node.nThread = m_nThreadNo;
                        node.nNo = ++m_nPushCount;
                        if ( !m_Queue.push( node )) {
                            ++m_nPushError;
                            --m_nPushCount;
                        }

                    }
                    else {
                        // pop
                        pop( nThreadCount );
                        bNextPop = false;
                    }
                }

                size_t nPopLoop = 0;
                while ( !m_Queue.empty() && nPopLoop < 1000000 ) {
                    if ( pop( nThreadCount ) )
                        nPopLoop = 0;
                    else
                        ++nPopLoop;
                }


                m_fTime = m_Timer.duration() - m_fTime;
            }

            bool pop( size_t nThreadCount )
            {
                SimpleValue node;
                node.nThread = -1;
                node.nNo = -1;
                if ( m_Queue.pop( node )) {
                    ++m_nPopCount;
                    if ( node.nThread < nThreadCount ) {
                        m_arrPopCountPerThread[ node.nThread ] += 1;
                        if ( m_nSpread ) {
                            if ( m_arrLastRead[ node.nThread ] > node.nNo ) {
                                if ( m_arrLastRead[ node.nThread ] - node.nNo > m_nSpread )
                                    ++m_nRepeatValue;
                            }
                            else if ( m_arrLastRead[ node.nThread ] == node.nNo )
                                ++m_nRepeatValue;
                            m_arrLastRead[ node.nThread ] = node.nNo;
                        }
                        else {
                            if ( m_arrLastRead[ node.nThread ] < node.nNo ) {
                                m_arrLastRead[ node.nThread ] = node.nNo;
                            }
                            else
                                ++m_nRepeatValue;
                        }

                        //if ( node.nNo < m_Test.m_nPushCount )
                        //    m_Test.m_pRead[ node.nWriter ][ node.nNo ] = node.nNo;
                    }
                    else {
                        ++m_nUndefWriter;
                    }
                }
                else {
                    ++m_nEmptyPop;
                    return false;
                }
                return true;
            }
        };

    protected:
        size_t  m_nThreadPushCount;

    protected:
        template <class Queue>
        void analyze( CppUnitMini::ThreadPool& pool, Queue& testQueue  )
        {
            CPPUNIT_CHECK( testQueue.empty() );

            std::vector< size_t > arrPushCount;
            arrPushCount.resize( s_nThreadCount, 0 );

            size_t nPushTotal = 0;
            size_t nPopTotal  = 0;
            double fTime = 0;
            size_t nPushError = 0;

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Queue> * pThread = static_cast<Thread<Queue> *>( *it );
                CPPUNIT_CHECK( pThread->m_nUndefWriter == 0 );
                CPPUNIT_CHECK_EX( pThread->m_nRepeatValue == 0, "nRepeatValue=" << pThread->m_nRepeatValue );
                if ( !boost::is_base_of<cds::bounded_container, Queue>::value ) {
                    CPPUNIT_CHECK( pThread->m_nPushError == 0 );
                }
                else
                    nPushError += pThread->m_nPushError;

                arrPushCount[ pThread->m_nThreadNo ] += pThread->m_nPushCount;

                nPushTotal += pThread->m_nPushCount;
                nPopTotal += pThread->m_nPopCount;
                fTime += pThread->m_fTime;
            }

            CPPUNIT_MSG( "     Duration=" << (fTime /= s_nThreadCount) );
            if ( boost::is_base_of<cds::bounded_container, Queue>::value ) {
                CPPUNIT_MSG( "         push error (when queue is full)=" << nPushError );
            }

            size_t nTotalItems = m_nThreadPushCount * s_nThreadCount;

            CPPUNIT_CHECK_EX( nPushTotal == nTotalItems, "nPushTotal=" << nPushTotal << ", nTotalItems=" << nTotalItems );
            CPPUNIT_CHECK_EX( nPopTotal == nTotalItems, "nPopTotal=" << nPopTotal << ", nTotalItems=" << nTotalItems );

            for ( size_t i = 0; i < s_nThreadCount; ++i )
                CPPUNIT_CHECK( arrPushCount[i] == m_nThreadPushCount );
        }

        template <class Queue>
        void test()
        {
            CPPUNIT_MSG( "Random push/pop test\n    thread count=" << s_nThreadCount << ", push count=" << s_nQueueSize << " ..." );

            m_nThreadPushCount = s_nQueueSize / s_nThreadCount;

            Queue testQueue;
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<Queue>( pool, testQueue ), s_nThreadCount );

            pool.run();

            analyze( pool, testQueue );
            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <class Queue>
        void test_segmented()
        {
            CPPUNIT_MSG( "Random push/pop test\n    thread count=" << s_nThreadCount << ", push count=" << s_nQueueSize << " ..." );

            m_nThreadPushCount = s_nQueueSize / s_nThreadCount;

            for ( size_t nSegmentSize = 4; nSegmentSize <= 256; nSegmentSize *= 4 ) {
                CPPUNIT_MSG( "Segment size: " << nSegmentSize );

                Queue testQueue( nSegmentSize );
                CppUnitMini::ThreadPool pool( *this );
                pool.add( new Thread<Queue>( pool, testQueue, nSegmentSize * 2 ), s_nThreadCount );

                pool.run();

                analyze( pool, testQueue );
                CPPUNIT_MSG( testQueue.statistics() );
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nThreadCount = cfg.getULong("ThreadCount", 8 );
            s_nQueueSize = cfg.getULong("QueueSize", 20000000 );
        }

    protected:
        CDSUNIT_DECLARE_MoirQueue( SimpleValue )
        CDSUNIT_DECLARE_MSQueue( SimpleValue )
        CDSUNIT_DECLARE_OptimisticQueue( SimpleValue )
        CDSUNIT_DECLARE_BasketQueue( SimpleValue )
        CDSUNIT_DECLARE_FCQueue( SimpleValue )
        CDSUNIT_DECLARE_FCDeque( SimpleValue )
        CDSUNIT_DECLARE_SegmentedQueue( SimpleValue )
        CDSUNIT_DECLARE_RWQueue( SimpleValue )
        CDSUNIT_DECLARE_TsigasCysleQueue( SimpleValue )
        CDSUNIT_DECLARE_VyukovMPMCCycleQueue( SimpleValue )
        CDSUNIT_DECLARE_StdQueue( SimpleValue )

        CPPUNIT_TEST_SUITE(Queue_Random)
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

CPPUNIT_TEST_SUITE_REGISTRATION(queue::Queue_Random);
