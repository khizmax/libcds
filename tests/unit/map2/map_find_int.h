//$$CDS-header$$

// defines concurrent access to map::nonconcurrent_iterator::Sequence::TValue::nAccess field

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <vector>

// find int test in map<int> in mutithreaded mode
namespace map2 {

#define TEST_CASE(TAG, X)  void X();

    class Map_find_int: public CppUnitMini::TestCase
    {
    public:
        size_t c_nThreadCount = 8;     // thread count
        size_t c_nMapSize = 10000000;  // map size (count of searching item)
        size_t c_nPercentExists = 50;  // percent of existing keys in searching sequence
        size_t c_nPassCount = 2;
        size_t c_nMaxLoadFactor = 8;   // maximum load factor
        bool   c_bPrintGCState = true;

        size_t c_nCuckooInitialSize = 1024;// initial size for CuckooMap
        size_t c_nCuckooProbesetSize = 16; // CuckooMap probeset size (only for list-based probeset)
        size_t c_nCuckooProbesetThreshold = 0; // CUckooMap probeset threshold (o - use default)

        size_t c_nMultiLevelMap_HeadBits = 10;
        size_t c_nMultiLevelMap_ArrayBits = 4;

        size_t  c_nLoadFactor;  // current load factor

    private:
        typedef CppUnitMini::TestCase Base;
        typedef size_t   key_type;
        struct value_type {
            key_type    nKey    ;   // key
            bool        bExists ;   // true - key in map, false - key not in map
        };

        typedef std::vector<value_type> ValueVector;
        ValueVector             m_Arr;
        size_t                  m_nRealMapSize;

        void generateSequence();

        template <typename Iterator, typename Map>
        static bool check_result( Iterator const& it, Map const& map )
        {
            return it != map.end();
        }
        template <typename Map>
        static bool check_result( bool b, Map const& )
        {
            return b;
        }

        template <class Map>
        class TestThread: public CppUnitMini::TestThread
        {
            Map&     m_Map;

            virtual TestThread *    clone()
            {
                return new TestThread( *this );
            }
        public:
            struct Stat {
                size_t      nSuccess;
                size_t      nFailed;

                Stat()
                    : nSuccess(0)
                    , nFailed(0)
                {}
            };

            Stat    m_KeyExists;
            Stat    m_KeyNotExists;

        public:
            TestThread( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            TestThread( TestThread& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_find_int&  getTest()
            {
                return reinterpret_cast<Map_find_int&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                ValueVector& arr = getTest().m_Arr;
                size_t const nPassCount = getTest().c_nPassCount;

                Map& rMap = m_Map;
                for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                    if ( m_nThreadNo & 1 ) {
                        ValueVector::const_iterator itEnd = arr.end();
                        for ( ValueVector::const_iterator it = arr.begin(); it != itEnd; ++it ) {
                            auto bFound = rMap.contains( it->nKey );
                            if ( it->bExists ) {
                                if ( check_result( bFound, rMap ))
                                    ++m_KeyExists.nSuccess;
                                else {
                                    //rMap.find( it->nKey );
                                    ++m_KeyExists.nFailed;
                                }
                            }
                            else {
                                if ( check_result( bFound, rMap )) {
                                    //rMap.find( it->nKey );
                                    ++m_KeyNotExists.nFailed;
                                }
                                else
                                    ++m_KeyNotExists.nSuccess;
                            }
                        }
                    }
                    else {
                        ValueVector::const_reverse_iterator itEnd = arr.rend();
                        for ( ValueVector::const_reverse_iterator it = arr.rbegin(); it != itEnd; ++it ) {
                            auto bFound = rMap.contains( it->nKey );
                            if ( it->bExists ) {
                                if ( check_result( bFound, rMap ))
                                    ++m_KeyExists.nSuccess;
                                else {
                                    //rMap.find( it->nKey );
                                    ++m_KeyExists.nFailed;
                                }
                            }
                            else {
                                if ( check_result( bFound, rMap )) {
                                    //rMap.find( it->nKey );
                                    ++m_KeyNotExists.nFailed;
                                }
                                else
                                    ++m_KeyNotExists.nSuccess;
                            }
                        }
                    }
                }
            }
        };

    protected:

        template <class Map>
        void find_int_test( Map& testMap )
        {
            typedef TestThread<Map>     Thread;
            cds::OS::Timer    timer;

            // Fill the map
            CPPUNIT_MSG( "  Fill map with " << m_Arr.size() << " items...");
            timer.reset();
            for ( size_t i = 0; i < m_Arr.size(); ++i ) {
                if ( m_Arr[i].bExists ) {
                    CPPUNIT_ASSERT( check_result( testMap.insert( m_Arr[i].nKey, m_Arr[i] ), testMap ));
                }
            }
            CPPUNIT_MSG( "   Duration=" << timer.duration() );

            CPPUNIT_MSG( "  Searching...");
            CppUnitMini::ThreadPool pool( *this );
            pool.add( new Thread( pool, testMap ), c_nThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                Thread * pThread = static_cast<Thread *>( *it );
                CPPUNIT_CHECK( pThread->m_KeyExists.nFailed == 0 );
                CPPUNIT_CHECK( pThread->m_KeyExists.nSuccess == m_nRealMapSize * c_nPassCount );
                CPPUNIT_CHECK( pThread->m_KeyNotExists.nFailed == 0 );
                CPPUNIT_CHECK( pThread->m_KeyNotExists.nSuccess == (m_Arr.size() - m_nRealMapSize) * c_nPassCount );
            }

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
                    Map  testMap( *this );
                    find_int_test( testMap );
                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Map testMap( *this );
                find_int_test( testMap );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );

    public:
        Map_find_int()
            : c_nLoadFactor(2)
        {}

#   include "map2/map_defs.h"
        CDSUNIT_DECLARE_MichaelMap
        CDSUNIT_DECLARE_MichaelMap_nogc
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_SplitList_nogc
        CDSUNIT_DECLARE_SkipListMap
        CDSUNIT_DECLARE_SkipListMap_nogc
        CDSUNIT_DECLARE_EllenBinTreeMap
        CDSUNIT_DECLARE_BronsonAVLTreeMap
        CDSUNIT_DECLARE_MultiLevelHashMap
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        CDSUNIT_DECLARE_StdMap
        CDSUNIT_DECLARE_StdMap_NoLock

        CPPUNIT_TEST_SUITE(Map_find_int)
            CDSUNIT_TEST_MichaelMap
            CDSUNIT_TEST_MichaelMap_nogc
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SplitList_nogc
            CDSUNIT_TEST_SkipListMap
            CDSUNIT_TEST_SkipListMap_nogc
            CDSUNIT_TEST_EllenBinTreeMap
            CDSUNIT_TEST_BronsonAVLTreeMap
            CDSUNIT_TEST_MultiLevelHashMap
            CDSUNIT_TEST_CuckooMap
            CDSUNIT_TEST_StripedMap
            CDSUNIT_TEST_RefinableMap
            CDSUNIT_TEST_StdMap
            CDSUNIT_TEST_StdMap_NoLock
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace map
