//$$CDS-header$$

// Linux scalability allocator test

#include "alloc/michael_allocator.h"

#include <cds/os/timer.h>
#include <cds/os/topology.h>

#include "cppunit/thread.h"

namespace memory {

    static size_t s_nPassCount = 10000000;
    static size_t s_nMaxBlockSize = 64 * 1024 - 16;
    static size_t s_nMaxThreadCount = 64;

    static size_t s_nPassPerThread;

#    define TEST_ALLOC(X, CLASS)    void X() { test< CLASS >(false)     ; }
#    define TEST_ALLOC_STAT(X, CLASS)    void X() { test< CLASS >(true) ; }

    class Linux_Scale: public CppUnitMini::TestCase
    {
        template <class ALLOC>
        class Thread: public CppUnitMini::TestThread
        {
            ALLOC&      m_Alloc;
            size_t      m_nSize;

            virtual Thread *    clone()
            {
                return new Thread( *this );
            }
        public:

        public:
            Thread( CppUnitMini::ThreadPool& pool, ALLOC& a, size_t nSize )
                : CppUnitMini::TestThread( pool )
                , m_Alloc( a )
                , m_nSize( nSize )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Alloc( src.m_Alloc )
                , m_nSize( src.m_nSize )
            {}

            Linux_Scale&  getTest()
            {
                return reinterpret_cast<Linux_Scale&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                for ( size_t i = 0; i < s_nPassPerThread; ++i ) {
                    typename ALLOC::value_type * p = m_Alloc.allocate( m_nSize / sizeof( typename ALLOC::value_type ), nullptr );
                    CPPUNIT_ASSERT( p != nullptr );
                    if ( m_nSize < 32 )
                        memset( p, 0, m_nSize );
                    else {
                        memset( p, 0, 16 );
                        memset( ((char *)p) + m_nSize * sizeof(*p) - 16, 0, 16 );
                    }
                    CPPUNIT_ASSERT( (reinterpret_cast<uintptr_t>(p) & (ALLOC::alignment - 1)) == 0 );
                    m_Alloc.deallocate( p, 1 );
                }
            }
        };

        template <class ALLOC>
        void test( size_t nThreadCount, size_t nSize )
        {
            cds::OS::Timer    timer;

            ALLOC alloc;

            CPPUNIT_MSG( "   Block size=" << nSize );
            s_nPassPerThread = s_nPassCount / nThreadCount;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<ALLOC>( pool, alloc, nSize ), nThreadCount );
            pool.run();
            CPPUNIT_MSG( "      Duration=" << pool.avgDuration() );
        }

        template <class ALLOC>
        void test( size_t nThreadCount )
        {
            CPPUNIT_MSG( "Thread count=" << nThreadCount );
            for ( size_t sz = 1; sz < s_nMaxBlockSize; sz *= 2 ) {
                test<ALLOC>( nThreadCount, sz );
            }
        }

        template <class ALLOC>
        void test( bool bStat )
        {
            for ( size_t nThreadCount = 1; nThreadCount <= s_nMaxThreadCount; nThreadCount *= 2 ) {
                summary_stat stBegin;
                if ( bStat )
                    ALLOC::stat(stBegin);

                test<ALLOC>( nThreadCount );

                summary_stat    stEnd;
                if ( bStat ) {
                    ALLOC::stat( stEnd );

                    std::cout << "\nStatistics:\n"
                        << stEnd
;
                    stEnd -= stBegin;
                    std::cout << "\nDelta statistics:\n"
                        << stEnd
;
                }
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg )
        {
            s_nPassCount = cfg.getULong( "PassCount", 10000000 );
            s_nMaxBlockSize = cfg.getULong( "MaxBlockSize", 64 * 1024 - 16 );
            s_nMaxThreadCount = cfg.getUInt( "MaxThreadCount", 64 );
            if ( s_nMaxThreadCount == 0 )
                s_nMaxThreadCount = cds::OS::topology::processor_count() * 2;
        }

        typedef MichaelAlignHeap_Stat<char, 64>      t_MichaelAlignHeap_Stat;
        typedef MichaelAlignHeap_NoStat<char,64>     t_MichaelAlignHeap_NoStat;
        typedef system_aligned_allocator<char, 64>   t_system_aligned_allocator;

        TEST_ALLOC_STAT( michael_heap_stat,      MichaelHeap_Stat<char> )
        TEST_ALLOC( michael_heap_nostat,    MichaelHeap_NoStat<char> )
        TEST_ALLOC( std_alloc,              std_allocator<char> )

        TEST_ALLOC_STAT( michael_alignheap_stat,     t_MichaelAlignHeap_Stat )
        TEST_ALLOC( michael_alignheap_nostat,   t_MichaelAlignHeap_NoStat )
        TEST_ALLOC( system_aligned_alloc,       t_system_aligned_allocator )

        CPPUNIT_TEST_SUITE( Linux_Scale )
            CPPUNIT_TEST( michael_heap_nostat )
            CPPUNIT_TEST( michael_heap_stat )
            CPPUNIT_TEST( std_alloc )

            CPPUNIT_TEST( system_aligned_alloc )
            CPPUNIT_TEST( michael_alignheap_stat )
            CPPUNIT_TEST( michael_alignheap_nostat )
        CPPUNIT_TEST_SUITE_END();
    };

}   // namespace memory
CPPUNIT_TEST_SUITE_REGISTRATION( memory::Linux_Scale );
