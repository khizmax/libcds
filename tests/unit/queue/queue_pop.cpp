//$$CDS-header$$

#include "cppunit/thread.h"
#include "queue/queue_type.h"
#include "queue/queue_defs.h"

// Multi-threaded queue test for pop operation
namespace queue {

#define TEST_CASE( Q, V )       void Q() { test< Types<V>::Q >(); }
#define TEST_BOUNDED( Q, V )    void Q() { test_bounded< Types<V>::Q >(); }
#define TEST_SEGMENTED( Q, V )  void Q() { test_segmented< Types<V>::Q >(); }

    namespace ns_Queue_Pop {
        static size_t s_nThreadCount = 8;
        static size_t s_nQueueSize = 20000000 ;   // no more than 20 million records

        struct SimpleValue {
            size_t    nNo;

            SimpleValue(): nNo(0) {}
            SimpleValue( size_t n ): nNo(n) {}
            size_t getNo() const { return  nNo; }
        };
    }
    using namespace ns_Queue_Pop;

    class Queue_Pop: public CppUnitMini::TestCase
    {
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
            long *              m_arr;
            size_t              m_nPopCount;

        public:
            Thread( CppUnitMini::ThreadPool& pool, Queue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {
                m_arr = new long[s_nQueueSize];
            }
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {
                m_arr = new long[s_nQueueSize];
            }
            ~Thread()
            {
                delete [] m_arr;
            }

            Queue_Pop&  getTest()
            {
                return reinterpret_cast<Queue_Pop&>( m_Pool.m_Test );
            }

            virtual void init()
            {
                cds::threading::Manager::attachThread();
                memset(m_arr, 0, sizeof(m_arr[0]) * s_nQueueSize );
            }
            virtual void fini()
            {
                cds::threading::Manager::detachThread();
            }

            virtual void test()
            {
                m_fTime = m_Timer.duration();

                typedef typename Queue::value_type value_type;
                value_type value = value_type();
                size_t nPopCount = 0;
                while ( m_Queue.pop( value ) ) {
                    ++m_arr[ value.getNo() ];
                    ++nPopCount;
                }
                m_nPopCount = nPopCount;

                m_fTime = m_Timer.duration() - m_fTime;
            }
        };

    protected:

        template <class Queue>
        void analyze( CppUnitMini::ThreadPool& pool, Queue& testQueue  )
        {
            size_t * arr = new size_t[ s_nQueueSize ];
            memset(arr, 0, sizeof(arr[0]) * s_nQueueSize );

            double fTime = 0;
            size_t nTotalPops = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Queue> * pThread = reinterpret_cast<Thread<Queue> *>(*it);
                for ( size_t i = 0; i < s_nQueueSize; ++i )
                    arr[i] += pThread->m_arr[i];
                nTotalPops += pThread->m_nPopCount;
                fTime += pThread->m_fTime;
            }
            CPPUNIT_MSG( "     Duration=" << (fTime / s_nThreadCount) );
            CPPUNIT_CHECK_EX( nTotalPops == s_nQueueSize, "Total pop=" << nTotalPops << ", queue size=" << s_nQueueSize);
            CPPUNIT_CHECK( testQueue.empty() )

            size_t nError = 0;
            for ( size_t i = 0; i < s_nQueueSize; ++i ) {
                if ( arr[i] != 1 ) {
                    CPPUNIT_MSG( "   ERROR: Item " << i << " has not been popped" );
                    CPPUNIT_ASSERT( ++nError <= 10 );
                }
            }

            delete [] arr;
        }

        template <class Queue>
        void test()
        {
            Queue testQueue;
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<Queue>( pool, testQueue ), s_nThreadCount );

            CPPUNIT_MSG( "   Create queue size =" << s_nQueueSize << " ...");
            cds::OS::Timer      timer;
            for ( size_t i = 0; i < s_nQueueSize; ++i )
                testQueue.push( i );
            CPPUNIT_MSG( "     Duration=" << timer.duration() );

            CPPUNIT_MSG( "   Pop test, thread count=" << s_nThreadCount << " ...");
            pool.run();

            analyze( pool, testQueue );

            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <class Queue>
        void test_bounded()
        {
            Queue testQueue( s_nQueueSize );
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<Queue>( pool, testQueue ), s_nThreadCount );

            CPPUNIT_MSG( "   Create queue size =" << s_nQueueSize << " ...");
            cds::OS::Timer      timer;
            for ( size_t i = 0; i < s_nQueueSize; ++i )
                testQueue.push( i );
            CPPUNIT_MSG( "     Duration=" << timer.duration() );

            CPPUNIT_MSG( "   Pop test, thread count=" << s_nThreadCount << " ...");
            pool.run();

            analyze( pool, testQueue );

            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <class Queue>
        void test_segmented()
        {
            for ( size_t nSegmentSize = 4; nSegmentSize <= 256; nSegmentSize *= 4 ) {
                CPPUNIT_MSG( "Segment size: " << nSegmentSize );

                Queue testQueue( nSegmentSize );
                CppUnitMini::ThreadPool pool( *this );
                pool.add( new Thread<Queue>( pool, testQueue ), s_nThreadCount );

                CPPUNIT_MSG( "   Create queue size =" << s_nQueueSize << " ...");
                cds::OS::Timer      timer;
                for ( size_t i = 0; i < s_nQueueSize; ++i )
                    testQueue.push( i );
                CPPUNIT_MSG( "     Duration=" << timer.duration() );

                CPPUNIT_MSG( "   Pop test, thread count=" << s_nThreadCount << " ...");
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

        CPPUNIT_TEST_SUITE(Queue_Pop)
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

CPPUNIT_TEST_SUITE_REGISTRATION(queue::Queue_Pop);
