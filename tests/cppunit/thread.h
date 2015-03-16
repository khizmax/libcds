//$$CDS-header$$

#ifndef CDS_CPPUNIT_THREAD_H
#define CDS_CPPUNIT_THREAD_H

#include <cds/details/defs.h>
#include "cppunit/cppunit_mini.h"
#include <boost/thread.hpp>
#include <cds/os/timer.h>
#include <cds/threading/model.h>    // for attach/detach thread
#include <cds/algo/atomic.h>

// Visual leak detector (see http://vld.codeplex.com/)
#if defined(CDS_USE_VLD) && CDS_COMPILER == CDS_COMPILER_MSVC
#   ifdef _DEBUG
#       include <vld.h>
#   endif
#endif

namespace CppUnitMini {
    static inline unsigned int Rand( unsigned int nMax )
    {
        double rnd = double( rand() ) / double( RAND_MAX );
        unsigned int n = (unsigned int) (rnd * nMax);
        return n < nMax ? n : (n-1);
    }

    class ThreadPool;
    class TestThread
    {
    protected:
        typedef TestThread  Base;
        friend class ThreadPool;

        ThreadPool&         m_Pool;
        boost::thread *     m_pThread;
        cds::OS::Timer      m_Timer;
        atomics::atomic<bool>    m_bTimeElapsed;

    public:
        double              m_nDuration;
        size_t              m_nThreadNo;

    protected:
        static void threadEntryPoint( TestThread * pThread );

        TestThread( TestThread& src )
            : m_Pool( src.m_Pool )
            , m_pThread( nullptr )
            , m_bTimeElapsed( false )
            , m_nDuration( 0 )
            , m_nThreadNo( 0 )
        {}

        virtual ~TestThread()
        {
            if ( m_pThread )
                delete m_pThread;
        }

        virtual TestThread *    clone() = 0;

        void create();
        void run();

        virtual void init() {}
        virtual void test() = 0;
        virtual void fini() {}
        void stop()
        {
            m_bTimeElapsed.store( true, atomics::memory_order_release );
        }
        bool time_elapsed() const
        {
            return m_bTimeElapsed.load( atomics::memory_order_acquire );
        }

        bool check_timeout( size_t nMaxDuration )
        {
            return m_Timer.duration() > nMaxDuration;
        }

        void error(const char *in_macroName, const char *in_macro, const char *in_file, int in_line);

    public:
        TestThread( ThreadPool& pool )
            : m_Pool( pool )
            , m_pThread( nullptr )
            , m_nDuration( 0 )
            , m_nThreadNo( 0 )
        {}
    };

    class ThreadPool
    {
    public:
        TestCase&                       m_Test;

    private:
        typedef std::vector< TestThread * >     thread_vector;

        boost::thread_group             m_Pool;
        thread_vector                   m_arrThreads;

        boost::barrier * volatile       m_pBarrierStart;
        boost::barrier * volatile       m_pBarrierDone;

    public:
        typedef thread_vector::iterator    iterator;

    public:
        ThreadPool( TestCase& tc )
            : m_Test( tc )
            , m_pBarrierStart( nullptr )
            , m_pBarrierDone( nullptr )
        {}
        ~ThreadPool();

        void    add( TestThread * pThread, size_t nCount );

        void    run();
        void    run( unsigned int nDuration );

        void    onThreadInitDone( TestThread * pThread );
        void    onThreadTestDone( TestThread * pThread );
        void    onThreadFiniDone( TestThread * pThread );

        iterator begin() { return m_arrThreads.begin(); }
        iterator end()   { return m_arrThreads.end() ;   }

        double  avgDuration() const
        {
            double nDur = 0;
            for ( size_t i = 0; i < m_arrThreads.size(); ++i )
                nDur += m_arrThreads[i]->m_nDuration;
            return nDur / m_arrThreads.size();
        }
    };
}

#endif    // #ifndef CDS_CPPUNIT_THREAD_H
