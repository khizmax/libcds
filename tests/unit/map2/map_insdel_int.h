//$$CDS-header$$

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <vector>

namespace map2 {

#   define TEST_CASE(TAG, X)  void X();

    class Map_InsDel_int: public CppUnitMini::TestCase
    {
    public:
        size_t c_nMapSize = 1000000;      // map size
        size_t c_nInsertThreadCount = 4;  // count of insertion thread
        size_t c_nDeleteThreadCount = 4;  // count of deletion thread
        size_t c_nThreadPassCount = 4;    // pass count for each thread
        size_t c_nMaxLoadFactor = 8;      // maximum load factor

        size_t c_nCuckooInitialSize = 1024;// initial size for CuckooMap
        size_t c_nCuckooProbesetSize = 16; // CuckooMap probeset size (only for list-based probeset)
        size_t c_nCuckooProbesetThreshold = 0; // CUckooMap probeset threshold (o - use default)

        size_t c_nFeldmanMap_HeadBits = 10;
        size_t c_nFeldmanMap_ArrayBits = 4;

        bool   c_bPrintGCState = true;

        size_t  c_nLoadFactor;  // current load factor

    private:
        typedef size_t  key_type;
        typedef size_t  value_type;

        typedef std::vector<key_type>   key_array;
        key_array                       m_arrValues;

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

            Map_InsDel_int&  getTest()
            {
                return reinterpret_cast<Map_InsDel_int&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nInsertSuccess =
                    m_nInsertFailed = 0;
                key_array const& arr = getTest().m_arrValues;

                size_t const nPassCount = getTest().c_nThreadPassCount;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = arr.begin(), itEnd = arr.end(); it != itEnd; ++it ) {
                            if ( rMap.insert( *it, *it * 8 ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = arr.rbegin(), itEnd = arr.rend(); it != itEnd; ++it ) {
                            if ( rMap.insert( *it, *it * 8 ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
            }
        };

        template <class Map>
        class Deleter: public CppUnitMini::TestThread
        {
            Map&     m_Map;

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

            Map_InsDel_int&  getTest()
            {
                return reinterpret_cast<Map_InsDel_int&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;
                key_array const& arr = getTest().m_arrValues;

                size_t const nPassCount = getTest().c_nThreadPassCount;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = arr.begin(), itEnd = arr.end(); it != itEnd; ++it ) {
                            if ( rMap.erase( *it ) )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = arr.rbegin(), itEnd = arr.rend(); it != itEnd; ++it ) {
                            if ( rMap.erase( *it ) )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
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

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testMap ), c_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testMap ), c_nDeleteThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                InserterThread * pThread = dynamic_cast<InserterThread *>( *it );
                if ( pThread ) {
                    nInsertSuccess += pThread->m_nInsertSuccess;
                    nInsertFailed += pThread->m_nInsertFailed;
                }
                else {
                    DeleterThread * p = static_cast<DeleterThread *>( *it );
                    nDeleteSuccess += p->m_nDeleteSuccess;
                    nDeleteFailed += p->m_nDeleteFailed;
                }
            }

            CPPUNIT_MSG( "    Totals: Ins succ=" << nInsertSuccess
                << " Del succ=" << nDeleteSuccess << "\n"
                << "          : Ins fail=" << nInsertFailed
                << " Del fail=" << nDeleteFailed
                << " Map size=" << testMap.size()
                );

            check_before_cleanup( testMap );

            CPPUNIT_MSG( "  Clear map (single-threaded)..." );
            timer.reset();
            for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                testMap.erase( nItem );
            }
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_CHECK( testMap.empty());
            CPPUNIT_CHECK_EX( testMap.size() == 0, "size() == " << testMap.size() );

            additional_check( testMap );
            print_stat( testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test()
        {
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " pass count=" << c_nThreadPassCount
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
        CDSUNIT_DECLARE_FeldmanHashMap_fixed
        CDSUNIT_DECLARE_FeldmanHashMap_city
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        CDSUNIT_DECLARE_StdMap

        CPPUNIT_TEST_SUITE(Map_InsDel_int)
            CDSUNIT_TEST_MichaelMap
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListMap
            CDSUNIT_TEST_EllenBinTreeMap
            CDSUNIT_TEST_BronsonAVLTreeMap
            CDSUNIT_TEST_FeldmanHashMap_fixed
            CDSUNIT_TEST_FeldmanHashMap_city
            CDSUNIT_TEST_CuckooMap
            CDSUNIT_TEST_StripedMap
            CDSUNIT_TEST_RefinableMap
            CDSUNIT_TEST_StdMap
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace map2
