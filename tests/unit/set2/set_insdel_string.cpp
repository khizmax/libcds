/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set2/set_types.h"
#include "cppunit/thread.h"

#include <vector>

namespace set2 {

#   define TEST_SET(X)          void X() { test<SetTypes<key_type, value_type>::X >(); }
#   define TEST_SET_EXTRACT(X)  void X() { test_extract<SetTypes<key_type, value_type>::X >(); }
#   define TEST_SET_NOLF(X)     void X() { test_nolf<SetTypes<key_type, value_type>::X >(); }
#   define TEST_SET_NOLF_EXTRACT(X) void X() { test_nolf_extract<SetTypes<key_type, value_type>::X >(); }

    namespace {
        static size_t  c_nMapSize = 1000000    ;  // set size
        static size_t  c_nInsertThreadCount = 4;  // count of insertion thread
        static size_t  c_nDeleteThreadCount = 4;  // count of deletion thread
        static size_t  c_nThreadPassCount = 4  ;  // pass count for each thread
        static size_t  c_nMaxLoadFactor = 8    ;  // maximum load factor
        static bool    c_bPrintGCState = true;
    }

    class Set_InsDel_string: public CppUnitMini::TestCase
    {
        typedef std::string key_type;
        typedef size_t      value_type;

        const std::vector<std::string> *  m_parrString;

        template <class Set>
        class Inserter: public CppUnitMini::TestThread
        {
            Set&     m_Set;
            typedef typename Set::value_type    keyval_type;

            virtual Inserter *    clone()
            {
                return new Inserter( *this );
            }
        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;

