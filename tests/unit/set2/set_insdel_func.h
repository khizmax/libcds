//$$CDS-header$$

#include <functional>
#include <vector>
#include <mutex>    //unique_lock

#include "set2/set_type.h"
#include "cppunit/thread.h"
#include <cds/sync/spinlock.h>

namespace set2 {

#   define TEST_SET(IMPL, C, X)          void C::X() { test<set_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_SET_EXTRACT(IMPL, C, X)  TEST_SET(IMPL, C, X)
#   define TEST_SET_NOLF(IMPL, C, X)     void C::X() { test_nolf<set_type<IMPL, key_type, value_type>::X >(); }
#   define TEST_SET_NOLF_EXTRACT(IMPL, C, X) TEST_SET_NOLF(IMPL, C, X)

    class Set_InsDel_func: public CppUnitMini::TestCase
    {
        static size_t  c_nMapSize           ;  // map size
        static size_t  c_nInsertThreadCount ;  // count of insertion thread
        static size_t  c_nDeleteThreadCount ;  // count of deletion thread
        static size_t  c_nEnsureThreadCount ;  // count of ensure thread
        static size_t  c_nThreadPassCount   ;  // pass count for each thread
        static size_t  c_nMaxLoadFactor     ;  // maximum load factor
        static bool    c_bPrintGCState;

        typedef size_t  key_type;
        struct value_type {
            size_t      nKey;
            size_t      nData;
            atomics::atomic<size_t> nEnsureCall;
            bool volatile   bInitialized;
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
                , bInitialized( s.bInitialized )
                , threadId( cds::OS::get_current_thread_id() )
            {}

            // boost::container::flat_map requires operator =
            value_type& operator=( value_type const& v )
            {
                nKey = v.nKey;
                nData = v.nData;
                nEnsureCall.store( v.nEnsureCall.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
                bInitialized = v.bInitialized;

                return *this;
            }

        };


        size_t *    m_pKeyFirst;
        size_t *    m_pKeyLast;
        size_t *    m_pKeyArr;

        template <class Set>
        class Inserter: public CppUnitMini::TestThread
        {
            Set&     m_Set;
            typedef typename Set::value_type keyval_type;

            virtual Inserter *    clone()
            {
                return new Inserter( *this );
            }

            struct insert_functor {
                size_t nTestFunctorRef;

                insert_functor()
                    : nTestFunctorRef(0)
                {}

                void operator()( keyval_type& val )
                {
                    std::unique_lock< typename value_type::lock_type>    ac( val.val.m_access );

                    val.val.nKey  = val.key;
                    val.val.nData = val.key * 8;

                    ++nTestFunctorRef;
                    val.val.bInitialized = true;
                }
            };

        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;

