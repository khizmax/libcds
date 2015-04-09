//$$CDS-header$$

#include "map2/map_type.h"
#include "cppunit/thread.h"
#include <vector>

namespace map2 {

#   define TEST_MAP(IMPL, C, X)         void C::X() { test<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_NOLF(IMPL, C, X)    void C::X() { test_nolf<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_EXTRACT(IMPL, C, X)  TEST_MAP(IMPL, C, X)
#   define TEST_MAP_NOLF_EXTRACT(IMPL, C, X) TEST_MAP_NOLF(IMPL, C, X)

    class Map_InsDelFind: public CppUnitMini::TestCase
    {
        static size_t  c_nInitialMapSize;   // initial map size
        static size_t  c_nThreadCount;      // thread count
        static size_t  c_nMaxLoadFactor;    // maximum load factor
        static unsigned int c_nInsertPercentage;
        static unsigned int c_nDeletePercentage;
        static unsigned int c_nDuration;    // test duration, seconds
        static bool    c_bPrintGCState;

    public:
        enum actions
        {
            do_find,
            do_insert,
            do_delete
        };
        static const unsigned int c_nShuffleSize = 100;
        actions m_arrShuffle[c_nShuffleSize];

    protected:
        typedef CppUnitMini::TestCase Base;
        typedef size_t  key_type;
        typedef size_t  value_type;

        template <class MAP>
        class WorkThread: public CppUnitMini::TestThread
        {
            MAP&     m_Map;

            virtual WorkThread *    clone()
            {
                return new WorkThread( *this );
            }
        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;
            size_t  m_nFindSuccess;
            size_t  m_nFindFailed;

        public:
            WorkThread( CppUnitMini::ThreadPool& pool, MAP& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            WorkThread( WorkThread& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDelFind&  getTest()
            {
                return reinterpret_cast<Map_InsDelFind&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                MAP& rMap = m_Map;

                m_nInsertSuccess =
                    m_nInsertFailed =
                    m_nDeleteSuccess =
                    m_nDeleteFailed =
                    m_nFindSuccess =
                    m_nFindFailed = 0;

                actions * pAct = getTest().m_arrShuffle;
                unsigned int i = 0;
                size_t const nNormalize = size_t(-1) / (c_nInitialMapSize * 2);

                size_t nRand = 0;
                while ( !time_elapsed() ) {
                    nRand = cds::bitop::RandXorShift(nRand);
                    size_t n = nRand / nNormalize;
                    switch ( pAct[i] ) {
                    case do_find:
                        if ( rMap.find( n ))
                            ++m_nFindSuccess;
                        else
                            ++m_nFindFailed;
                        break;
                    case do_insert:
                        if ( rMap.insert( n, n ))
                            ++m_nInsertSuccess;
                        else
                            ++m_nInsertFailed;
                        break;
                    case do_delete:
                        if ( rMap.erase( n ))
                            ++m_nDeleteSuccess;
                        else
                            ++m_nDeleteFailed;
                        break;
                    }

                    if ( ++i >= c_nShuffleSize )
                        i = 0;
                }
            }
        };

    protected:
        template <class MAP>
        void do_test( MAP& testMap )
        {
            typedef WorkThread<MAP> work_thread;
            cds::OS::Timer    timer;

            // fill map - only odd number
            {
                std::vector<size_t> arr;
                arr.reserve( c_nInitialMapSize );
                for ( size_t i = 0; i < c_nInitialMapSize; ++i )
                    arr.push_back( i * 2 + 1);
                shuffle( arr.begin(), arr.end() );
                for ( size_t i = 0; i < c_nInitialMapSize; ++i )
                    testMap.insert( arr[i], arr[i] );
            }
            CPPUNIT_MSG( "   Insert " << c_nInitialMapSize << " items time (single-threaded)=" << timer.duration() );

            timer.reset();
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new work_thread( pool, testMap ), c_nThreadCount );
            pool.run( c_nDuration );
            //CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nFindSuccess = 0;
            size_t nFindFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                work_thread * pThread = static_cast<work_thread *>( *it );
                assert( pThread != nullptr );
                nInsertSuccess += pThread->m_nInsertSuccess;
                nInsertFailed += pThread->m_nInsertFailed;
                nDeleteSuccess += pThread->m_nDeleteSuccess;
                nDeleteFailed += pThread->m_nDeleteFailed;
                nFindSuccess += pThread->m_nFindSuccess;
                nFindFailed += pThread->m_nFindFailed;
            }

