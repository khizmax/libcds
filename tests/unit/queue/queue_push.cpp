/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#include "cppunit/thread.h"
#include "queue/queue_type.h"
#include "queue/queue_defs.h"


// Multi-threaded queue test for push operation
namespace queue {

#define TEST_CASE( Q, V )       void Q() { test< Types<V>::Q >(); }
#define TEST_BOUNDED( Q, V )    void Q() { test_bounded< Types<V>::Q >(); }
#define TEST_SEGMENTED( Q, V )  void Q() { test_segmented< Types<V>::Q >(); }

    namespace ns_Queue_Push {
        static size_t s_nThreadCount = 8;
        static size_t s_nQueueSize = 20000000 ;   // no more than 20 million records

        struct SimpleValue {
            size_t      nNo;

            SimpleValue(): nNo(0) {}
            SimpleValue( size_t n ): nNo(n) {}
            size_t getNo() const { return  nNo; }
        };
    }
    using namespace ns_Queue_Push;

    class Queue_Push: public CppUnitMini::TestCase
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
            size_t              m_nStartItem;
            size_t              m_nEndItem;
            size_t              m_nPushError;

        public:
            Thread( CppUnitMini::ThreadPool& pool, Queue& q )
                : CppUnitMini::TestThread( pool )
                , m_Queue( q )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Queue( src.m_Queue )
            {}

            Queue_Push&  getTest()
            {
                return reinterpret_cast<Queue_Push&>( m_Pool.m_Test );
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
                for ( size_t nItem = m_nStartItem; nItem < m_nEndItem; ++nItem ) {
                    if ( !m_Queue.push( nItem ))
                        ++m_nPushError;
                }

                m_fTime = m_Timer.duration() - m_fTime;
            }
        };

    protected:
        template <class Queue>
        void analyze( CppUnitMini::ThreadPool& pool, Queue& testQueue  )
        {
            size_t nThreadItems = s_nQueueSize / s_nThreadCount;
            double fTime = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Queue> * pThread = reinterpret_cast<Thread<Queue> *>(*it);
                fTime += pThread->m_fTime;
                if ( pThread->m_nPushError != 0 )
                    CPPUNIT_MSG("     ERROR: thread push error count=" << pThread->m_nPushError );
            }
            CPPUNIT_MSG( "     Duration=" << (fTime / s_nThreadCount) );
            CPPUNIT_CHECK( !testQueue.empty() )

            size_t * arr = new size_t[ s_nQueueSize ];
            memset(arr, 0, sizeof(arr[0]) * s_nQueueSize );

            cds::OS::Timer      timer;
            CPPUNIT_MSG( "   Pop (single-threaded)..." );
            size_t nPopped = 0;
            SimpleValue val = SimpleValue();
            while ( testQueue.pop( val )) {
                nPopped++;
                ++arr[ val.getNo() ];
            }
            CPPUNIT_MSG( "     Duration=" << timer.duration() );

            size_t nTotalItems = nThreadItems * s_nThreadCount;
            size_t nError = 0;
            for ( size_t i = 0; i < nTotalItems; ++i ) {
                if ( arr[i] != 1 ) {
                    CPPUNIT_MSG( "   ERROR: Item " << i << " has not been pushed" );
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

            size_t nStart = 0;
            size_t nThreadItemCount = s_nQueueSize / s_nThreadCount;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Queue> * pThread = reinterpret_cast<Thread<Queue> *>(*it);
                pThread->m_nStartItem = nStart;
                nStart += nThreadItemCount;
                pThread->m_nEndItem = nStart;
            }

            CPPUNIT_MSG( "   Push test, thread count=" << s_nThreadCount << " ...");
            pool.run();

            analyze( pool, testQueue );

            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <class Queue>
        void test_bounded()
        {
            size_t nStart = 0;
            size_t nThreadItemCount = s_nQueueSize / s_nThreadCount;

            Queue testQueue( s_nQueueSize );

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<Queue>( pool, testQueue ), s_nThreadCount );

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread<Queue> * pThread = reinterpret_cast<Thread<Queue> *>(*it);
                pThread->m_nStartItem = nStart;
                nStart += nThreadItemCount;
                pThread->m_nEndItem = nStart;
            }

            CPPUNIT_MSG( "   Push test, thread count=" << s_nThreadCount << " ...");
            pool.run();

            analyze( pool, testQueue );

            CPPUNIT_MSG( testQueue.statistics() );
        }

        template <class Queue>
        void test_segmented()
        {
            CPPUNIT_MSG( "   Push test, thread count=" << s_nThreadCount << " ...");
            for ( size_t nSegmentSize = 4; nSegmentSize <= 256; nSegmentSize *= 4 ) {
                CPPUNIT_MSG( "Segment size: " << nSegmentSize );

                Queue testQueue( nSegmentSize );

                CppUnitMini::ThreadPool pool( *this );
                pool.add( new Thread<Queue>( pool, testQueue ), s_nThreadCount );

                size_t nStart = 0;
                size_t nThreadItemCount = s_nQueueSize / s_nThreadCount;
                for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                    Thread<Queue> * pThread = reinterpret_cast<Thread<Queue> *>(*it);
                    pThread->m_nStartItem = nStart;
                    nStart += nThreadItemCount;
                    pThread->m_nEndItem = nStart;
                }

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
        CDSUNIT_DECLARE_TsigasCycleQueue( SimpleValue )
        CDSUNIT_DECLARE_VyukovMPMCCycleQueue( SimpleValue )
        CDSUNIT_DECLARE_StdQueue( SimpleValue )

        CPPUNIT_TEST_SUITE(Queue_Push)
            CDSUNIT_TEST_MoirQueue
            CDSUNIT_TEST_MSQueue
            CDSUNIT_TEST_OptimisticQueue
            CDSUNIT_TEST_BasketQueue
            CDSUNIT_TEST_FCQueue
            CDSUNIT_TEST_FCDeque
            CDSUNIT_TEST_SegmentedQueue
            CDSUNIT_TEST_RWQueue
            CDSUNIT_TEST_TsigasCycleQueue
            CDSUNIT_TEST_VyukovMPMCCycleQueue
            CDSUNIT_TEST_StdQueue
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(queue::Queue_Push);
