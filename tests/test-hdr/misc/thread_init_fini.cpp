//$$CDS-header$$

#include "cppunit/thread.h"
#include <cds/threading/model.h>

namespace {
    static size_t s_nThreadCount = 8;
    static size_t s_nPassCount = 1000000;
}

class thread_init_fini: public CppUnitMini::TestCase
{
    class Thread: public CppUnitMini::TestThread
    {
        virtual TestThread *    clone()
        {
            return new Thread( *this );
        }

    public:
        Thread( CppUnitMini::ThreadPool& pool )
            : CppUnitMini::TestThread( pool )
        {}
        Thread( Thread& src )
            : CppUnitMini::TestThread( src )
        {}

        thread_init_fini&  getTest()
        {
            return reinterpret_cast<thread_init_fini&>( m_Pool.m_Test );
        }

        virtual void init()
        {}
        virtual void fini()
        {}

        virtual void test()
        {
            for ( size_t i = 0; i < s_nPassCount; ++i ) {
                CPPUNIT_ASSERT(!cds::threading::Manager::isThreadAttached());
                if ( !cds::threading::Manager::isThreadAttached() )
                    cds::threading::Manager::attachThread();
                CPPUNIT_ASSERT( cds::threading::Manager::isThreadAttached() );
                cds::threading::Manager::detachThread();
            }
        }
    };

protected:
    void init_fini()
    {
        CPPUNIT_MSG( "Thread init/fini test,\n    thread count=" << s_nThreadCount << " pass count=" << s_nPassCount << "..." );

        CppUnitMini::ThreadPool pool( *this );

        pool.add( new Thread( pool ), s_nThreadCount );

        cds::OS::Timer    timer;
        timer.reset();

        pool.run();

        CPPUNIT_MSG( "   Duration=" << timer.duration() );
    }

    void setUpParams( const CppUnitMini::TestCfg& cfg ) {
        s_nThreadCount = cfg.getULong("ThreadCount", 8 );
        s_nPassCount = cfg.getULong("PassCount", 1000000 );
    }

    CPPUNIT_TEST_SUITE(thread_init_fini)
        CPPUNIT_TEST(init_fini);
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(thread_init_fini);
