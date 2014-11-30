//$$CDS-header$$

// Hoard threadtest allocator test

#include "alloc/michael_allocator.h"

#include <cds/os/timer.h>
#include <cds/os/topology.h>

#include "cppunit/thread.h"

namespace memory {

    static size_t s_nPassCount = 1000;
    static size_t s_nBlockCount = 10000;
    static size_t s_nMinBlockSize = 16;
    static size_t s_nMaxBlockSize = 1024;
    static size_t s_nMaxThreadCount = 32;

    static size_t s_nPassPerThread;

    static size_t s_nCurBlockSize;

#    define TEST_ALLOC(X, CLASS)            void X() { test< CLASS >( false )    ; }
#    define TEST_ALLOC_STAT(X, CLASS)       void X() { test< CLASS >( true )    ; }

    /*
        In Threadtest, each thread performs s_nPassCount iterations of allocating s_nBlockCount
        s_nCurBlockSize-byte blocks and then freeing them in order. The benchmarks capture
        allocator latency and scalability under regular private allocation patterns.
    */
    class Hoard_ThreadTest: public CppUnitMini::TestCase
    {
        template <class ALLOC>
        class Thread: public CppUnitMini::TestThread
        {
            ALLOC&      m_Alloc;
            typedef typename ALLOC::value_type  value_type;
            value_type **     m_arr;

            virtual Thread *    clone()
            {
                return new Thread( *this );
            }
        public:

        public:
            Thread( CppUnitMini::ThreadPool& pool, ALLOC& a )
                : CppUnitMini::TestThread( pool )
                , m_Alloc( a )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Alloc( src.m_Alloc )
            {}

            Hoard_ThreadTest&  getTest()
            {
                return reinterpret_cast<Hoard_ThreadTest&>( m_Pool.m_Test );
            }

            virtual void init()
            {
                cds::threading::Manager::attachThread();
                m_arr = new value_type *[s_nBlockCount];
            }
            virtual void fini()
            {
                delete [] m_arr;
                cds::threading::Manager::detachThread();
            }

            virtual void test()
            {
                size_t nSize = s_nCurBlockSize / sizeof(value_type);
                for ( size_t nPass = 0; nPass < s_nPassPerThread; ++nPass ) {
                    value_type ** pCell = m_arr;
                    for ( size_t i = 0; i < s_nBlockCount; ++i, ++pCell ) {
                        *pCell = m_Alloc.allocate( nSize, nullptr );
                        CPPUNIT_ASSERT( *pCell != nullptr );

                        if ( nSize < 32 )
                            memset( *pCell, 0, nSize );
                        else {
                            memset( *pCell, 0, 16 );
                            memset( ((char *)(*pCell)) + nSize * sizeof(value_type) - 16, 0, 16 );
                        }

                        CPPUNIT_ASSERT( (reinterpret_cast<uintptr_t>(*pCell) & (ALLOC::alignment - 1)) == 0 );
                    }
                    pCell = m_arr;
                    for ( size_t i = 0; i < s_nBlockCount; ++i, ++pCell ) {
                        m_Alloc.deallocate( *pCell, 1 );
                    }
                }
            }
        };

        template <class ALLOC>
        void test( size_t nThreadCount )
        {
            ALLOC alloc;
            cds::OS::Timer    timer;

            CPPUNIT_MSG( "Thread count=" << nThreadCount );
            s_nPassPerThread = s_nPassCount / nThreadCount;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<ALLOC>( pool, alloc ), nThreadCount );
            pool.run();
            CPPUNIT_MSG( "  Duration=" << pool.avgDuration() );
        }

        template <class ALLOC>
        void test( bool bStat)
        {
            s_nCurBlockSize = s_nMinBlockSize;
            while ( s_nCurBlockSize <= s_nMaxBlockSize ) {
                CPPUNIT_MSG( "Block size=" << s_nCurBlockSize << ", block count=" << s_nBlockCount << ", pass count=" << s_nPassCount << " per thread" );
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
                s_nCurBlockSize *= 2;
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg )
        {
            s_nPassCount = cfg.getULong( "PassCount", 1000 );
            s_nBlockCount = cfg.getULong( "BlockCount", 10000 );
            s_nMinBlockSize = cfg.getULong( "MinBlockSize", 16 );
            s_nMaxBlockSize = cfg.getULong( "MaxBlockSize", 1024 );
            s_nMaxThreadCount = cfg.getUInt( "MaxThreadCount", 32 );
            if ( s_nMaxThreadCount == 0 )
                s_nMaxThreadCount = cds::OS::topology::processor_count() * 2;
        }

        typedef MichaelAlignHeap_Stat<int, 64>      t_MichaelAlignHeap_Stat;
        typedef MichaelAlignHeap_NoStat<int,64>     t_MichaelAlignHeap_NoStat;
        typedef system_aligned_allocator<int, 64>   t_system_aligned_allocator;

        TEST_ALLOC_STAT( michael_heap_stat,      MichaelHeap_Stat<int> )
        TEST_ALLOC( michael_heap_nostat,    MichaelHeap_NoStat<int> )
        TEST_ALLOC( std_alloc,              std_allocator<int> )

        TEST_ALLOC_STAT( michael_alignheap_stat,     t_MichaelAlignHeap_Stat )
        TEST_ALLOC( michael_alignheap_nostat,   t_MichaelAlignHeap_NoStat )
        TEST_ALLOC( system_aligned_alloc,       t_system_aligned_allocator )

        CPPUNIT_TEST_SUITE( Hoard_ThreadTest )
            CPPUNIT_TEST( std_alloc )
            CPPUNIT_TEST( michael_heap_stat )
            CPPUNIT_TEST( michael_heap_nostat )

            CPPUNIT_TEST( system_aligned_alloc )
            CPPUNIT_TEST( michael_alignheap_stat )
            CPPUNIT_TEST( michael_alignheap_nostat )

        CPPUNIT_TEST_SUITE_END();
    };

}   // namespace memory
CPPUNIT_TEST_SUITE_REGISTRATION( memory::Hoard_ThreadTest );
