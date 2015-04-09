//$$CDS-header$$

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <cds/os/topology.h>
#include <vector>

namespace map2 {

#   define TEST_MAP(IMPL, C, X)         void C::X() { test<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_NOLF(IMPL, C, X)    void C::X() { test_nolf<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_EXTRACT(IMPL, C, X)  TEST_MAP(IMPL, C, X)
#   define TEST_MAP_NOLF_EXTRACT(IMPL, C, X) TEST_MAP_NOLF(IMPL, C, X)

    class Map_InsFind_int: public CppUnitMini::TestCase
    {
        static size_t  c_nMapSize;       // map size
        static size_t  c_nThreadCount;   // count of insertion thread
        static size_t  c_nMaxLoadFactor; // maximum load factor
        static bool    c_bPrintGCState;

        typedef CppUnitMini::TestCase Base;
        typedef size_t  key_type;
        typedef size_t  value_type;

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
        class Inserter: public CppUnitMini::TestThread
        {
            Map&     m_Map;
            std::vector<size_t> m_arrVal;

            virtual Inserter *    clone()
            {
                return new Inserter( *this );
            }

            void make_array()
            {
                size_t const nSize = c_nMapSize / c_nThreadCount + 1;
                m_arrVal.resize( nSize );
                size_t nItem = m_nThreadNo;
                for ( size_t i = 0; i < nSize; nItem += c_nThreadCount, ++i )
                    m_arrVal[i] = nItem;
                shuffle( m_arrVal.begin(), m_arrVal.end() );
            }
        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;
            size_t  m_nFindSuccess;
            size_t  m_nFindFail;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsFind_int&  getTest()
            {
                return reinterpret_cast<Map_InsFind_int&>( m_Pool.m_Test );
            }

            virtual void init()
            {
                cds::threading::Manager::attachThread();
                make_array();
            }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nInsertSuccess =
                    m_nInsertFailed =
                    m_nFindSuccess =
                    m_nFindFail = 0;

                size_t const nArrSize = m_arrVal.size();
                for ( size_t i = 0; i < nArrSize; ++i ) {
                    size_t const nItem = m_arrVal[i];
                    if ( check_result( rMap.insert( nItem, nItem * 8 ), rMap ))
                        ++m_nInsertSuccess;
                    else
                        ++m_nInsertFailed;

                    for ( size_t k = 0; k <= i; ++k ) {
                        if ( check_result( rMap.find( m_arrVal[k] ), rMap ))
                            ++m_nFindSuccess;
                        else
                            ++m_nFindFail;
                    }
                }
            }
        };

    protected:

        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map>       InserterThread;
            cds::OS::Timer    timer;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testMap ), c_nThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nFindSuccess = 0;
            size_t nFindFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                InserterThread * pThread = static_cast<InserterThread *>( *it );

                nInsertSuccess += pThread->m_nInsertSuccess;
                nInsertFailed += pThread->m_nInsertFailed;
                nFindSuccess += pThread->m_nFindSuccess;
                nFindFailed += pThread->m_nFindFail;
            }

            CPPUNIT_MSG( "    Totals: Ins succ=" << nInsertSuccess << " fail=" << nInsertFailed << "\n"
                      << "           Find succ=" << nFindSuccess << " fail=" << nFindFailed
            );

            CPPUNIT_CHECK( nInsertFailed == 0 );
            CPPUNIT_CHECK( nFindFailed == 0 );

            check_before_cleanup( testMap );

            testMap.clear();
            additional_check( testMap );
            print_stat( testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void test()
        {
            static_assert( (!std::is_same< typename Map::item_counter, cds::atomicity::empty_item_counter >::value),
                "Empty item counter is not suitable for this test");

            CPPUNIT_MSG( "Thread count: " << c_nThreadCount
                << " map size=" << c_nMapSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                CPPUNIT_MSG( "Load factor=" << nLoadFactor );
                Map  testMap( c_nMapSize, nLoadFactor );
                do_test( testMap );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Map>
        void test_nolf()
        {
            static_assert( (!std::is_same< typename Map::item_counter, cds::atomicity::empty_item_counter >::value),
                "Empty item counter is not suitable for this test");

            CPPUNIT_MSG( "Thread count: " << c_nThreadCount
                << " map size=" << c_nMapSize
                );

            Map testMap;
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
        CDSUNIT_DECLARE_MichaelMap_nogc
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_SplitList_nogc
        CDSUNIT_DECLARE_SkipListMap
        CDSUNIT_DECLARE_SkipListMap_nogc
        CDSUNIT_DECLARE_EllenBinTreeMap
        CDSUNIT_DECLARE_BronsonAVLTreeMap
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        CDSUNIT_DECLARE_StdMap
    };
} // namespace map2