            size_t nTotalOps = nInsertSuccess + nInsertFailed + nDeleteSuccess + nDeleteFailed + nFindSuccess + nFindFailed;

            CPPUNIT_MSG( "  Totals (success/failed): \n\t"
                      << "      Insert=" << nInsertSuccess << '/' << nInsertFailed << "\n\t"
                      << "      Delete=" << nDeleteSuccess << '/' << nDeleteFailed << "\n\t"
                      << "        Find=" << nFindSuccess   << '/' << nFindFailed   << "\n\t"
                      << "       Speed=" << (nFindSuccess + nFindFailed) / c_nDuration << " find/sec\n\t"
                      << "             " << (nInsertSuccess + nDeleteSuccess) / c_nDuration << " modify/sec\n\t"
                      << "   Total ops=" << nTotalOps << "\n\t"
                      << "       speed=" << nTotalOps / c_nDuration << " ops/sec\n\t"
                      << "      Map size=" << testMap.size()
                );


            check_before_cleanup( testMap );

            CPPUNIT_MSG( "  Clear map (single-threaded)..." );
            timer.reset();
            testMap.clear();
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_ASSERT_EX( testMap.empty(), ((long long) testMap.size()) );

            additional_check( testMap );
            print_stat( testMap );
            additional_cleanup( testMap );
        }

        template <class MAP>
        void test()
        {
            CPPUNIT_MSG( "Thread count=" << c_nThreadCount
                << " initial map size=" << c_nInitialMapSize
                << " insert=" << c_nInsertPercentage << '%'
                << " delete=" << c_nDeletePercentage << '%'
                << " duration=" << c_nDuration << "s"
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                CPPUNIT_MSG( "Load factor=" << nLoadFactor );
                MAP  testMap( c_nInitialMapSize, nLoadFactor );
                do_test( testMap );
                if ( c_bPrintGCState )
                    print_gc_state();
            }

        }

        template <class MAP>
        void test_nolf()
        {
            CPPUNIT_MSG( "Thread count=" << c_nThreadCount
                << " initial map size=" << c_nInitialMapSize
                << " insert=" << c_nInsertPercentage << '%'
                << " delete=" << c_nDeletePercentage << '%'
                << " duration=" << c_nDuration << "s"
                );

            MAP testMap;
            do_test( testMap );
            if ( c_bPrintGCState )
                print_gc_state();
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );

        void run_MichaelMap(const char *in_name, bool invert = false);
        void run_SplitList(const char *in_name, bool invert = false);
        void run_StripedMap(const char *in_name, bool invert = false);
        void run_RefinableMap(const char *in_name, bool invert = false);
        void run_CuckooMap(const char *in_name, bool invert = false);
        void run_SkipListMap(const char *in_name, bool invert = false);
        void run_EllenBinTreeMap(const char *in_name, bool invert = false);
        void run_BronsonAVLTreeMap(const char *in_name, bool invert = false);
        void run_StdMap(const char *in_name, bool invert = false);

        virtual void myRun(const char *in_name, bool invert = false);

#   include "map2/map_defs.h"
        CDSUNIT_DECLARE_MichaelMap
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_SkipListMap
        CDSUNIT_DECLARE_EllenBinTreeMap
        CDSUNIT_DECLARE_BronsonAVLTreeMap
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        CDSUNIT_DECLARE_StdMap
    };
} // namespace map2
