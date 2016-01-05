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

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <vector>

namespace map2 {

#define TEST_CASE(TAG, X)  void X();

    class Map_InsDel_Item_string: public CppUnitMini::TestCase
    {
    public:
        size_t  c_nMapSize = 1000000;       // map size
        size_t  c_nThreadCount = 4;         // thread count
        size_t  c_nAttemptCount = 100000;   // count of SUCCESS insert/delete for each thread
        size_t  c_nMaxLoadFactor = 8;       // maximum load factor
        bool    c_bPrintGCState = true;

        size_t c_nCuckooInitialSize = 1024; // initial size for CuckooMap
        size_t c_nCuckooProbesetSize = 16;  // CuckooMap probeset size (only for list-based probeset)
        size_t c_nCuckooProbesetThreshold = 0; // CUckooMap probeset threshold (o - use default)

        size_t c_nFeldmanMap_HeadBits = 10;
        size_t c_nFeldmanMap_ArrayBits = 4;

        size_t  c_nGoalItem;
        size_t  c_nLoadFactor = 2;  // current load factor

    private:
        typedef std::string  key_type;
        typedef size_t  value_type;

        const std::vector<std::string> *  m_parrString;

        template <class Map>
        class Inserter: public CppUnitMini::TestThread
        {
            Map&     m_Map;

            virtual Inserter *    clone()
            {
                return new Inserter( *this );
            }
        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_Item_string&  getTest()
            {
                return reinterpret_cast<Map_InsDel_Item_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nInsertSuccess =
                    m_nInsertFailed = 0;

                size_t nGoalItem = getTest().c_nGoalItem;
                std::string strGoal = (*getTest().m_parrString)[nGoalItem];
                size_t const nAttemptCount = getTest().c_nAttemptCount;

                for ( size_t nAttempt = 0; nAttempt < nAttemptCount; ) {
                    if ( rMap.insert( strGoal, nGoalItem )) {
                        ++m_nInsertSuccess;
                        ++nAttempt;
                    }
                    else
                        ++m_nInsertFailed;
                }
            }
        };

        template <class Map>
        class Deleter: public CppUnitMini::TestThread
        {
            Map&     m_Map;

            struct erase_cleaner {
                void operator ()(std::pair<typename Map::key_type const, typename Map::mapped_type>& val )
                {
                    val.second = 0;
                }
                // for boost::container::flat_map
                void operator ()(std::pair< typename std::remove_const< typename Map::key_type >::type, typename Map::mapped_type>& val )
                {
                    val.second = 0;
                }
                // for BronsonAVLTreeMap
                void operator()( typename Map::key_type const& /*key*/, typename Map::mapped_type& val )
                {
                    val = 0;
                }
            };

            virtual Deleter *    clone()
            {
                return new Deleter( *this );
            }
        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

