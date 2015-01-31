//$$CDS-header$$

// Random allocator test

#include "alloc/michael_allocator.h"
#include "alloc/random_gen.h"

#include <cds/os/timer.h>
#include <cds/os/topology.h>

#include "cppunit/thread.h"

namespace memory {

    static size_t s_nMaxThreadCount = 32;
    static unsigned int s_nMinBlockSize = 8;
    static unsigned int s_nMaxBlockSize = 1024;
    //static size_t s_nBlocksPerThread = 1000;
    static size_t s_nPassCount = 100000;
    static size_t s_nDataSize = 1000;

    static size_t s_nPassPerThread;

    struct Item {
        cds::sync::spin m_access;
        char *          m_pszBlock;

        Item()
            : m_access( false )
            , m_pszBlock( nullptr )
        {}

        Item& operator =(Item const& i)
        {
            m_pszBlock = i.m_pszBlock;
            return *this;
        }
    };
    typedef std::vector<Item>   item_array;

#    define TEST_ALLOC(X, CLASS)        void X() { test< CLASS >(false); }
#    define TEST_ALLOC_STAT(X, CLASS)   void X() { test< CLASS >(true) ; }

    class Random_Alloc: public CppUnitMini::TestCase
    {
        item_array  m_Data;

        template <class ALLOC>
        class Thread: public CppUnitMini::TestThread
        {
            ALLOC&      m_Alloc;
            typedef typename ALLOC::value_type value_type;

            randomGen<size_t>   m_rndGen;

            virtual Thread *    clone()
            {
                return new Thread( *this );
            }

        public:
            Thread( CppUnitMini::ThreadPool& pool, ALLOC& a )
                : CppUnitMini::TestThread( pool )
                , m_Alloc( a )
            {}
            Thread( Thread& src )
                : CppUnitMini::TestThread( src )
                , m_Alloc( src.m_Alloc )
            {}

            Random_Alloc&  getTest()
            {
                return reinterpret_cast<Random_Alloc&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                item_array& arr = getTest().m_Data;
                for ( size_t nPass = 0; nPass < s_nPassPerThread; ) {
                    size_t nIdx = m_rndGen( size_t(0), s_nDataSize - 1 );
                    Item & item = arr.at(nIdx);
                    if ( item.m_access.try_lock() ) {
                        if ( item.m_pszBlock ) {
                            m_Alloc.deallocate( item.m_pszBlock, 1 );
                            item.m_pszBlock = nullptr;
                        }
                        else {
                            size_t nSize;
                            item.m_pszBlock = m_Alloc.allocate( nSize = m_rndGen( s_nMinBlockSize, s_nMaxBlockSize ), nullptr );

                            if ( nSize < 32 )
                                memset( item.m_pszBlock, 0, nSize );
                            else {
                                memset( item.m_pszBlock, 0, 16 );
                                memset( ((char *) item.m_pszBlock) + nSize * sizeof(*item.m_pszBlock) - 16, 0, 16 );
                            }
                        }
                        item.m_access.unlock();

                        ++nPass;
                    }
                }
            }
        };

        template <class ALLOC>
        void test( size_t nThreadCount )
        {
            ALLOC alloc;

            CPPUNIT_MSG( "Thread count=" << nThreadCount );
            s_nPassPerThread = s_nPassCount / nThreadCount;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread<ALLOC>( pool, alloc ), nThreadCount );

            cds::OS::Timer    timer;
            pool.run();
            CPPUNIT_MSG( "  Duration=" << pool.avgDuration() );

            for ( size_t i = 0; i < m_Data.size(); ++i ) {
                if ( m_Data[i].m_pszBlock ) {
                    alloc.deallocate( m_Data[i].m_pszBlock, 1 );
                    m_Data[i].m_pszBlock = nullptr;
                }
            }
        }

        template <class ALLOC>
        void test( bool bStat )
        {
            CPPUNIT_MSG( "Block size=" << s_nMinBlockSize << "-" << s_nMaxBlockSize
                << ", pass count=" << s_nPassCount << ", data size=" << s_nDataSize );

            m_Data.resize( s_nDataSize );

            for ( size_t nThreadCount = 2; nThreadCount <= s_nMaxThreadCount; nThreadCount *= 2 ) {
                summary_stat    stBegin;
                if ( bStat )
                    ALLOC::stat( stBegin );

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

            m_Data.resize(0);
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg )
        {
            s_nDataSize = cfg.getULong( "DataSize", 1000 );
            s_nPassCount = cfg.getULong( "PassCount", 100000 );
            s_nMinBlockSize = cfg.getUInt( "MinBlockSize", 8 );
            s_nMaxBlockSize = cfg.getUInt( "MaxBlockSize", 1024 );
            s_nMaxThreadCount = cfg.getUInt( "MaxThreadCount", 32 );
            if ( s_nMaxThreadCount == 0 )
                s_nMaxThreadCount = cds::OS::topology::processor_count() * 2;
            if ( s_nMaxThreadCount < 2 )
                s_nMaxThreadCount = 2;
        }

        typedef MichaelAlignHeap_Stat<char, 32>     t_MichaelAlignHeap_Stat;
        typedef MichaelAlignHeap_NoStat<char,32>    t_MichaelAlignHeap_NoStat;
        typedef system_aligned_allocator<char, 32>  t_system_aligned_allocator;

        TEST_ALLOC_STAT( michael_heap_stat, MichaelHeap_Stat<char> )
        TEST_ALLOC( michael_heap_nostat,    MichaelHeap_NoStat<char> )
        TEST_ALLOC( std_alloc,              std_allocator<char> )

        TEST_ALLOC_STAT( michael_alignheap_stat,t_MichaelAlignHeap_Stat )
        TEST_ALLOC( michael_alignheap_nostat,   t_MichaelAlignHeap_NoStat )
        TEST_ALLOC( system_aligned_alloc,       t_system_aligned_allocator )

        CPPUNIT_TEST_SUITE( Random_Alloc )
            CPPUNIT_TEST( michael_heap_stat )
            CPPUNIT_TEST( michael_heap_nostat )
            CPPUNIT_TEST( std_alloc )

            CPPUNIT_TEST( system_aligned_alloc )
            CPPUNIT_TEST( michael_alignheap_stat )
            CPPUNIT_TEST( michael_alignheap_nostat )

        CPPUNIT_TEST_SUITE_END();
    };

}   // namespace memory
CPPUNIT_TEST_SUITE_REGISTRATION( memory::Random_Alloc );
