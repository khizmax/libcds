//$$CDS-header$$

#include <functional>
#include <mutex>    //unique_lock
#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <cds/sync/spinlock.h>
#include <vector>

namespace map2 {

#define TEST_CASE(TAG, X)  void X();

    class Map_InsDel_func: public CppUnitMini::TestCase
    {
    public:
        size_t c_nMapSize = 1000000;      // map size
        size_t c_nInsertThreadCount = 4;  // count of insertion thread
        size_t c_nDeleteThreadCount = 4;  // count of deletion thread
        size_t c_nUpdateThreadCount = 4;  // count of updating thread
        size_t c_nThreadPassCount   = 4;  // pass count for each thread
        size_t c_nMaxLoadFactor = 8;      // maximum load factor
        bool   c_bPrintGCState = true;

        size_t c_nCuckooInitialSize = 1024;// initial size for CuckooMap
        size_t c_nCuckooProbesetSize = 16; // CuckooMap probeset size (only for list-based probeset)
        size_t c_nCuckooProbesetThreshold = 0; // CUckooMap probeset threshold (o - use default)

        size_t c_nMultiLevelMap_HeadBits = 10;
        size_t c_nMultiLevelMap_ArrayBits = 4;

        size_t  c_nLoadFactor;  // current load factor

    private:
        typedef size_t  key_type;
        struct value_type {
            size_t      nKey;
            size_t      nData;
            atomics::atomic<size_t> nUpdateCall;
            atomics::atomic<bool>   bInitialized;
            cds::OS::ThreadId          threadId     ;   // insert thread id

            typedef cds::sync::spin_lock< cds::backoff::pause > lock_type;
            mutable lock_type   m_access;

            value_type()
                : nKey(0)
                , nData(0)
                , nUpdateCall(0)
                , bInitialized( false )
                , threadId( cds::OS::get_current_thread_id() )
            {}

            value_type( value_type const& s )
                : nKey(s.nKey)
                , nData(s.nData)
                , nUpdateCall(s.nUpdateCall.load(atomics::memory_order_relaxed))
                , bInitialized( s.bInitialized.load(atomics::memory_order_relaxed) )
                , threadId( cds::OS::get_current_thread_id() )
            {}

            // boost::container::flat_map requires operator =
            value_type& operator=( value_type const& v )
            {
                nKey = v.nKey;
                nData = v.nData;
                nUpdateCall.store( v.nUpdateCall.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
                bInitialized.store(v.bInitialized.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed);

                return *this;
            }
        };

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

            struct insert_functor {
                size_t nTestFunctorRef;

                insert_functor()
                    : nTestFunctorRef(0)
                {}

                template <typename Pair>
                void operator()( Pair& val )
                {
                    operator()( val.first, val.second );
                }

                template <typename Key, typename Val >
                void operator()( Key const& key, Val& v )
                {
                    std::unique_lock< typename value_type::lock_type>    ac( v.m_access );

                    v.nKey  = key;
                    v.nData = key * 8;

                    ++nTestFunctorRef;
                    v.bInitialized.store( true, atomics::memory_order_relaxed);
                }
            };

        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;