            size_t  m_nTestFunctorRef;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_InsDel_func&  getTest()
            {
                return reinterpret_cast<Set_InsDel_func&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nInsertSuccess =
                    m_nInsertFailed =
                    m_nTestFunctorRef = 0;

                size_t * pKeyFirst = getTest().m_pKeyFirst;
                size_t * pKeyLast = getTest().m_pKeyLast;

                // func is passed by reference
                insert_functor  func;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            if ( rSet.insert( *p, std::ref(func) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t * p = pKeyLast - 1; p >= pKeyFirst; --p ) {
                            if ( rSet.insert( *p, std::ref(func) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }

                m_nTestFunctorRef = func.nTestFunctorRef;
            }
        };

        template <class Set>
        class Ensurer: public CppUnitMini::TestThread
        {
            Set&     m_Set;
            typedef typename Set::value_type keyval_type;

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

                void operator()( bool bNew, keyval_type& val, size_t /*nKey*/ )
                {
                    std::unique_lock<typename value_type::lock_type> ac( val.val.m_access );
                    if ( !val.val.bInitialized )
                    {
                        val.val.nKey = val.key;
                        val.val.nData = val.key * 8;
                        val.val.bInitialized = true;
                    }

                    if ( bNew ) {
                        ++nCreated;
                    }
                    else {
                        val.val.nEnsureCall.fetch_add( 1, atomics::memory_order_relaxed );
                        ++nModified;
                    }
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
            Ensurer( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Ensurer( Ensurer& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_InsDel_func&  getTest()
            {
                return reinterpret_cast<Set_InsDel_func&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nEnsureCreated =
                    m_nEnsureExisted =
                    m_nEnsureFailed = 0;

                size_t * pKeyFirst = getTest().m_pKeyFirst;
                size_t * pKeyLast = getTest().m_pKeyLast;

                ensure_functor func;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            std::pair<bool, bool> ret = rSet.ensure( *p, std::ref( func ) );
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
                        for ( size_t * p = pKeyLast - 1 ; p >= pKeyFirst; --p ) {
                            std::pair<bool, bool> ret = rSet.ensure( *p, std::ref( func ) );
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

        template <class Set>
        class Deleter: public CppUnitMini::TestThread
        {
            Set&     m_Set;
            typedef typename Set::value_type keyval_type;

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

                void operator ()( keyval_type const& itm )
                {
                    keyval_type& item = const_cast<keyval_type&>(itm);
                    while ( true ) {
                        bool bBkoff = false;
                        {
                            std::unique_lock< typename value_type::lock_type>    ac( item.val.m_access );
                            if ( item.val.bInitialized ) {
                                if ( m_cnt.nKeyExpected == item.val.nKey && m_cnt.nKeyExpected * 8 == item.val.nData )
                                    ++m_cnt.nSuccessItem;
                                else
                                    ++m_cnt.nFailedItem;
                                item.val.nData++;
                                item.val.nKey = 0;
                                break;
                            }
                            else
                                bBkoff = true;
                        }
                        if ( bBkoff )
                            cds::backoff::yield()();
                    }
                }
            };

        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

            size_t  m_nValueSuccess;
            size_t  m_nValueFailed;

        public:
            Deleter( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Deleter( Deleter& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_InsDel_func&  getTest()
            {
                return reinterpret_cast<Set_InsDel_func&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                size_t * pKeyFirst = getTest().m_pKeyFirst;
                size_t * pKeyLast = getTest().m_pKeyLast;

                erase_functor   func;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            func.m_cnt.nKeyExpected = *p;
                            if ( rSet.erase( *p, std::ref(func) ))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t * p = pKeyLast - 1; p >= pKeyFirst; --p ) {
                            func.m_cnt.nKeyExpected = *p;
                            if ( rSet.erase( *p, std::ref(func) ))
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
        template <class Set>
        void do_test( size_t nLoadFactor )
        {
            CPPUNIT_MSG( "Load factor=" << nLoadFactor );

            Set  testSet( c_nMapSize, nLoadFactor );
            do_test_with( testSet );
        }

        template <class Set>
        void do_test_with( Set& testSet )
        {
            typedef Inserter<Set>       InserterThread;
            typedef Deleter<Set>        DeleterThread;
            typedef Ensurer<Set>        EnsurerThread;

            m_pKeyArr = new size_t[ c_nMapSize ];
            m_pKeyFirst = m_pKeyArr;
            m_pKeyLast = m_pKeyFirst + c_nMapSize;
            for ( size_t i = 0; i < c_nMapSize; ++i )
                m_pKeyArr[i] = i;
            shuffle( m_pKeyFirst, m_pKeyLast );

            cds::OS::Timer    timer;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testSet ), c_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testSet ), c_nDeleteThreadCount );
            pool.add( new EnsurerThread( pool, testSet ), c_nEnsureThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            delete [] m_pKeyArr;

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

            CPPUNIT_MSG(
                   "    Totals: Ins succ=" << nInsertSuccess
                << " Del succ=" << nDeleteSuccess << "\n"
                << "          : Ins fail=" << nInsertFailed
                << " Del fail=" << nDeleteFailed << "\n"
                << "          : Ensure succ=" << (nEnsureCreated + nEnsureModified) << " fail=" << nEnsureFailed
                << " create=" << nEnsureCreated << " modify=" << nEnsureModified << "\n"
                << "          Set size=" << testSet.size()
                );

            CPPUNIT_CHECK_EX( nDelValueFailed == 0, "Functor del failed=" << nDelValueFailed );
            CPPUNIT_CHECK_EX( nDelValueSuccess == nDeleteSuccess,  "Delete success=" << nDeleteSuccess << " functor=" << nDelValueSuccess );

            CPPUNIT_CHECK( nEnsureFailed == 0 );

            CPPUNIT_CHECK_EX( nEnsureCreated == nEnsFuncCreated, "Ensure created=" << nEnsureCreated << " functor=" << nEnsFuncCreated );
            CPPUNIT_CHECK_EX( nEnsureModified == nEnsFuncModified, "Ensure modified=" << nEnsureModified << " functor=" << nEnsFuncModified );

            // nTestFunctorRef is call count of insert functor
            CPPUNIT_CHECK_EX( nTestFunctorRef == nInsertSuccess, "nInsertSuccess=" << nInsertSuccess << " functor nTestFunctorRef=" << nTestFunctorRef );

            CPPUNIT_MSG( "  Clear set (single-threaded)..." );
            timer.reset();
            testSet.clear();
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_CHECK( testSet.empty() );

            additional_check( testSet );
            print_stat(  testSet  );

            additional_cleanup( testSet );
        }

        template <class Set>
        void test()
        {
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " ensure=" << c_nEnsureThreadCount
                << " pass count=" << c_nThreadPassCount
                << " map size=" << c_nMapSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                do_test<Set>( nLoadFactor );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Set>
        void test_nolf()
        {
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " ensure=" << c_nEnsureThreadCount
                << " pass count=" << c_nThreadPassCount
                << " map size=" << c_nMapSize
                );

            Set s;
            do_test_with( s );
            if ( c_bPrintGCState )
                print_gc_state();
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            c_nInsertThreadCount = cfg.getULong("InsertThreadCount", 4 );
            c_nDeleteThreadCount = cfg.getULong("DeleteThreadCount", 4 );
            c_nEnsureThreadCount = cfg.getULong("EnsureThreadCount", 4 );
            c_nThreadPassCount = cfg.getULong("ThreadPassCount", 4 );
            c_nMapSize = cfg.getULong("MapSize", 1000000 );
            c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", 8 );
            c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
        }

        void run_MichaelSet(const char *in_name, bool invert = false);
        void run_SplitList(const char *in_name, bool invert = false);
        void run_StripedSet(const char *in_name, bool invert = false);
        void run_RefinableSet(const char *in_name, bool invert = false);
        void run_CuckooSet(const char *in_name, bool invert = false);
        void run_SkipListSet(const char *in_name, bool invert = false);
        void run_EllenBinTreeSet(const char *in_name, bool invert = false);

        typedef CppUnitMini::TestCase Base;
        virtual void myRun(const char *in_name, bool invert = false)
        {
            setUpParams( m_Cfg.get( "Map_InsDel_func" ));

            run_MichaelSet(in_name, invert);
            run_SplitList(in_name, invert);
            run_SkipListSet(in_name, invert);
            run_EllenBinTreeSet(in_name, invert);
            run_StripedSet(in_name, invert);
            run_RefinableSet(in_name, invert);
            run_CuckooSet(in_name, invert);

            endTestCase();
        }


#   include "set2/set_defs.h"
    CDSUNIT_DECLARE_MichaelSet
    CDSUNIT_DECLARE_SplitList
    CDSUNIT_DECLARE_StripedSet
    CDSUNIT_DECLARE_RefinableSet
    CDSUNIT_DECLARE_CuckooSet
    CDSUNIT_DECLARE_SkipListSet
    CDSUNIT_DECLARE_EllenBinTreeSet

    };
} // namespace set2
