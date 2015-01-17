/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "map2/map_types.h"
#include "cppunit/thread.h"
#include <algorithm> // random_shuffle
#include <vector>

namespace map2 {

#   define TEST_MAP(X)         void X() { test<MapTypes<key_type, value_type>::X >()    ; }
#   define TEST_MAP_NOLF(X)    void X() { test_nolf<MapTypes<key_type, value_type>::X >()    ; }
#   define TEST_MAP_EXTRACT(X)  TEST_MAP(X)
#   define TEST_MAP_NOLF_EXTRACT(X) TEST_MAP_NOLF(X)

    namespace {
        static size_t  c_nInitialMapSize = 500000   ;  // initial map size
        static size_t  c_nThreadCount = 8           ;  // thread count
        static size_t  c_nMaxLoadFactor = 8         ;  // maximum load factor
        static unsigned int c_nInsertPercentage = 5;
        static unsigned int c_nDeletePercentage = 5;
        static unsigned int c_nDuration = 30        ;  // test duration, seconds
        static bool    c_bPrintGCState = true;
    }

    class Map_InsDelFind: public CppUnitMini::TestCase
    {
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
                std::random_shuffle( arr.begin(), arr.end() );
                for ( size_t i = 0; i < c_nInitialMapSize; ++i )
                    testMap.insert( arr[i], arr[i] );
            }
            CPPUNIT_MSG( "   Insert " << c_nInitialMapSize << " items time (single-threaded)=" << timer.duration() );

            timer.reset();
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new work_thread( pool, testMap ), c_nThreadCount );
            pool.run( c_nDuration );
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nFindSuccess = 0;
            size_t nFindFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                work_thread * pThread = static_cast<work_thread *>( *it );
                assert( pThread != NULL );
                nInsertSuccess += pThread->m_nInsertSuccess;
                nInsertFailed += pThread->m_nInsertFailed;
                nDeleteSuccess += pThread->m_nDeleteSuccess;
                nDeleteFailed += pThread->m_nDeleteFailed;
                nFindSuccess += pThread->m_nFindSuccess;
                nFindFailed += pThread->m_nFindFailed;
            }

            CPPUNIT_MSG( "  Totals (success/failed): \n\t"
                      << "      Insert=" << nInsertSuccess << '/' << nInsertFailed << "\n\t"
                      << "      Delete=" << nDeleteSuccess << '/' << nDeleteFailed << "\n\t"
                      << "        Find=" << nFindSuccess   << '/' << nFindFailed   << "\n\t"
                      << "       Speed=" << (nFindSuccess + nFindFailed) / c_nDuration << " find/sec\n\t"
                      << "             " << (nInsertSuccess + nDeleteSuccess) / c_nDuration << " modify/sec\n\t"
                      << "      Map size=" << testMap.size()
                );


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

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            c_nInitialMapSize = cfg.getULong("InitialMapSize", 500000 );
            c_nThreadCount = cfg.getULong("ThreadCount", 8 );
            c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", 8 );
            c_nInsertPercentage = cfg.getUInt("InsertPercentage", 5 );
            c_nDeletePercentage = cfg.getUInt("DeletePercentage", 5 );
            c_nDuration = cfg.getUInt("Duration", 30 );
            c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );

            if ( c_nThreadCount == 0 )
                c_nThreadCount = cds::OS::topology::processor_count() * 2;

            CPPUNIT_ASSERT( c_nInsertPercentage + c_nDeletePercentage <= 100 );

            actions * pFirst = m_arrShuffle;
            actions * pLast = m_arrShuffle + c_nInsertPercentage;
            std::fill( pFirst, pLast, do_insert );
            pFirst = pLast;
            pLast += c_nDeletePercentage;
            std::fill( pFirst, pLast, do_delete );
            pFirst = pLast;
            pLast = m_arrShuffle + sizeof(m_arrShuffle)/sizeof(m_arrShuffle[0]);
            std::fill( pFirst, pLast, do_find );
            std::random_shuffle( m_arrShuffle, pLast );
        }

#   include "map2/map_defs.h"
        CDSUNIT_DECLARE_MichaelMap
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_SkipListMap
        CDSUNIT_DECLARE_EllenBinTreeMap
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        CDSUNIT_DECLARE_StdMap

        CPPUNIT_TEST_SUITE( Map_InsDelFind )
            CDSUNIT_TEST_MichaelMap
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListMap
            CDSUNIT_TEST_EllenBinTreeMap
            CDSUNIT_TEST_StripedMap
            CDSUNIT_TEST_RefinableMap
            CDSUNIT_TEST_CuckooMap
            CDSUNIT_TEST_StdMap
        CPPUNIT_TEST_SUITE_END()
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDelFind );
} // namespace map2
