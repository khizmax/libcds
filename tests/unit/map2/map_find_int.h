//$$CDS-header$$

// defines concurrent access to map::nonconcurrent_iterator::Sequence::TValue::nAccess field

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <vector>

// find int test in map<int> in mutithreaded mode
namespace map2 {

#   define TEST_MAP(IMPL, C, X)         void C::X() { test<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_NOLF(IMPL, C, X)    void C::X() { test_nolf<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_EXTRACT(IMPL, C, X)  TEST_MAP(IMPL, C, X)
#   define TEST_MAP_NOLF_EXTRACT(IMPL, C, X) TEST_MAP_NOLF(IMPL, C, X)

    class Map_find_int: public CppUnitMini::TestCase
    {
        static size_t c_nThreadCount;   // thread count
        static size_t c_nMapSize;       // map size (count of searching item)
        static size_t c_nPercentExists; // percent of existing keys in searching sequence
        static size_t c_nPassCount;
        static size_t c_nMaxLoadFactor; // maximum load factor
        static bool   c_bPrintGCState;

        typedef CppUnitMini::TestCase Base;
        typedef size_t   key_type;
        struct value_type {
            key_type    nKey    ;   // key
            bool        bExists ;   // true - key in map, false - key not in map
        };

        typedef std::vector<value_type> ValueVector;
        ValueVector             m_Arr;
        size_t                  m_nRealMapSize;
        bool                    m_bSequenceInitialized;

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
                //size_t nSize = arr.size();

                Map& rMap = m_Map;
                for ( size_t nPass = 0; nPass < c_nPassCount; ++nPass ) {
                    if ( m_nThreadNo & 1 ) {
                        ValueVector::const_iterator itEnd = arr.end();
                        for ( ValueVector::const_iterator it = arr.begin(); it != itEnd; ++it ) {
                            auto bFound = rMap.find( it->nKey );
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
                            auto bFound = rMap.find( it->nKey );
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

        void initTestSequence();

        template <class Map>
        void test()
        {
            if ( !m_bSequenceInitialized )
                initTestSequence();

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                CPPUNIT_MSG( "Load factor=" << nLoadFactor );
                Map  testMap( c_nMapSize, nLoadFactor );
                find_int_test( testMap );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Map>
        void test_nolf()
        {
            if ( !m_bSequenceInitialized )
                initTestSequence();

            Map testMap;
            find_int_test( testMap );
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

    public:
        Map_find_int()
            : m_bSequenceInitialized( false )
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
        CDSUNIT_DECLARE_StripedMap
        CDSUNIT_DECLARE_RefinableMap
        CDSUNIT_DECLARE_CuckooMap
        CDSUNIT_DECLARE_StdMap
    };
} // namespace map
