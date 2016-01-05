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

#include <cds/lock/spinlock.h>

// Multi-threaded stack test for push operation
namespace lock {

#define TEST_CASE( N, L )   void N() { test<L>(); }

    namespace {
        static size_t s_nThreadCount = 8;
        static size_t s_nLoopCount = 1000000    ;     // loop count per thread

        static size_t   s_nSharedInt;
    }

    class Spinlock_MT: public CppUnitMini::TestCase
    {
        template <class LOCK>
        class Thread: public CppUnitMini::TestThread
        {
            virtual TestThread *    clone()
            {
                return new Thread( *this );
            }
        public:
            LOCK&               m_Lock;
            double              m_fTime;

        public:
            Thread( CppUnitMini::ThreadPool& pool, LOCK& l )
                : CppUnitMini::TestThread( pool )
                , m_Lock( l )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Lock( src.m_Lock )
            {}

            Spinlock_MT&  getTest()
            {
                return reinterpret_cast<Spinlock_MT&>( m_Pool.m_Test );
            }

            virtual void init()
            {}
            virtual void fini()
            {}

            virtual void test()
            {
                m_fTime = m_Timer.duration();

                for ( size_t i  = 0; i < s_nLoopCount; ++i ) {
                    m_Lock.lock();
                    ++s_nSharedInt;
                    m_Lock.unlock();
                }

                m_fTime = m_Timer.duration() - m_fTime;
            }
        };

    protected:
        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            s_nThreadCount = cfg.getULong("ThreadCount", 8 );
            s_nLoopCount = cfg.getULong("LoopCount", 1000000 );
        }

        template <class LOCK>
        void test()
        {
            LOCK    testLock;

            for ( size_t nThreadCount = 1; nThreadCount <= s_nThreadCount; nThreadCount *= 2 ) {
                s_nSharedInt = 0;

                CppUnitMini::ThreadPool pool( *this );
                pool.add( new Thread<LOCK>( pool, testLock ), nThreadCount );

                CPPUNIT_MSG( "   Lock test, thread count=" << nThreadCount
                    << " loop per thread=" << s_nLoopCount
                    << "...");
                cds::OS::Timer      timer;
                pool.run();
                CPPUNIT_MSG( "     Duration=" << timer.duration() );

                CPPUNIT_ASSERT_EX( s_nSharedInt == nThreadCount * s_nLoopCount,
                    "Expected=" << nThreadCount * s_nLoopCount
                    << " real=" << s_nSharedInt );
            }
        }

        typedef cds::sync::spin_lock<cds::backoff::exponential<cds::backoff::hint, cds::backoff::yield> > Spinlock_exp;

        typedef cds::sync::reentrant_spin_lock<unsigned int, cds::backoff::exponential<cds::backoff::hint, cds::backoff::yield> > reentrantSpin_exp;
        typedef cds::sync::reentrant_spin_lock<unsigned int, cds::backoff::yield> reentrantSpin_yield;
        typedef cds::sync::reentrant_spin_lock<unsigned int, cds::backoff::hint> reentrantSpin_hint;
        typedef cds::sync::reentrant_spin_lock<unsigned int, cds::backoff::empty> reentrantSpin_empty;

        TEST_CASE(spinLock_exp,         Spinlock_exp );
        TEST_CASE(spinLock_yield,       cds::sync::spin_lock<cds::backoff::yield> );
        TEST_CASE(spinLock_hint,        cds::sync::spin_lock<cds::backoff::hint> );
        TEST_CASE(spinLock_empty,       cds::sync::spin_lock<cds::backoff::empty> );
        TEST_CASE( spinLock_yield_lock, cds::lock::Spinlock<cds::backoff::yield> );
        TEST_CASE( spinLock_hint_lock, cds::lock::Spinlock<cds::backoff::hint> );
        TEST_CASE( spinLock_empty_lock, cds::lock::Spinlock<cds::backoff::empty> );

        TEST_CASE( reentrantSpinLock32, cds::sync::reentrant_spin32 );
        TEST_CASE( reentrantSpinLock64, cds::sync::reentrant_spin64 );
        TEST_CASE( reentrantSpinLock32_lock, cds::lock::ReentrantSpin32 );
        TEST_CASE( reentrantSpinLock64_lock, cds::lock::ReentrantSpin64 );

        TEST_CASE(reentrantSpinlock_exp,        reentrantSpin_exp );
        TEST_CASE(reentrantSpinlock_yield,      reentrantSpin_yield );
        TEST_CASE(reentrantSpinlock_hint,       reentrantSpin_hint );
        TEST_CASE(reentrantSpinlock_empty,      reentrantSpin_empty );

    protected:
        CPPUNIT_TEST_SUITE(Spinlock_MT)
            CPPUNIT_TEST(spinLock_exp);
            CPPUNIT_TEST(spinLock_yield);
            CPPUNIT_TEST(spinLock_hint);
            CPPUNIT_TEST(spinLock_empty);

            CPPUNIT_TEST(reentrantSpinLock32);
            CPPUNIT_TEST(reentrantSpinLock64);

            CPPUNIT_TEST(reentrantSpinlock_exp)
            CPPUNIT_TEST(reentrantSpinlock_yield)
            CPPUNIT_TEST(reentrantSpinlock_hint)
            CPPUNIT_TEST(reentrantSpinlock_empty)
        CPPUNIT_TEST_SUITE_END();
    };

} // namespace lock

CPPUNIT_TEST_SUITE_REGISTRATION(lock::Spinlock_MT);
