//$$CDS-header$$

#include <functional>
#include <vector>
#include <mutex>    //unique_lock

#include "set2/set_type.h"
#include "cppunit/thread.h"
#include <cds/sync/spinlock.h>

namespace set2 {

#define TEST_CASE(TAG, X)  void X();

    class Set_InsDel_func: public CppUnitMini::TestCase
    {
    public:
        size_t  c_nSetSize = 1000000;      // set size
        size_t  c_nInsertThreadCount = 4;  // count of insertion thread
        size_t  c_nDeleteThreadCount = 4;  // count of deletion thread
        size_t  c_nUpdateThreadCount = 4;  // count of ensure thread
        size_t  c_nThreadPassCount = 4;    // pass count for each thread
        size_t  c_nMaxLoadFactor = 8;      // maximum load factor
        bool    c_bPrintGCState;

        size_t  c_nCuckooInitialSize = 1024;// initial size for CuckooSet
        size_t  c_nCuckooProbesetSize = 16; // CuckooSet probeset size (only for list-based probeset)
        size_t  c_nCuckooProbesetThreshold = 0; // CUckooSet probeset threshold (0 - use default)

        size_t c_nLoadFactor = 2;

    private:
        typedef size_t  key_type;
        struct value_type {
            size_t      nKey;
            size_t      nData;
            atomics::atomic<size_t> nUpdateCall;
            bool volatile   bInitialized;
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
                , bInitialized( s.bInitialized )
                , threadId( cds::OS::get_current_thread_id() )
            {}

            // boost::container::flat_map requires operator =
            value_type& operator=( value_type const& v )
            {
                nKey = v.nKey;
                nData = v.nData;
                nUpdateCall.store( v.nUpdateCall.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed );
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
                size_t const nPassCount = getTest().c_nThreadPassCount;

                // func is passed by reference
                insert_functor  func;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            if ( rSet.insert( *p, std::ref(func) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
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
        class Updater: public CppUnitMini::TestThread
        {
            Set&     m_Set;
            typedef typename Set::value_type keyval_type;

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
                        val.val.nUpdateCall.fetch_add( 1, atomics::memory_order_relaxed );
                        ++nModified;
                    }
                }
            private:
                update_functor(const update_functor& );
            };

        public:
            size_t  m_nUpdateFailed;
            size_t  m_nUpdateCreated;
            size_t  m_nUpdateExisted;
            size_t  m_nFunctorCreated;
            size_t  m_nFunctorModified;

        public:
            Updater( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Updater( Updater& src )
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

                m_nUpdateCreated =
                    m_nUpdateExisted =
                    m_nUpdateFailed = 0;

                size_t * pKeyFirst = getTest().m_pKeyFirst;
                size_t * pKeyLast = getTest().m_pKeyLast;
                size_t const nPassCount = getTest().c_nThreadPassCount;

                update_functor func;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t * p = pKeyFirst; p < pKeyLast; ++p ) {
                            std::pair<bool, bool> ret = rSet.update( *p, std::ref( func ), true );
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
                        for ( size_t * p = pKeyLast - 1 ; p >= pKeyFirst; --p ) {
                            std::pair<bool, bool> ret = rSet.update( *p, std::ref( func ), true );
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
                            std::unique_lock< typename value_type::lock_type> ac( item.val.m_access );
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
                size_t const nPassCount = getTest().c_nThreadPassCount;

                erase_functor   func;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
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
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
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
        void do_test( Set& testSet )
        {
            typedef Inserter<Set>       InserterThread;
            typedef Deleter<Set>        DeleterThread;
            typedef Updater<Set>        UpdaterThread;

            m_pKeyArr = new size_t[ c_nSetSize ];
            m_pKeyFirst = m_pKeyArr;
            m_pKeyLast = m_pKeyFirst + c_nSetSize;
            for ( size_t i = 0; i < c_nSetSize; ++i )
                m_pKeyArr[i] = i;
            shuffle( m_pKeyFirst, m_pKeyLast );

            cds::OS::Timer    timer;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testSet ), c_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testSet ), c_nDeleteThreadCount );
            pool.add( new UpdaterThread( pool, testSet ), c_nUpdateThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            delete [] m_pKeyArr;

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

            CPPUNIT_MSG(
                   "    Totals: Ins succ=" << nInsertSuccess
                << " Del succ=" << nDeleteSuccess << "\n"
                << "          : Ins fail=" << nInsertFailed
                << " Del fail=" << nDeleteFailed << "\n"
                << "          : Update succ=" << (nUpdateCreated + nUpdateModified) << " fail=" << nUpdateFailed
                << " create=" << nUpdateCreated << " modify=" << nUpdateModified << "\n"
                << "          Set size=" << testSet.size()
                );

            CPPUNIT_CHECK_EX( nDelValueFailed == 0, "Functor del failed=" << nDelValueFailed );
            CPPUNIT_CHECK_EX( nDelValueSuccess == nDeleteSuccess,  "Delete success=" << nDeleteSuccess << " functor=" << nDelValueSuccess );

            CPPUNIT_CHECK( nUpdateFailed == 0 );

            CPPUNIT_CHECK_EX( nUpdateCreated == nEnsFuncCreated, "Update created=" << nUpdateCreated << " functor=" << nEnsFuncCreated );
            CPPUNIT_CHECK_EX( nUpdateModified == nEnsFuncModified, "Update modified=" << nUpdateModified << " functor=" << nEnsFuncModified );

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
        void run_test()
        {
            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " ensure=" << c_nUpdateThreadCount
                << " pass count=" << c_nThreadPassCount
                << " set size=" << c_nSetSize
                );

            if ( Set::c_bLoadFactorDepended ) {
                for ( c_nLoadFactor = 1; c_nLoadFactor <= c_nMaxLoadFactor; c_nLoadFactor *= 2 ) {
                    Set s( *this );
                    do_test( s );
                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Set s( *this );
                do_test( s );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );

#   include "set2/set_defs.h"
    CDSUNIT_DECLARE_MichaelSet
    CDSUNIT_DECLARE_SkipListSet
    CDSUNIT_DECLARE_SplitList
    CDSUNIT_DECLARE_StripedSet
    CDSUNIT_DECLARE_RefinableSet
    CDSUNIT_DECLARE_CuckooSet
    CDSUNIT_DECLARE_EllenBinTreeSet

    CPPUNIT_TEST_SUITE_(Set_InsDel_func, "Map_InsDel_func")
        CDSUNIT_TEST_MichaelSet
        CDSUNIT_TEST_SplitList
        CDSUNIT_TEST_SkipListSet
        CDSUNIT_TEST_EllenBinTreeSet
        CDSUNIT_TEST_StripedSet
        CDSUNIT_TEST_RefinableSet
        CDSUNIT_TEST_CuckooSet

        //CDSUNIT_TEST_MultiLevelHashSet // the test is not suitable
    CPPUNIT_TEST_SUITE_END();

    };
} // namespace set2