        public:
            Inserter( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Inserter( Inserter& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_InsDel_string&  getTest()
            {
                return reinterpret_cast<Set_InsDel_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nInsertSuccess =
                    m_nInsertFailed = 0;

                const std::vector<std::string>& arrString = *getTest().m_parrString;
                size_t nArrSize = arrString.size();

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            if ( rSet.insert( keyval_type(arrString[nItem % nArrSize], nItem * 8) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            if ( rSet.insert( keyval_type( arrString[nItem % nArrSize], nItem * 8) ) )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
            }
        };

        template <class Set>
        class Deleter: public CppUnitMini::TestThread
        {
            Set&     m_Set;

            virtual Deleter *    clone()
            {
                return new Deleter( *this );
            }
        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

        public:
            Deleter( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Deleter( Deleter& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_InsDel_string&  getTest()
            {
                return reinterpret_cast<Set_InsDel_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                const std::vector<std::string>& arrString = *getTest().m_parrString;
                size_t nArrSize = arrString.size();

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            if ( rSet.erase( arrString[nItem % nArrSize] ) )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            if ( rSet.erase( arrString[nItem % nArrSize] ) )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
            }
        };

        template <typename GC, class Set>
        class Extractor: public CppUnitMini::TestThread
        {
            Set&     m_Set;

            virtual Extractor *    clone()
            {
                return new Extractor( *this );
            }
        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

        public:
            Extractor( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Extractor( Extractor& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_InsDel_string&  getTest()
            {
                return reinterpret_cast<Set_InsDel_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                typename Set::guarded_ptr gp;

                const std::vector<std::string>& arrString = *getTest().m_parrString;
                size_t nArrSize = arrString.size();

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            gp = rSet.extract( arrString[nItem % nArrSize]);
                            if (  gp )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                            gp.release();
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            gp = rSet.extract( arrString[nItem % nArrSize]);
                            if ( gp )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                            gp.release();
                        }
                    }
                }
            }
        };

        template <typename RCU, class Set>
        class Extractor<cds::urcu::gc<RCU>, Set >: public CppUnitMini::TestThread
        {
            Set&     m_Set;

            virtual Extractor *    clone()
            {
                return new Extractor( *this );
            }
        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

        public:
            Extractor( CppUnitMini::ThreadPool& pool, Set& rSet )
                : CppUnitMini::TestThread( pool )
                , m_Set( rSet )
            {}
            Extractor( Extractor& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_InsDel_string&  getTest()
            {
                return reinterpret_cast<Set_InsDel_string&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                typename Set::exempt_ptr xp;

                const std::vector<std::string>& arrString = *getTest().m_parrString;
                size_t nArrSize = arrString.size();

                if ( m_nThreadNo & 1 ) {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < c_nMapSize; ++nItem ) {
                            if ( Set::c_bExtractLockExternal ) {
                                {
                                    typename Set::rcu_lock l;
                                    xp = rSet.extract( arrString[nItem % nArrSize] );
                                    if ( xp )
                                        ++m_nDeleteSuccess;
                                    else
                                        ++m_nDeleteFailed;
                                }
                            }
                            else {
                                xp = rSet.extract( arrString[nItem % nArrSize] );
                                if ( xp )
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                            xp.release();
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < c_nThreadPassCount; ++nPass ) {
                        for ( size_t nItem = c_nMapSize; nItem > 0; --nItem ) {
                            if ( Set::c_bExtractLockExternal ) {
                                {
                                    typename Set::rcu_lock l;
                                    xp = rSet.extract( arrString[nItem % nArrSize] );
                                    if ( xp )
                                        ++m_nDeleteSuccess;
                                    else
                                        ++m_nDeleteFailed;
                                }
                            }
                            else {
                                xp = rSet.extract( arrString[nItem % nArrSize] );
                                if ( xp )
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                            xp.release();
                        }
                    }
                }
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
        void do_test_extract( size_t nLoadFactor )
        {
            CPPUNIT_MSG( "Load factor=" << nLoadFactor );

            Set  testSet( c_nMapSize, nLoadFactor );
            do_test_extract_with( testSet );
        }

        template <class Set>
        void do_test_with( Set& testSet )
        {
            typedef Inserter<Set>       InserterThread;
            typedef Deleter<Set>        DeleterThread;
            cds::OS::Timer    timer;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testSet ), c_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testSet ), c_nDeleteThreadCount );
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
                << " Set size=" << testSet.size()
                );


            CPPUNIT_MSG( "  Clear set (single-threaded)..." );
            timer.reset();
            for ( size_t i = 0; i < m_parrString->size(); ++i )
                testSet.erase( (*m_parrString)[i] );
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_ASSERT( testSet.empty() );

            additional_check( testSet );
            print_stat(  testSet  );
            additional_cleanup( testSet );
        }

        template <class Set>
        void do_test_extract_with( Set& testSet )
        {
            typedef Inserter<Set>       InserterThread;
            typedef Deleter<Set>        DeleterThread;
            typedef Extractor<typename Set::gc, Set> ExtractThread;

            size_t nDelThreadCount = c_nDeleteThreadCount / 2;

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new InserterThread( pool, testSet ), c_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testSet ), nDelThreadCount );
            pool.add( new ExtractThread( pool, testSet ), c_nDeleteThreadCount - nDelThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nExtractSuccess = 0;
            size_t nExtractFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                InserterThread * pThread = dynamic_cast<InserterThread *>( *it );
                if ( pThread ) {
                    nInsertSuccess += pThread->m_nInsertSuccess;
                    nInsertFailed += pThread->m_nInsertFailed;
                }
                else {
                    DeleterThread * p = dynamic_cast<DeleterThread *>( *it );
                    if ( p ) {
                        nDeleteSuccess += p->m_nDeleteSuccess;
                        nDeleteFailed += p->m_nDeleteFailed;
                    }
                    else {
                        ExtractThread * pExtract = dynamic_cast<ExtractThread *>( *it );
                        assert( pExtract );
                        nExtractSuccess += pExtract->m_nDeleteSuccess;
                        nExtractFailed += pExtract->m_nDeleteFailed;
                    }
                }
            }

            CPPUNIT_MSG( "    Totals: Ins succ=" << nInsertSuccess
                << " Del succ=" << nDeleteSuccess
                << " Extract succ= " << nExtractSuccess << "\n"
                << "          : Ins fail=" << nInsertFailed
                << " Del fail=" << nDeleteFailed
                << " Extract fail=" << nExtractFailed
                << " Set size=" << testSet.size()
                );


            CPPUNIT_MSG( "  Clear set (single-threaded)..." );
            cds::OS::Timer    timer;
            for ( size_t i = 0; i < m_parrString->size(); ++i )
                testSet.erase( (*m_parrString)[i] );
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_ASSERT( testSet.empty() );

            additional_check( testSet );
            print_stat(  testSet  );
            additional_cleanup( testSet );
        }

        template <class Set>
        void test()
        {
            m_parrString = &CppUnitMini::TestCase::getTestStrings();

            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " pass count=" << c_nThreadPassCount
                << " set size=" << c_nMapSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                do_test<Set>( nLoadFactor );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Set>
        void test_extract()
        {
            m_parrString = &CppUnitMini::TestCase::getTestStrings();

            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " pass count=" << c_nThreadPassCount
                << " set size=" << c_nMapSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                do_test_extract<Set>( nLoadFactor );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Set>
        void test_nolf()
        {
            m_parrString = &CppUnitMini::TestCase::getTestStrings();

            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " pass count=" << c_nThreadPassCount
                << " set size=" << c_nMapSize
                );

            Set s;
            do_test_with( s );
            if ( c_bPrintGCState )
                print_gc_state();
        }

        template <class Set>
        void test_nolf_extract()
        {
            m_parrString = &CppUnitMini::TestCase::getTestStrings();

            CPPUNIT_MSG( "Thread count: insert=" << c_nInsertThreadCount
                << " delete=" << c_nDeleteThreadCount
                << " pass count=" << c_nThreadPassCount
                << " set size=" << c_nMapSize
                );

            Set s;
            do_test_extract_with( s );
            if ( c_bPrintGCState )
                print_gc_state();
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg ) {
            c_nInsertThreadCount = cfg.getULong("InsertThreadCount", 4 );
            c_nDeleteThreadCount = cfg.getULong("DeleteThreadCount", 4 );
            c_nThreadPassCount = cfg.getULong("ThreadPassCount", 4 );
            c_nMapSize = cfg.getULong("MapSize", 1000000 );
            c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", 8 );
            c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
        }

#   include "set2/set_defs.h"
        CDSUNIT_DECLARE_MichaelSet
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_StripedSet
        CDSUNIT_DECLARE_RefinableSet
        CDSUNIT_DECLARE_CuckooSet
        CDSUNIT_DECLARE_SkipListSet
        CDSUNIT_DECLARE_EllenBinTreeSet
        CDSUNIT_DECLARE_StdSet

        CPPUNIT_TEST_SUITE_( Set_InsDel_string, "Map_InsDel_string" )
            CDSUNIT_TEST_MichaelSet
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListSet
            CDSUNIT_TEST_EllenBinTreeSet
            CDSUNIT_TEST_StripedSet
            CDSUNIT_TEST_RefinableSet
            CDSUNIT_TEST_CuckooSet
            CDSUNIT_TEST_StdSet
        CPPUNIT_TEST_SUITE_END()

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDel_string );
} // namespace set2
