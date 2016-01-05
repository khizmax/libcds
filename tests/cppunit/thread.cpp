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

#include <chrono>
#include <cds/details/defs.h> // TSan annotations
#include "cppunit/thread.h"

namespace CppUnitMini {

    void TestThread::threadEntryPoint( TestThread * pInst )
    {
        pInst->run();
    }

    void TestThread::create()
    {
        m_pThread = new boost::thread( threadEntryPoint, this );
    }

    void TestThread::run()
    {
        try {
            init();
            m_Pool.onThreadInitDone( this );

            test();
            m_Pool.onThreadTestDone( this );

            fini();
            m_Pool.onThreadFiniDone( this );
        }
        catch ( std::exception& ex )
        {
            m_Pool.m_Test.message( "EXCEPTION in working thread: ");
            m_Pool.m_Test.message( ex.what() );
        }
    }

    void TestThread::error(const char *in_macroName, const char *in_macro, const char *in_file, int in_line)
    {
        m_Pool.m_Test.error( in_macroName, in_macro, in_file, in_line );
    }


    ThreadPool::~ThreadPool()
    {
        delete m_pBarrierStart;
        delete m_pBarrierDone;

        for ( size_t i = 0; i < m_arrThreads.size(); ++i )
            delete m_arrThreads[i];
        m_arrThreads.resize( 0 );
    }

    void    ThreadPool::add( TestThread * pThread, size_t nCount )
    {
        pThread->m_nThreadNo = m_arrThreads.size();
        m_arrThreads.push_back( pThread );
        while ( --nCount ) {
            TestThread * p = pThread->clone();
            if ( p ) {
                p->m_nThreadNo = m_arrThreads.size();
                m_arrThreads.push_back( p );
            }
        }
    }

    void    ThreadPool::run()
    {
        const size_t nThreadCount = m_arrThreads.size();
        m_pBarrierStart = new boost::barrier( (unsigned int) nThreadCount );
        // nThreadCount threads + current thread
        m_pBarrierDone = new boost::barrier( (unsigned int) (nThreadCount + 1) );

        for ( size_t i = 0; i < nThreadCount; ++i )
            m_arrThreads[i]->create();

        // Wait while all threads is done
        m_pBarrierDone->wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void ThreadPool::run( unsigned int nDuration )
    {
        const size_t nThreadCount = m_arrThreads.size();
        m_pBarrierStart = new boost::barrier( (unsigned int) nThreadCount );
        m_pBarrierDone = new boost::barrier( (unsigned int) (nThreadCount + 1) );

        for ( size_t i = 0; i < nThreadCount; ++i )
            m_arrThreads[i]->create();

        auto stEnd(std::chrono::steady_clock::now() + std::chrono::seconds( nDuration ));
        do {
            std::this_thread::sleep_until( stEnd );
        } while ( std::chrono::steady_clock::now() < stEnd );

        for ( size_t i = 0; i < nThreadCount; ++i )
            m_arrThreads[i]->stop();

        // Wait while all threads is done
        m_pBarrierDone->wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void    ThreadPool::onThreadInitDone( TestThread * pThread )
    {
        // Calls in context of caller thread
        // Wait while all threads started
        m_pBarrierStart->wait();

        pThread->m_Timer.reset();
    }

    void    ThreadPool::onThreadTestDone( TestThread * pThread )
    {
        // Calls in context of caller thread
        pThread->m_nDuration = pThread->m_Timer.duration();
    }

    void    ThreadPool::onThreadFiniDone( TestThread * /*pThread*/ )
    {
        // Calls in context of caller thread
        // Wait while all threads done
        m_pBarrierDone->wait();
    }
}
