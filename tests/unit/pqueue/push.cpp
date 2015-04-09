//$$CDS-header$$

#include "cppunit/thread.h"
#include "pqueue/pqueue_item.h"
#include "pqueue/pqueue_type.h"

#include <vector>
#include <memory>

namespace pqueue {

#define TEST_CASE( Q ) void Q() { test< Types<pqueue::SimpleValue>::Q >(); }
#define TEST_BOUNDED( Q ) void Q() { test_bounded< Types<pqueue::SimpleValue>::Q >(); }

    namespace {
        static size_t s_nThreadCount = 8;
        static size_t s_nQueueSize = 2000000;
    }
} // namespace pqueue

namespace pqueue {

    class PQueue_Push: public CppUnitMini::TestCase
    {
        template <class PQueue>
        class Pusher: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Pusher( *this );
            }
        public:
            PQueue&             m_Queue;
            size_t              m_nPushError;

            typedef std::vector<size_t> array_type;
            array_type          m_arr;

        public:
            Pusher( CppUnitMini::ThreadPool& pool, PQueue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {}
            Pusher( Pusher& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {}

            PQueue_Push&  getTest()
            {
                return static_cast<PQueue_Push&>( m_Pool.m_Test );
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
                m_nPushError = 0;

                for ( array_type::const_iterator it = m_arr.begin(); it != m_arr.end(); ++it ) {
                    if ( !m_Queue.push( SimpleValue( *it ) ))
                        ++m_nPushError;
                }
            }

            void prepare( size_t nStart, size_t nEnd )
            {
                m_arr.reserve( nEnd - nStart );
                for ( size_t i = nStart; i < nEnd; ++i )
                    m_arr.push_back( i );
                shuffle( m_arr.begin(), m_arr.end() );
            }
        };

    protected:
        template <class PQueue>
        void analyze( CppUnitMini::ThreadPool& pool, PQueue& testQueue  )
        {
            size_t nThreadItems = s_nQueueSize / s_nThreadCount;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Pusher<PQueue> * pThread = static_cast<Pusher<PQueue> *>(*it);
                CPPUNIT_CHECK_EX( pThread->m_nPushError == 0, "Thread push error count=" << pThread->m_nPushError );
            }
            CPPUNIT_MSG( "     Duration=" << pool.avgDuration() );
            CPPUNIT_ASSERT( !testQueue.empty() );

            typedef std::vector<size_t> vector_type;
            vector_type arr;
            arr.reserve( s_nQueueSize );

            cds::OS::Timer      timer;
            CPPUNIT_MSG( "   Pop (single-threaded)..." );
            size_t nPopped = 0;
            SimpleValue val;
            while ( testQueue.pop( val )) {
                nPopped++;
                arr.push_back( val.key );
            }
            CPPUNIT_MSG( "     Duration=" << timer.duration() );

            CPPUNIT_CHECK( arr.size() == nThreadItems * s_nThreadCount );
            vector_type::const_iterator it = arr.begin();
            size_t nPrev = *it;
            ++it;
            size_t nErrCount = 0;
            for ( vector_type::const_iterator itEnd = arr.end(); it != itEnd; ++it ) {
                if ( nPrev - 1 != *it ) {
                    if ( ++nErrCount < 10 ) {
                        CPPUNIT_CHECK_EX( nPrev - 1 == *it, "Expected=" << nPrev - 1 << ", current=" << *it );
                    }
                }
                nPrev = *it;
            }

            CPPUNIT_CHECK_EX( nErrCount == 0, "Error count=" << nErrCount );
        }

        template <class PQueue>
        void test()
        {
            PQueue testQueue;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Pusher<PQueue>( pool, testQueue ), s_nThreadCount );

            size_t nStart = 0;
            size_t nThreadItemCount = s_nQueueSize / s_nThreadCount;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Pusher<PQueue> * pThread = static_cast<Pusher<PQueue> *>(*it);
                pThread->prepare( nStart, nStart + nThreadItemCount );
                nStart += nThreadItemCount;
            }

            CPPUNIT_MSG( "   Push test, thread count=" << s_nThreadCount << ", item count=" << nThreadItemCount * s_nThreadCount << " ..." );
            pool.run();

            analyze( pool, testQueue );

            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <class PQueue>
        void test_bounded()
        {
            size_t nStart = 0;
            size_t nThreadItemCount = s_nQueueSize / s_nThreadCount;

            std::unique_ptr<PQueue> pq( new PQueue(s_nQueueSize) );

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Pusher<PQueue>( pool, *pq ), s_nThreadCount );

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Pusher<PQueue> * pThread = static_cast<Pusher<PQueue> *>(*it);
                pThread->prepare( nStart, nStart + nThreadItemCount );
                nStart += nThreadItemCount;
            }

            CPPUNIT_MSG( "   Push test, thread count=" << s_nThreadCount << ", item count=" << nThreadItemCount * s_nThreadCount << " ..." );
            pool.run();

            analyze( pool, *pq );

            CPPUNIT_MSG( pq->statistics() );
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nThreadCount = cfg.getULong("ThreadCount", (unsigned long) s_nThreadCount );
            s_nQueueSize = cfg.getULong("QueueSize", (unsigned long) s_nQueueSize );
        }

    protected:
#include "pqueue/pqueue_defs.h"
        CDSUNIT_DECLARE_MSPriorityQueue
        CDSUNIT_DECLARE_EllenBinTree
        CDSUNIT_DECLARE_SkipList
        CDSUNIT_DECLARE_FCPriorityQueue
        CDSUNIT_DECLARE_StdPQueue

        CPPUNIT_TEST_SUITE(PQueue_Push)
            CDSUNIT_TEST_MSPriorityQueue
            CDSUNIT_TEST_EllenBinTree
            CDSUNIT_TEST_SkipList
            CDSUNIT_TEST_FCPriorityQueue
            CDUNIT_TEST_StdPQueue
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(pqueue::PQueue_Push);
