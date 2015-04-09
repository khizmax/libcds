//$$CDS-header$$

#include <functional>
#include <mutex>    //unique_lock
#include "map2/map_type.h"
#include "cppunit/thread.h"

#include <cds/sync/spinlock.h>
#include <vector>

namespace map2 {

#   define TEST_MAP(IMPL, C, X)          void C::X() { test<map_type<IMPL, key_type, value_type>::X >()    ; }
#   define TEST_MAP_EXTRACT(IMPL, C, X)  TEST_MAP(IMPL, C, X)
#   define TEST_MAP_NOLF(IMPL, C, X)     void C::X() { test_nolf<map_type<IMPL, key_type, value_type>::X >()    ; }
#   define TEST_MAP_NOLF_EXTRACT(IMPL, C, X) TEST_MAP_NOLF(IMPL, C, X)

    class Map_InsDel_func: public CppUnitMini::TestCase
    {
        static size_t  c_nMapSize;            // map size
        static size_t  c_nInsertThreadCount;  // count of insertion thread
        static size_t  c_nDeleteThreadCount;  // count of deletion thread
        static size_t  c_nEnsureThreadCount;  // count of ensure thread
        static size_t  c_nThreadPassCount;    // pass count for each thread
        static size_t  c_nMaxLoadFactor;      // maximum load factor
        static bool    c_bPrintGCState;

        typedef size_t  key_type;
        struct value_type {
            size_t      nKey;
            size_t      nData;
            atomics::atomic<size_t> nEnsureCall;
            atomics::atomic<bool>   bInitialized;
            cds::OS::ThreadId          threadId     ;   // insert thread id

            typedef cds::sync::spin_lock< cds::backoff::pause > lock_type;
            mutable lock_type   m_access;

            value_type()
                : nKey(0)
                , nData(0)
                , nEnsureCall(0)
                , bInitialized( false )
                , threadId( cds::OS::get_current_thread_id() )
            {}

            value_type( value_type const& s )
                : nKey(s.nKey)
                , nData(s.nData)
                , nEnsureCall(s.nEnsureCall.load(atomics::memory_order_relaxed))
                , bInitialized( s.bInitialized.load(atomics::memory_order_relaxed) )
                , threadId( cds::OS::get_current_thread_id() )
            {}

            // boost::container::flat_map requires operator =
            value_type& operator=( value_type const& v )
            {
                nKey = v.nKey;
                nData = v.nData;
                nEnsureCall.store( v.nEnsureCall.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
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

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = arr.begin(), itEnd = arr.end(); it != itEnd; ++it ) {
                            if ( rMap.insert_with( *it, std::ref(func) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
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
        class Ensurer: public CppUnitMini::TestThread
        {
            Map&     m_Map;

            virtual Ensurer *    clone()
            {
                return new Ensurer( *this );
            }

            struct ensure_functor {
                size_t  nCreated;
                size_t  nModified;

                ensure_functor()
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
                        v.nEnsureCall.fetch_add( 1, atomics::memory_order_relaxed );
                        ++nModified;
                    }
                }

                template <typename Pair>
                void operator()( bool bNew, Pair& val )
                {
                    operator()( bNew, val.first, val.second );
                }
            private:
                ensure_functor(const ensure_functor& );
            };

        public:
            size_t  m_nEnsureFailed;
            size_t  m_nEnsureCreated;
            size_t  m_nEnsureExisted;
            size_t  m_nFunctorCreated;
            size_t  m_nFunctorModified;

        public:
            Ensurer( CppUnitMini::ThreadPool& pool, Map& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Map( rMap )
            {}
            Ensurer( Ensurer& src )
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

                m_nEnsureCreated =
                    m_nEnsureExisted =
                    m_nEnsureFailed = 0;

                ensure_functor func;

                key_array const& arr = getTest().m_arrValues;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = arr.begin(), itEnd = arr.end(); it != itEnd; ++it ) {
                        //for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            std::pair<bool, bool> ret = rMap.ensure( *it, std::ref( func ) );
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nEnsureCreated;
                                else
                                    ++m_nEnsureExisted;
                            }
                            else
                                ++m_nEnsureFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = arr.rbegin(), itEnd = arr.rend(); it != itEnd; ++it ) {
                            std::pair<bool, bool> ret = rMap.ensure( *it, std::ref( func ) );
                            if ( ret.first  ) {
                                if ( ret.second )
                                    ++m_nEnsureCreated;
                                else
                                    ++m_nEnsureExisted;
                            }
                            else
                                ++m_nEnsureFailed;
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

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
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
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
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
            typedef Ensurer<Map>        EnsurerThread;
            cds::OS::Timer    timer;

            m_arrValues.clear();
            m_arrValues.reserve( c_nMapSize );
            for ( size_t i = 0; i < c_nMapSize; ++i )
                m_arrValues.push_back( i );
            shuffle( m_arrValues.begin(), m_arrValues.end() );

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testMap ), c_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testMap ), c_nDeleteThreadCount );
            pool.add( new EnsurerThread( pool, testMap ), c_nEnsureThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nDelValueSuccess = 0;
            size_t nDelValueFailed = 0;
            size_t nEnsureFailed = 0;
            size_t nEnsureCreated = 0;
            size_t nEnsureModified = 0;
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
                        EnsurerThread * pEns = static_cast<EnsurerThread *>( *it );
                        nEnsureCreated += pEns->m_nEnsureCreated;
                        nEnsureModified += pEns->m_nEnsureExisted;
                        nEnsureFailed += pEns->m_nEnsureFailed;
                        nEnsFuncCreated += pEns->m_nFunctorCreated;
                        nEnsFuncModified += pEns->m_nFunctorModified;
                    }
                }
            }

