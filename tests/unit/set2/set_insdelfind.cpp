/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set2/set_types.h"
#include "cppunit/thread.h"
#include <algorithm> // random_shuffle

namespace set2 {

#   define TEST_SET(X)          void X() { test<SetTypes<key_type, value_type>::X >()    ; }
#   define TEST_SET_EXTRACT(X)  TEST_SET(X)
#   define TEST_SET_NOLF(X)    void X() { test_nolf<SetTypes<key_type, value_type>::X >()    ; }
#   define TEST_SET_NOLF_EXTRACT(X) TEST_SET_NOLF(X)

    namespace {
        static size_t  c_nInitialMapSize = 500000   ;  // initial map size
        static size_t  c_nThreadCount = 8           ;  // thread count
        static size_t  c_nMaxLoadFactor = 8         ;  // maximum load factor
        static unsigned int c_nInsertPercentage = 5;
        static unsigned int c_nDeletePercentage = 5;
        static unsigned int c_nDuration = 30        ;  // test duration, seconds
        static bool    c_bPrintGCState = true;
    }

    class Set_InsDelFind: public CppUnitMini::TestCase
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

        template <class Set>
        class WorkThread: public CppUnitMini::TestThread
        {
            Set&     m_Map;

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
            WorkThread( CppUnitMini::ThreadPool& pool, Set& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            WorkThread( WorkThread& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Set_InsDelFind&  getTest()
            {
                return reinterpret_cast<Set_InsDelFind&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rMap = m_Map;

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
                        if ( rMap.insert( n ))
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
        template <class Set>
        void do_test( size_t nLoadFactor )
        {
            CPPUNIT_MSG( "Load factor=" << nLoadFactor );

            Set  testSet( c_nInitialMapSize, nLoadFactor );
            do_test_with( testSet );
        }

        template <class Set>
        void do_test_with( Set& testSet )
        {
            typedef WorkThread<Set> work_thread;

            // fill map - only odd number
            {
                size_t * pInitArr = new size_t[ c_nInitialMapSize ];
                size_t * pEnd = pInitArr + c_nInitialMapSize;
                for ( size_t i = 0; i < c_nInitialMapSize; ++i )
                    pInitArr[i] = i * 2 + 1;
                std::random_shuffle( pInitArr, pEnd );
                for ( size_t * p = pInitArr; p < pEnd; ++p )
                    testSet.insert( typename Set::value_type( *p, *p ) );
                delete [] pInitArr;
            }

            cds::OS::Timer    timer;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new work_thread( pool, testSet ), c_nThreadCount );
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
                assert( pThread != nullptr );
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
                      << "      Set size=" << testSet.size()
                );


            CPPUNIT_MSG( "  Clear map (single-threaded)..." );
            timer.reset();
            testSet.clear();
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_CHECK_EX( testSet.empty(), ((long long) testSet.size()) );

            additional_check( testSet );
            print_stat( testSet );
            additional_cleanup( testSet );
        }

        template <class Set>
        void test()
        {
            CPPUNIT_MSG( "Thread count=" << c_nThreadCount
                << " initial map size=" << c_nInitialMapSize
                << " insert=" << c_nInsertPercentage << '%'
                << " delete=" << c_nDeletePercentage << '%'
                << " duration=" << c_nDuration << "s"
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                do_test<Set>( nLoadFactor );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Set>
        void test_nolf()
        {
            CPPUNIT_MSG( "Thread count=" << c_nThreadCount
                << " initial map size=" << c_nInitialMapSize
                << " insert=" << c_nInsertPercentage << '%'
                << " delete=" << c_nDeletePercentage << '%'
                << " duration=" << c_nDuration << "s"
                );

            Set s;
            do_test_with( s );
            //CPPUNIT_MSG( s.statistics() );
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

#   include "set2/set_defs.h"
        CDSUNIT_DECLARE_MichaelSet
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_StripedSet
        CDSUNIT_DECLARE_RefinableSet
        CDSUNIT_DECLARE_CuckooSet
        CDSUNIT_DECLARE_SkipListSet
        CDSUNIT_DECLARE_EllenBinTreeSet
        CDSUNIT_DECLARE_StdSet

        CPPUNIT_TEST_SUITE_( Set_InsDelFind, "Map_InsDelFind" )
            CDSUNIT_TEST_MichaelSet
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListSet
            CDSUNIT_TEST_EllenBinTreeSet
            CDSUNIT_TEST_StripedSet
            CDSUNIT_TEST_RefinableSet
            CDSUNIT_TEST_CuckooSet
            CDSUNIT_TEST_StdSet
        CPPUNIT_TEST_SUITE_END()
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDelFind );
} // namespace set2