            size_t  m_nTestFunctorRef;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_func&  getTest()
            {
                return reinterpret_cast<Map_InsDel_func&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nInsertSuccess =
                    m_nInsertFailed =
                    m_nTestFunctorRef = 0;

                // func is passed by reference
                insert_functor  func;
                key_array const& arr = getTest().m_arrValues;
                size_t const nPassCount = getTest().c_nThreadPassCount;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = arr.begin(), itEnd = arr.end(); it != itEnd; ++it ) {
                            if ( rMap.insert_with( *it, std::ref(func) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = arr.rbegin(), itEnd = arr.rend(); it != itEnd; ++it ) {
                            if ( rMap.insert_with( *it, std::ref(func) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }

                m_nTestFunctorRef = func.nTestFunctorRef;
            }
        };

        template <class Map>
        class Updater: public CppUnitMini::TestThread
        {
            Map&     m_Map;

            virtual Updater *    clone()
            {
                return new Updater( *this );
            }

            struct update_functor {
                size_t  nCreated;
                size_t  nModified;

                update_functor()
                    : nCreated(0)
                    , nModified(0)
                {}

                template <typename Key, typename Val>
                void operator()( bool bNew, Key const& key, Val& v )
                {
                    std::unique_lock<typename value_type::lock_type>    ac( v.m_access );
                    if ( bNew ) {
                        ++nCreated;
                        v.nKey = key;
                        v.nData = key * 8;
                        v.bInitialized.store( true, atomics::memory_order_relaxed);
                    }
                    else {
                        v.nUpdateCall.fetch_add( 1, atomics::memory_order_relaxed );
                        ++nModified;
                    }
                }

                template <typename Pair>
                void operator()( bool bNew, Pair& val )
                {
                    operator()( bNew, val.first, val.second );
                }

                // For MultiLevelHashMap
                template <typename Val>
                void operator()( Val& cur, Val * old )
                {
                    operator()( old != nullptr, cur.first, cur.second );
                }

            private:
                update_functor(const update_functor& ) = delete;
            };

        public:
            size_t  m_nUpdateFailed;
            size_t  m_nUpdateCreated;
            size_t  m_nUpdateExisted;
            size_t  m_nFunctorCreated;
            size_t  m_nFunctorModified;

        public:
            Updater( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Updater( Updater& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_func&  getTest()
            {
                return reinterpret_cast<Map_InsDel_func&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nUpdateCreated =
                    m_nUpdateExisted =
                    m_nUpdateFailed = 0;

                update_functor func;

                key_array const& arr = getTest().m_arrValues;
                size_t const nPassCount = getTest().c_nThreadPassCount;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = arr.begin(), itEnd = arr.end(); it != itEnd; ++it ) {
                        //for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            std::pair<bool, bool> ret = rMap.update( *it, std::ref( func ) );
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nUpdateCreated;
                                else
                                    ++m_nUpdateExisted;
                            }
                            else
                                ++m_nUpdateFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = arr.rbegin(), itEnd = arr.rend(); it != itEnd; ++it ) {
                            std::pair<bool, bool> ret = rMap.update( *it, std::ref( func ) );
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nUpdateCreated;
                                else
                                    ++m_nUpdateExisted;
                            }
                            else
                                ++m_nUpdateFailed;
                        }
                    }
                }

                m_nFunctorCreated = func.nCreated;
                m_nFunctorModified = func.nModified;
            }
        };

        template <class Map>
        class Deleter: public CppUnitMini::TestThread
        {
            Map&     m_Map;
            typedef typename Map::mapped_type value_type;

            virtual Deleter *    clone()
            {
                return new Deleter( *this );
            }

            struct value_container
            {
                size_t      nKeyExpected;

                size_t      nSuccessItem;
                size_t      nFailedItem;

                value_container()
                    : nSuccessItem(0)
                    , nFailedItem(0)
                {}
            };

            struct erase_functor {
                value_container     m_cnt;

                template <typename Key, typename Val>
                void operator()( Key const& /*key*/, Val& v )
                {
                    while ( true ) {
                        if ( v.bInitialized.load( atomics::memory_order_relaxed )) {
                            std::unique_lock< typename value_type::lock_type>    ac( v.m_access );

                            if ( m_cnt.nKeyExpected == v.nKey && m_cnt.nKeyExpected * 8 == v.nData )
                                ++m_cnt.nSuccessItem;
                            else
                                ++m_cnt.nFailedItem;
                            v.nData++;
                            v.nKey = 0;
                            break;
                        }
                        else
                            cds::backoff::yield()();
                    }
                }

                template <typename Pair>
                void operator ()( Pair& item )
                {
                    operator()( item.first, item.second );
                }
            };

        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

            size_t  m_nValueSuccess;
            size_t  m_nValueFailed;

        public:
            Deleter( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Deleter( Deleter& src )
                : CppUnitMini::TestThread( src )
                , m_Map( src.m_Map )
            {}

            Map_InsDel_func&  getTest()
            {
                return reinterpret_cast<Map_InsDel_func&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Map& rMap = m_Map;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                erase_functor   func;
                key_array const& arr = getTest().m_arrValues;
                size_t const nPassCount = getTest().c_nThreadPassCount;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = arr.begin(), itEnd = arr.end(); it != itEnd; ++it ) {
                            func.m_cnt.nKeyExpected = *it;
                            if ( rMap.erase( *it, std::ref(func) ))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = arr.rbegin(), itEnd = arr.rend(); it != itEnd; ++it ) {
                            func.m_cnt.nKeyExpected = *it;
                            if ( rMap.erase( *it, std::ref(func) ))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }

                m_nValueSuccess = func.m_cnt.nSuccessItem;
                m_nValueFailed = func.m_cnt.nFailedItem;
            }
        };