            CPPUNIT_MSG( "    Totals: Ins succ=" << nInsertSuccess
                << " Del succ=" << nDeleteSuccess << "\n"
                << "          : Ins fail=" << nInsertFailed
                << " Del fail=" << nDeleteFailed << "\n"
                << "          : Ensure succ=" << (nEnsureCreated + nEnsureModified) << " fail=" << nEnsureFailed
                << " create=" << nEnsureCreated << " modify=" << nEnsureModified << "\n"
                << "          Map size=" << testMap.size()
                );

            CPPUNIT_CHECK_EX( nDelValueFailed == 0, "Functor del failed=" << nDelValueFailed );
            CPPUNIT_CHECK_EX( nDelValueSuccess == nDeleteSuccess,  "Delete success=" << nDeleteSuccess << " functor=" << nDelValueSuccess );

            CPPUNIT_CHECK( nEnsureFailed == 0 );

            CPPUNIT_CHECK_EX( nEnsureCreated == nEnsFuncCreated, "Ensure created=" << nEnsureCreated << " functor=" << nEnsFuncCreated );
            CPPUNIT_CHECK_EX( nEnsureModified == nEnsFuncModified, "Ensure modified=" << nEnsureModified << " functor=" << nEnsFuncModified );

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
        void test()
        {
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " ensure=" << c_nEnsureThreadCount
                << " pass count=" << c_nThreadPassCount
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
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " ensure=" << c_nEnsureThreadCount
                << " pass count=" << c_nThreadPassCount
                << " map size=" << c_nMapSize
                );

            Map testMap;
            do_test( testMap );
            if ( c_bPrintGCState )
                print_gc_state();
        }

        typedef CppUnitMini::TestCase Base;
        void setUpParams( const CppUnitMini::TestCfg& cfg );

        void run_MichaelMap(const char *in_name, bool invert = false);
        void run_SplitList(const char *in_name, bool invert = false);
        void run_StripedMap(const char *in_name, bool invert = false);
        void run_RefinableMap(const char *in_name, bool invert = false);
        void run_CuckooMap(const char *in_name, bool invert = false);
        void run_SkipListMap(const char *in_name, bool invert = false);
        void run_EllenBinTreeMap(const char *in_name, bool invert = false);
        void run_BronsonAVLTreeMap(const char *in_name, bool invert = false);

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
    };
} // namespace map2