        public:
            Deleter( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Deleter( Deleter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_Item_string&  getTest()
            {
                return reinterpret_cast<Map_InsDel_Item_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                size_t nGoalItem = getTest().c_nGoalItem;
                std::string strGoal = (*getTest().m_parrString)[nGoalItem];
                size_t const nAttemptCount = getTest().c_nAttemptCount;

                for ( size_t nAttempt = 0; nAttempt < nAttemptCount; ) {
                    if ( rMap.erase( strGoal, erase_cleaner() )) {
                        ++m_nDeleteSuccess;
                        ++nAttempt;
                    }
                    else
                        ++m_nDeleteFailed;
                }
            }
        };

    protected:

        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map>       InserterThread;
            typedef Deleter<Map>        DeleterThread;
            cds::OS::Timer    timer;

            // Fill the map
            CPPUNIT_MSG( "  Fill map (" << c_nMapSize << " items)...");
            timer.reset();
            for ( size_t i = 0; i < c_nMapSize; ++i ) {
                CPPUNIT_ASSERT_EX( testMap.insert( (*m_parrString)[i], i ), i );
            }
            CPPUNIT_MSG( "   Duration=" << timer.duration() );

            CPPUNIT_MSG( "  Insert/delete the key " << c_nGoalItem << " (" << c_nAttemptCount << " successful times)...");
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testMap ), (c_nThreadCount + 1) / 2 );
            pool.add( new DeleterThread( pool, testMap ), (c_nThreadCount + 1) / 2 );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                InserterThread * pThread = dynamic_cast<InserterThread *>( *it );
                if ( pThread ) {
                    CPPUNIT_CHECK( pThread->m_nInsertSuccess == c_nAttemptCount );
                    nInsertSuccess += pThread->m_nInsertSuccess;
                    nInsertFailed += pThread->m_nInsertFailed;
                }
                else {
                    DeleterThread * p = static_cast<DeleterThread *>( *it );
                    CPPUNIT_CHECK( p->m_nDeleteSuccess == c_nAttemptCount );
                    nDeleteSuccess += p->m_nDeleteSuccess;
                    nDeleteFailed += p->m_nDeleteFailed;
                }
            }
            CPPUNIT_CHECK_EX( nInsertSuccess == nDeleteSuccess, "nInsertSuccess=" << nInsertSuccess << ", nDeleteSuccess=" << nDeleteSuccess );
            CPPUNIT_MSG( "    Totals: Ins fail=" << nInsertFailed << " Del fail=" << nDeleteFailed );

            // Check if the map contains all items
            CPPUNIT_MSG( "    Check if the map contains all items" );
            timer.reset();
            for ( size_t i = 0; i < c_nMapSize; ++i ) {
                CPPUNIT_CHECK_EX( testMap.contains( (*m_parrString)[i] ), "Key \"" << (*m_parrString)[i] << "\" not found" );
            }
            CPPUNIT_MSG( "    Duration=" << timer.duration() );

            check_before_cleanup( testMap );

            testMap.clear();
            additional_check( testMap );
            print_stat( testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test()
        {
            m_parrString = &CppUnitMini::TestCase::getTestStrings();
            if ( c_nMapSize > m_parrString->size() )
                c_nMapSize = m_parrString->size();
            if ( c_nGoalItem > m_parrString->size() )
                c_nGoalItem = m_parrString->size() / 2;

            CPPUNIT_MSG( "Thread count= " << c_nThreadCount
                << " pass count=" << c_nAttemptCount
                << " map size=" << c_nMapSize
                );

            if ( Map::c_bLoadFactorDepended ) {
                for ( c_nLoadFactor = 1; c_nLoadFactor <= c_nMaxLoadFactor; c_nLoadFactor *= 2 ) {
                    CPPUNIT_MSG( "Load factor=" << c_nLoadFactor );
                    Map  testMap( *this );
                    do_test( testMap );
                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Map testMap( *this );
                do_test( testMap );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );

#   include "map2/map_defs.h"
        CDSUNIT_DECLARE_MichaelMap
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_SkipListMap
        CDSUNIT_DECLARE_EllenBinTreeMap
        CDSUNIT_DECLARE_BronsonAVLTreeMap
        CDSUNIT_DECLARE_FeldmanHashMap_city
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        // CDSUNIT_DECLARE_StdMap // very slow!!

        CPPUNIT_TEST_SUITE(Map_InsDel_Item_string)
            CDSUNIT_TEST_MichaelMap
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListMap
            CDSUNIT_TEST_EllenBinTreeMap
            CDSUNIT_TEST_BronsonAVLTreeMap
            CDSUNIT_TEST_FeldmanHashMap_city
            CDSUNIT_TEST_CuckooMap
            CDSUNIT_TEST_StripedMap
            CDSUNIT_TEST_RefinableMap
            // CDSUNIT_TEST_StdMap // very slow!!
        CPPUNIT_TEST_SUITE_END();

    };
} // namespace map2