    protected:

        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map>       InserterThread;
            typedef Deleter<Map>        DeleterThread;
            typedef Updater<Map>        UpdaterThread;
            cds::OS::Timer    timer;

            m_arrValues.clear();
            m_arrValues.reserve( c_nMapSize );
            for ( size_t i = 0; i < c_nMapSize; ++i )
                m_arrValues.push_back( i );
            shuffle( m_arrValues.begin(), m_arrValues.end() );

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testMap ), c_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testMap ), c_nDeleteThreadCount );
            pool.add( new UpdaterThread( pool, testMap ), c_nUpdateThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nDelValueSuccess = 0;
            size_t nDelValueFailed = 0;
            size_t nUpdateFailed = 0;
            size_t nUpdateCreated = 0;
            size_t nUpdateModified = 0;
            size_t nEnsFuncCreated = 0;
            size_t nEnsFuncModified = 0;
            size_t nTestFunctorRef = 0;

            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                InserterThread * pThread = dynamic_cast<InserterThread *>( *it );
                if ( pThread ) {
                    nInsertSuccess += pThread->m_nInsertSuccess;
                    nInsertFailed += pThread->m_nInsertFailed;
                    nTestFunctorRef += pThread->m_nTestFunctorRef;
                }
                else {
                    DeleterThread * p = dynamic_cast<DeleterThread *>( *it );
                    if ( p ) {
                        nDeleteSuccess += p->m_nDeleteSuccess;
                        nDeleteFailed += p->m_nDeleteFailed;
                        nDelValueSuccess += p->m_nValueSuccess;
                        nDelValueFailed += p->m_nValueFailed;
                    }
                    else {
                        UpdaterThread * pEns = static_cast<UpdaterThread *>( *it );
                        nUpdateCreated += pEns->m_nUpdateCreated;
                        nUpdateModified += pEns->m_nUpdateExisted;
                        nUpdateFailed += pEns->m_nUpdateFailed;
                        nEnsFuncCreated += pEns->m_nFunctorCreated;
                        nEnsFuncModified += pEns->m_nFunctorModified;
                    }
                }
            }

            CPPUNIT_MSG( "    Totals: Ins succ=" << nInsertSuccess
                << " Del succ=" << nDeleteSuccess << "\n"
                << "          : Ins fail=" << nInsertFailed
                << " Del fail=" << nDeleteFailed << "\n"
                << "          : Update succ=" << (nUpdateCreated + nUpdateModified) << " fail=" << nUpdateFailed
                << " create=" << nUpdateCreated << " modify=" << nUpdateModified << "\n"
                << "          Map size=" << testMap.size()
                );

            CPPUNIT_CHECK_EX( nDelValueFailed == 0, "Functor del failed=" << nDelValueFailed );
            CPPUNIT_CHECK_EX( nDelValueSuccess == nDeleteSuccess,  "Delete success=" << nDeleteSuccess << " functor=" << nDelValueSuccess );

            CPPUNIT_CHECK( nUpdateFailed == 0 );

            CPPUNIT_CHECK_EX( nUpdateCreated == nEnsFuncCreated, "Update created=" << nUpdateCreated << " functor=" << nEnsFuncCreated );
            CPPUNIT_CHECK_EX( nUpdateModified == nEnsFuncModified, "Update modified=" << nUpdateModified << " functor=" << nEnsFuncModified );

            // nTestFunctorRef is call count of insert functor
            CPPUNIT_CHECK_EX( nTestFunctorRef == nInsertSuccess, "nInsertSuccess=" << nInsertSuccess << " functor nTestFunctorRef=" << nTestFunctorRef );

            check_before_cleanup( testMap );

            CPPUNIT_MSG( "  Clear map (single-threaded)..." );
            timer.reset();
            for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                testMap.erase( nItem );
            }
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_CHECK( testMap.empty() );

            additional_check( testMap );
            print_stat( testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test()
        {
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " update=" << c_nUpdateThreadCount
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

        typedef CppUnitMini::TestCase Base;
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

        CPPUNIT_TEST_SUITE(Map_InsDel_func)
            CDSUNIT_TEST_MichaelMap
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListMap
            CDSUNIT_TEST_EllenBinTreeMap
            CDSUNIT_TEST_BronsonAVLTreeMap
            CDSUNIT_TEST_MultiLevelHashMap
            CDSUNIT_TEST_CuckooMap
            CDSUNIT_TEST_StripedMap
            CDSUNIT_TEST_RefinableMap
        CPPUNIT_TEST_SUITE_END();

    };
} // namespace map2
