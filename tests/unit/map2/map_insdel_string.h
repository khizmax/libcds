//$$CDS-header$$

#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <vector>

namespace map2 {

#   define TEST_MAP(IMPL, C, X)         void C::X() { test<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_NOLF(IMPL, C, X)    void C::X() { test_nolf<map_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_MAP_EXTRACT(IMPL, C, X)  TEST_MAP(IMPL, C, X)
#   define TEST_MAP_NOLF_EXTRACT(IMPL, C, X) TEST_MAP_NOLF(IMPL, C, X)

    class Map_InsDel_string: public CppUnitMini::TestCase
    {
        static size_t  c_nMapSize;            // map size
        static size_t  c_nInsertThreadCount;  // count of insertion thread
        static size_t  c_nDeleteThreadCount;  // count of deletion thread
        static size_t  c_nThreadPassCount;    // pass count for each thread
        static size_t  c_nMaxLoadFactor;      // maximum load factor
        static bool    c_bPrintGCState;

        typedef CppUnitMini::TestCase Base;
        typedef std::string key_type;
        typedef size_t      value_type;

        const std::vector<std::string> *  m_parrString;

        template <class MAP>
        class Inserter: public CppUnitMini::TestThread
        {
            MAP&     m_Map;

            virtual Inserter *    clone()
            {
                return new Inserter( *this );
            }
        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, MAP& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_string&  getTest()
            {
                return reinterpret_cast<Map_InsDel_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                MAP& rMap = m_Map;

                m_nInsertSuccess =
                    m_nInsertFailed = 0;

                const std::vector<std::string>& arrString = *getTest().m_parrString;
                size_t nArrSize = arrString.size();

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            if ( rMap.insert( arrString[nItem % nArrSize], nItem * 8 ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            if ( rMap.insert( arrString[nItem % nArrSize], nItem * 8 ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
            }
        };

        template <class MAP>
        class Deleter: public CppUnitMini::TestThread
        {
            MAP&     m_Map;

            virtual Deleter *    clone()
            {
                return new Deleter( *this );
            }
        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

        public:
            Deleter( CppUnitMini::ThreadPool& pool, MAP& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Deleter( Deleter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_string&  getTest()
            {
                return reinterpret_cast<Map_InsDel_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                MAP& rMap = m_Map;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                const std::vector<std::string>& arrString = *getTest().m_parrString;
                size_t nArrSize = arrString.size();

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            if ( rMap.erase( arrString[nItem % nArrSize] ) )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            if ( rMap.erase( arrString[nItem % nArrSize] ) )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
            }
        };

    protected:

        template <class MAP>
        void do_test( MAP& testMap )
        {
            typedef Inserter<MAP>       InserterThread;
            typedef Deleter<MAP>        DeleterThread;
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
            for ( size_t i = 0; i < m_parrString->size(); ++i )
                testMap.erase( (*m_parrString)[i] );
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_CHECK( testMap.empty() );

            additional_check( testMap );
            print_stat( testMap );
            additional_cleanup( testMap );
        }

        template <class MAP>
        void test()
        {
            m_parrString = &CppUnitMini::TestCase::getTestStrings();

            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " pass count=" << c_nThreadPassCount
                << " map size=" << c_nMapSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                CPPUNIT_MSG( "Load factor=" << nLoadFactor );
                MAP  testMap( c_nMapSize, nLoadFactor );
                do_test( testMap );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class MAP>
        void test_nolf()
        {
            m_parrString = &CppUnitMini::TestCase::getTestStrings();

            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " pass count=" << c_nThreadPassCount
                << " map size=" << c_nMapSize
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
