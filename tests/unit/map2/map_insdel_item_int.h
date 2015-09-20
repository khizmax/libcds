//$$CDS-header$$

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <vector>

namespace map2 {

#define TEST_CASE(TAG, X)  void X();

    class Map_InsDel_Item_int: public CppUnitMini::TestCase
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

        size_t c_nMultiLevelMap_HeadBits = 10;
        size_t c_nMultiLevelMap_ArrayBits = 4;

        size_t  c_nGoalItem;
        size_t  c_nLoadFactor = 2;  // current load factor

    private:
        typedef size_t  key_type;
        typedef size_t  value_type;

        template <class Map>
        class Inserter: public CppUnitMini::TestThread
        {
            Map&     m_Map;

            virtual Inserter *    clone()
            {
                return new Inserter( *this );
            }

            struct update_func
            {
                void operator()( bool bNew, std::pair<key_type const, value_type>& item )
                {
                    if ( bNew )
                        item.second = item.first;
                }
                // for boost::container::flat_map
                void operator()( bool bNew, std::pair<key_type, value_type>& item )
                {
                    if ( bNew )
                        item.second = item.first;
                }

                // for BronsonAVLTreeMap
                void operator()( bool bNew, key_type key, value_type& val )
                {
                    if ( bNew )
                        val = key;
                }

                // for MultiLevelHashMap
                void operator()( std::pair<key_type const, value_type>& item, std::pair<key_type const, value_type> * pOld )
                {
                    if ( !pOld )
                        item.second = item.first;
                }
            };

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

            Map_InsDel_Item_int&  getTest()
            {
                return reinterpret_cast<Map_InsDel_Item_int&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nInsertSuccess =
                    m_nInsertFailed = 0;

                size_t nGoalItem = getTest().c_nGoalItem;
                size_t const nAttemptCount = getTest().c_nAttemptCount;

                for ( size_t nAttempt = 0; nAttempt < nAttemptCount; ) {
                    if ( nAttempt % 2  == 0 ) {
                        if ( rMap.insert( nGoalItem, nGoalItem )) {
                            ++m_nInsertSuccess;
                            ++nAttempt;
                        }
                        else
                            ++m_nInsertFailed;
                    }
                    else {
                        std::pair<bool, bool> updateResult = rMap.update( nGoalItem, update_func(), true );
                        if ( updateResult.second ) {
                            ++m_nInsertSuccess;
                            ++nAttempt;
                        }
                        else
                            ++m_nInsertFailed;
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

            Map_InsDel_Item_int&  getTest()
            {
                return reinterpret_cast<Map_InsDel_Item_int&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                size_t nGoalItem = getTest().c_nGoalItem;
                size_t const nAttemptCount = getTest().c_nAttemptCount;
                for ( size_t nAttempt = 0; nAttempt < nAttemptCount; ) {
                    if ( rMap.erase( nGoalItem )) {
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
            {
                std::vector<key_type>   v;
                v.reserve( c_nMapSize );
                for ( size_t i = 0; i < c_nMapSize; ++i )
                    v.push_back( i );
                shuffle( v.begin(), v.end() );
                for ( size_t i = 0; i < v.size(); ++i ) {
                    CPPUNIT_ASSERT( testMap.insert( v[i], v[i] ));
                }
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
            CPPUNIT_CHECK( nInsertSuccess == nDeleteSuccess );
            size_t nGoalItem = c_nGoalItem;
            CPPUNIT_CHECK( testMap.contains( nGoalItem ));


            CPPUNIT_MSG( "    Totals: Ins fail=" << nInsertFailed << " Del fail=" << nDeleteFailed );

            // Check if the map contains all items
            CPPUNIT_MSG( "    Check if the map contains all items" );
            timer.reset();
            for ( size_t i = 0; i < c_nMapSize; ++i ) {
                CPPUNIT_CHECK_EX( testMap.contains( i ), "key " << i );
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
            if ( Map::c_bLoadFactorDepended ) {
                for ( c_nLoadFactor = 1; c_nLoadFactor <= c_nMaxLoadFactor; c_nLoadFactor *= 2 ) {
                    CPPUNIT_MSG( "Load factor=" << c_nLoadFactor );
                    Map testMap( *this );
                    do_test( testMap );
                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Map testMap( *this );
                do_test<Map>( testMap );
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
        CDSUNIT_DECLARE_MultiLevelHashMap
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        // CDSUNIT_DECLARE_StdMap // very slow!!

        CPPUNIT_TEST_SUITE(Map_InsDel_Item_int)
            CDSUNIT_TEST_MichaelMap
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListMap
            CDSUNIT_TEST_EllenBinTreeMap
            CDSUNIT_TEST_BronsonAVLTreeMap
            CDSUNIT_TEST_MultiLevelHashMap
            CDSUNIT_TEST_CuckooMap
            CDSUNIT_TEST_StripedMap
            CDSUNIT_TEST_RefinableMap
            // CDSUNIT_TEST_StdMap // very slow!!
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace map2
