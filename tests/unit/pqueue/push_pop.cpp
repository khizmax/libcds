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
        static size_t s_nPushThreadCount = 4;
        static size_t s_nPopThreadCount = 4;
        static size_t s_nQueueSize = 2000000;
    }
} // namespace pqueue

namespace pqueue {

    class PQueue_PushPop: public CppUnitMini::TestCase
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

            PQueue_PushPop&  getTest()
            {
                return static_cast<PQueue_PushPop&>( m_Pool.m_Test );
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

                getTest().end_pusher();
            }

            void prepare( size_t nStart, size_t nEnd )
            {
                m_arr.reserve( nEnd - nStart );
                for ( size_t i = nStart; i < nEnd; ++i )
                    m_arr.push_back( i );
                shuffle( m_arr.begin(), m_arr.end() );
            }
        };

        template <class PQueue>
        class Popper: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Popper( *this );
            }
        public:
            PQueue&             m_Queue;
            size_t              m_nPopSuccess;
            size_t              m_nPopFailed;

            typedef std::vector<size_t> array_type;
            array_type          m_arr;

        public:
            Popper( CppUnitMini::ThreadPool& pool, PQueue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {}
            Popper( Popper& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {}

            PQueue_PushPop&  getTest()
            {
                return static_cast<PQueue_PushPop&>( m_Pool.m_Test );
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
                m_nPopSuccess = 0;
                m_nPopFailed = 0;

                SimpleValue val;
                while ( getTest().pushing() || !m_Queue.empty() ) {
                    if ( m_Queue.pop( val ))
                        ++m_nPopSuccess;
                    else
                        ++m_nPopFailed;
                }
            }
        };

        size_t  m_nPusherCount;
        void end_pusher()
        {
            --m_nPusherCount;
        }
        bool pushing() const
        {
            return m_nPusherCount != 0;
        }

    protected:
        template <class PQueue>
        void test()
        {
            PQueue testQueue;
            test_with( testQueue );
        }

        template <class PQueue>
        void test_bounded()
        {
            std::unique_ptr<PQueue> pq( new PQueue(s_nQueueSize) );
            test_with( *pq.get() );
        }

        template <class PQueue>
        void test_with( PQueue& testQueue )
        {
            size_t const nThreadItemCount = s_nQueueSize / s_nPushThreadCount;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Pusher<PQueue>( pool, testQueue ), s_nPushThreadCount );

            size_t nStart = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Pusher<PQueue> * pThread = static_cast<Pusher<PQueue> *>(*it);
                pThread->prepare( nStart, nStart + nThreadItemCount );
                nStart += nThreadItemCount;
            }

            pool.add( new Popper<PQueue>( pool, testQueue ), s_nPopThreadCount );

            m_nPusherCount = s_nPushThreadCount;
            CPPUNIT_MSG( "   push thread count=" << s_nPushThreadCount << " pop thread count=" << s_nPopThreadCount
                << ", item count=" << nThreadItemCount * s_nPushThreadCount << " ..." );
            pool.run();
            CPPUNIT_MSG( "     Duration=" << pool.avgDuration() );

            // Analyze result
            size_t nTotalPopped = 0;
            size_t nPushFailed = 0;
            size_t nPopFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Popper<PQueue> * pPopper = dynamic_cast<Popper<PQueue> *>(*it);
                if ( pPopper ) {
                    nTotalPopped += pPopper->m_nPopSuccess;
                    nPopFailed += pPopper->m_nPopFailed;
                }
                else {
                    Pusher<PQueue> * pPusher = dynamic_cast<Pusher<PQueue> *>(*it);
                    assert( pPusher );
                    nPushFailed += pPusher->m_nPushError;
                }
            }

            CPPUNIT_MSG( "   Total: popped=" << nTotalPopped << ", empty pop=" << nPopFailed << ", push error=" << nPushFailed );
            CPPUNIT_CHECK( nTotalPopped == nThreadItemCount * s_nPushThreadCount );
            CPPUNIT_CHECK( nPushFailed == 0 );

            check_statistics( testQueue.statistics() );
            CPPUNIT_MSG( testQueue.statistics() );
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nPushThreadCount = cfg.getULong("PushThreadCount", (unsigned long) s_nPushThreadCount );
            s_nPopThreadCount = cfg.getULong("PopThreadCount", (unsigned long) s_nPopThreadCount );
            s_nQueueSize = cfg.getULong("QueueSize", (unsigned long) s_nQueueSize );
        }

    protected:
#include "pqueue/pqueue_defs.h"
        CDSUNIT_DECLARE_MSPriorityQueue
        CDSUNIT_DECLARE_EllenBinTree
        CDSUNIT_DECLARE_SkipList
        CDSUNIT_DECLARE_FCPriorityQueue
        CDSUNIT_DECLARE_StdPQueue

        CPPUNIT_TEST_SUITE(PQueue_PushPop)
            CDSUNIT_TEST_MSPriorityQueue
            CDSUNIT_TEST_EllenBinTree
            CDSUNIT_TEST_SkipList
            CDSUNIT_TEST_FCPriorityQueue
            CDUNIT_TEST_StdPQueue
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(pqueue::PQueue_PushPop);
