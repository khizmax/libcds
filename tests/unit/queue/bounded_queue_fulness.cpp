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

        CPPUNIT_TEST_SUITE( BoundedQueue_Fullness )
            CDSUNIT_TEST_TsigasCycleQueue
            CDSUNIT_TEST_VyukovMPMCCycleQueue
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(queue::BoundedQueue_Fullness );
