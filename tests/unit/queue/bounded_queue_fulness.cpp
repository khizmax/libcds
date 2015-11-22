//$$CDS-header$$

#include "cppunit/thread.h"
#include "queue/queue_type.h"
#include "queue/queue_defs.h"


/*
    Bounded queue test.
    The test checks the behaviour of bounded queue when it is almost full.
    Many algorithms says the queue is full when it is not, and vice versa.
*/
namespace queue {

#define TEST_BOUNDED( Q, V )    void Q() { test< Types<V>::Q >(); }

    namespace ns_BoundedQueue_Fullness {
        static size_t s_nThreadCount = 8;
        static size_t s_nQueueSize = 1024;
        static size_t s_nPassCount = 1000000;
    }
    using namespace ns_BoundedQueue_Fullness;

    class VyukovMPMCCycleQueue_dyn_fair_ : public Types<size_t>::VyukovMPMCCycleQueue_dyn_ic
    {
        typedef Types<size_t>::VyukovMPMCCycleQueue_dyn_ic base_class;
    public:
        typedef base_class::value_type value_type;

        VyukovMPMCCycleQueue_dyn_fair_()
            : base_class()
        {}

        VyukovMPMCCycleQueue_dyn_fair_( size_t nCapacity )
            : base_class( nCapacity )
        {}

        bool enqueue( value_type const& data )
        {
            bool ret;
            do {
                ret = base_class::enqueue( data );
            } while ( !ret && size() != capacity() );
            return ret;
        }

        bool push( value_type const& data )
        {
            return enqueue( data );
        }

        bool dequeue( value_type& dest )
        {
            bool ret;
            do {
                ret = base_class::dequeue( dest );
            } while ( !ret && size() != capacity() );
            return ret;
        }

        bool pop( value_type& dest )
        {
            return dequeue( dest );
        }

        size_t size() const { return base_class::size(); }
        size_t capacity() const { return base_class::capacity(); }
    };

    class BoundedQueue_Fullness: public CppUnitMini::TestCase
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
            size_t              m_nPushError;
            size_t              m_nPopError;

        public:
            Thread( CppUnitMini::ThreadPool& pool, Queue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {}

            BoundedQueue_Fullness&  getTest()
            {
                return reinterpret_cast<BoundedQueue_Fullness&>( m_Pool.m_Test );
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
                m_fTime = m_Timer.duration();

                m_nPushError = 0;
                m_nPopError = 0;
                for ( size_t i = 0; i < s_nPassCount; ++i ) {
                    if ( !m_Queue.push( i ))
                        ++m_nPushError;
                    size_t item;
                    if ( !m_Queue.pop( item ))
                        ++m_nPopError;
                }
                m_fTime = m_Timer.duration() - m_fTime;
            }
        };

    protected:
        template <class Queue>
        void analyze( CppUnitMini::ThreadPool& pool, Queue& testQueue  )
        {
            double fTime = 0;
            size_t nPushError = 0;
            size_t nPopError = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Queue> * pThread = reinterpret_cast<Thread<Queue> *>(*it);
                fTime += pThread->m_fTime;
                nPushError += pThread->m_nPushError;
                nPopError  += pThread->m_nPopError;
            }
            CPPUNIT_MSG( "     Duration=" << (fTime / s_nThreadCount) );
            CPPUNIT_MSG( "     Errors: push=" << nPushError << ", pop=" << nPopError );
            CPPUNIT_CHECK( !testQueue.empty());
            CPPUNIT_CHECK( nPushError == 0 );
            CPPUNIT_CHECK( nPopError == 0 );
        }

        template <class Queue>
        void test()
        {
            Queue testQueue( s_nQueueSize );

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<Queue>( pool, testQueue ), s_nThreadCount );

            size_t nSize = testQueue.capacity() - s_nThreadCount;
            for ( size_t i = 0; i < nSize; ++i )
                testQueue.push( i );

            CPPUNIT_MSG( "   Thread count=" << s_nThreadCount << ", push/pop pairs=" << s_nPassCount 
                         << ", queue capacity=" << testQueue.capacity() << " ...");
            pool.run();

            analyze( pool, testQueue );

            CPPUNIT_MSG( testQueue.statistics() );
        }
        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nThreadCount = cfg.getULong("ThreadCount", 8 );
            s_nQueueSize = cfg.getULong("QueueSize", 1024 );
            s_nPassCount = cfg.getULong( "PassCount", 1000000 );
        }

    protected:
        CDSUNIT_DECLARE_TsigasCycleQueue( size_t )
        CDSUNIT_DECLARE_VyukovMPMCCycleQueue( size_t )
        void VyukovMPMCCycleQueue_dyn_fair()
        {
            test< VyukovMPMCCycleQueue_dyn_fair_ >();
        }

        CPPUNIT_TEST_SUITE( BoundedQueue_Fullness )
            CDSUNIT_TEST_TsigasCycleQueue
            CDSUNIT_TEST_VyukovMPMCCycleQueue
            CPPUNIT_TEST( VyukovMPMCCycleQueue_dyn_fair_ ) \
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(queue::BoundedQueue_Fullness );
