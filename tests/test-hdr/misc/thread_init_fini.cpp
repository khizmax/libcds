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
