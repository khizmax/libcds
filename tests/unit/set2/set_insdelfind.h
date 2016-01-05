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

#include "set2/set_type.h"
#include "cppunit/thread.h"

namespace set2 {

#define TEST_CASE(TAG, X)  void X();

    class Set_InsDelFind: public CppUnitMini::TestCase
    {
    public:
        size_t c_nSetSize = 500000;      // initial set size
        size_t c_nThreadCount = 8;       // thread count
        size_t c_nMaxLoadFactor = 8;     // maximum load factor
        unsigned int c_nInsertPercentage = 5;
        unsigned int c_nDeletePercentage = 5;
        unsigned int c_nDuration = 30;   // test duration, seconds
        bool c_bPrintGCState = true;

        size_t  c_nCuckooInitialSize = 1024;// initial size for CuckooSet
        size_t  c_nCuckooProbesetSize = 16; // CuckooSet probeset size (only for list-based probeset)
        size_t  c_nCuckooProbesetThreshold = 0; // CUckooSet probeset threshold (0 - use default)

        size_t c_nFeldmanSet_HeadBits = 10;
        size_t c_nFeldmanSet_ArrayBits = 4;

        size_t c_nLoadFactor = 2;

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
                size_t const nNormalize = size_t(-1) / ( getTest().c_nSetSize * 2);

                size_t nRand = 0;
                while ( !time_elapsed() ) {
                    nRand = cds::bitop::RandXorShift(nRand);
                    size_t n = nRand / nNormalize;
                    switch ( pAct[i] ) {
                    case do_find:
                        if ( rMap.contains( n ))
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
        void do_test( Set& testSet )
        {
            typedef WorkThread<Set> work_thread;

            // fill map - only odd number
            {
                size_t * pInitArr = new size_t[ c_nSetSize ];
                size_t * pEnd = pInitArr + c_nSetSize;
                for ( size_t i = 0; i < c_nSetSize; ++i )
                    pInitArr[i] = i * 2 + 1;
                shuffle( pInitArr, pEnd );
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

            size_t nTotalOps = nInsertSuccess + nInsertFailed + nDeleteSuccess + nDeleteFailed + nFindSuccess + nFindFailed;

            CPPUNIT_MSG( "  Totals (success/failed): \n\t"
                      << "      Insert=" << nInsertSuccess << '/' << nInsertFailed << "\n\t"
                      << "      Delete=" << nDeleteSuccess << '/' << nDeleteFailed << "\n\t"
                      << "        Find=" << nFindSuccess   << '/' << nFindFailed   << "\n\t"
                      << "       Speed=" << (nFindSuccess + nFindFailed) / c_nDuration << " find/sec\n\t"
                      << "             " << (nInsertSuccess + nDeleteSuccess) / c_nDuration << " modify/sec\n\t"
                      << "   Total ops=" << nTotalOps << "\n\t"
                      << "       speed=" << nTotalOps / c_nDuration << " ops/sec\n\t"
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
        void run_test()
        {
            CPPUNIT_MSG( "Thread count=" << c_nThreadCount
                << " initial map size=" << c_nSetSize
                << " insert=" << c_nInsertPercentage << '%'
                << " delete=" << c_nDeletePercentage << '%'
                << " duration=" << c_nDuration << "s"
                );

            if ( Set::c_bLoadFactorDepended ) {
                for ( c_nLoadFactor = 1; c_nLoadFactor <= c_nMaxLoadFactor; c_nLoadFactor *= 2 ) {
                    CPPUNIT_MSG("  LoadFactor = " << c_nLoadFactor );
                    Set s( *this );
                    do_test( s );
                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Set s( *this );
                do_test( s );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );

#   include "set2/set_defs.h"
        CDSUNIT_DECLARE_MichaelSet
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_StripedSet
        CDSUNIT_DECLARE_RefinableSet
        CDSUNIT_DECLARE_CuckooSet
        CDSUNIT_DECLARE_SkipListSet
        CDSUNIT_DECLARE_EllenBinTreeSet
        CDSUNIT_DECLARE_FeldmanHashSet
        CDSUNIT_DECLARE_StdSet

        CPPUNIT_TEST_SUITE_(Set_InsDelFind, "Map_InsDelFind")
            CDSUNIT_TEST_MichaelSet
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListSet
            CDSUNIT_TEST_FeldmanHashSet
            CDSUNIT_TEST_EllenBinTreeSet
            CDSUNIT_TEST_StripedSet
            CDSUNIT_TEST_RefinableSet
            CDSUNIT_TEST_CuckooSet
            CDSUNIT_TEST_StdSet
        CPPUNIT_TEST_SUITE_END();

    };
} // namespace set2
