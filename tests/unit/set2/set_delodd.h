//$$CDS-header$$

#include "cppunit/thread.h"
#include "set2/set_type.h"

namespace set2 {

#    define TEST_SET(IMPL, C, X)         void C::X() { test<set_type<IMPL, key_type, value_type>::X >(); }
#    define TEST_SET_EXTRACT(IMPL, C, X) void C::X() { test_extract<set_type<IMPL, key_type, value_type>::X >(); }
#    define TEST_SET_NOLF(IMPL, C, X)    void C::X() { test_nolf<set_type<IMPL, key_type, value_type>::X >(); }
#    define TEST_SET_NOLF_EXTRACT(IMPL, C, X) void C::X() { test_nolf_extract<set_type<IMPL, key_type, value_type>::X >(); }

    namespace {
        struct key_thread
        {
            size_t  nKey;
            size_t  nThread;

            key_thread( size_t key, size_t threadNo )
                : nKey( key )
                , nThread( threadNo )
            {}

            key_thread()
            {}
        };

        typedef set_type_base<key_thread, size_t>::key_val     key_value_pair;
    }

    template <>
    struct cmp<key_thread> {
        int operator ()(key_thread const& k1, key_thread const& k2) const
        {
            if ( k1.nKey < k2.nKey )
                return -1;
            if ( k1.nKey > k2.nKey )
                return 1;
            if ( k1.nThread < k2.nThread )
                return -1;
            if ( k1.nThread > k2.nThread )
                return 1;
            return 0;
        }
        int operator ()(key_thread const& k1, size_t k2) const
        {
            if ( k1.nKey < k2 )
                return -1;
            if ( k1.nKey > k2 )
                return 1;
            return 0;
        }
        int operator ()(size_t k1, key_thread const& k2) const
        {
            if ( k1 < k2.nKey )
                return -1;
            if ( k1 > k2.nKey )
                return 1;
            return 0;
        }
    };

} // namespace set2

namespace std {
    template <>
    struct less<set2::key_thread>
    {
        bool operator()(set2::key_thread const& k1, set2::key_thread const& k2) const
        {
            if ( k1.nKey <= k2.nKey )
                return k1.nKey < k2.nKey || k1.nThread < k2.nThread;
            return false;
        }
    };

    template <>
    struct hash<set2::key_thread>
    {
        typedef size_t              result_type;
        typedef set2::key_thread    argument_type;

        size_t operator()( set2::key_thread const& k ) const
        {
            return std::hash<size_t>()(k.nKey);
        }
        size_t operator()( size_t k ) const
        {
            return std::hash<size_t>()(k);
        }
    };

} // namespace std

namespace boost {
    inline size_t hash_value( set2::key_thread const& k )
    {
        return std::hash<size_t>()( k.nKey );
    }

    template <>
    struct hash<set2::key_thread>
    {
        typedef size_t              result_type;
        typedef set2::key_thread    argument_type;

        size_t operator()(set2::key_thread const& k) const
        {
            return boost::hash<size_t>()( k.nKey );
        }
        size_t operator()(size_t k) const
        {
            return boost::hash<size_t>()( k );
        }
    };
} // namespace boost

namespace set2 {

    class Set_DelOdd: public CppUnitMini::TestCase
    {
        static size_t  c_nSetSize;          // max set size
        static size_t  c_nInsThreadCount;   // insert thread count
        static size_t  c_nDelThreadCount;   // delete thread count
        static size_t  c_nExtractThreadCount;  // extract thread count
        static size_t  c_nMaxLoadFactor;    // maximum load factor
        static bool    c_bPrintGCState;

        std::vector<size_t>     m_arrData;

    protected:
        typedef CppUnitMini::TestCase Base;
        typedef key_thread  key_type;
        typedef size_t      value_type;

        atomics::atomic<size_t>      m_nInsThreadCount;

        // Inserts keys from [0..N)
        template <class Set>
        class InsertThread: public CppUnitMini::TestThread
        {
            Set&     m_Set;

            virtual InsertThread *    clone()
            {
                return new InsertThread( *this );
            }

            struct ensure_func
            {
                template <typename Q>
                void operator()( bool /*bNew*/, key_value_pair const&, Q const& )
                {}
            };
        public:
            size_t  m_nInsertSuccess;
            size_t  m_nInsertFailed;

        public:
            InsertThread( CppUnitMini::ThreadPool& pool, Set& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Set( rMap )
            {}
            InsertThread( InsertThread& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_DelOdd&  getTest()
            {
                return reinterpret_cast<Set_DelOdd&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nInsertSuccess =
                    m_nInsertFailed = 0;

                std::vector<size_t>& arrData = getTest().m_arrData;
                for ( size_t i = 0; i < arrData.size(); ++i ) {
                    if ( rSet.insert( key_type( arrData[i], m_nThreadNo )))
                        ++m_nInsertSuccess;
                    else
                        ++m_nInsertFailed;
                }

                ensure_func f;
                for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                    if ( arrData[i] & 1 ) {
                        rSet.ensure( key_type( arrData[i], m_nThreadNo ), f );
                    }
                }

                getTest().m_nInsThreadCount.fetch_sub( 1, atomics::memory_order_release );
            }
        };

        struct key_equal {
            bool operator()( key_type const& k1, key_type const& k2 ) const
            {
                return k1.nKey == k2.nKey;
            }
            bool operator()( size_t k1, key_type const& k2 ) const
            {
                return k1 == k2.nKey;
            }
            bool operator()( key_type const& k1, size_t k2 ) const
            {
                return k1.nKey == k2;
            }
            bool operator ()( key_value_pair const& k1, key_value_pair const& k2 ) const
            {
                return operator()( k1.key, k2.key );
            }
            bool operator ()( key_value_pair const& k1, key_type const& k2 ) const
            {
                return operator()( k1.key, k2 );
            }
            bool operator ()( key_type const& k1, key_value_pair const& k2 ) const
            {
                return operator()( k1, k2.key );
            }
            bool operator ()( key_value_pair const& k1, size_t k2 ) const
            {
                return operator()( k1.key, k2 );
            }
            bool operator ()( size_t k1, key_value_pair const& k2 ) const
            {
                return operator()( k1, k2.key );
            }
        };

        struct key_less {
            bool operator()( key_type const& k1, key_type const& k2 ) const
            {
                return k1.nKey < k2.nKey;
            }
            bool operator()( size_t k1, key_type const& k2 ) const
            {
                return k1 < k2.nKey;
            }
            bool operator()( key_type const& k1, size_t k2 ) const
            {
                return k1.nKey < k2;
            }
            bool operator ()( key_value_pair const& k1, key_value_pair const& k2 ) const
            {
                return operator()( k1.key, k2.key );
            }
            bool operator ()( key_value_pair const& k1, key_type const& k2 ) const
            {
                return operator()( k1.key, k2 );
            }
            bool operator ()( key_type const& k1, key_value_pair const& k2 ) const
            {
                return operator()( k1, k2.key );
            }
            bool operator ()( key_value_pair const& k1, size_t k2 ) const
            {
                return operator()( k1.key, k2 );
            }
            bool operator ()( size_t k1, key_value_pair const& k2 ) const
            {
                return operator()( k1, k2.key );
            }

            typedef key_equal   equal_to;
        };

        // Deletes odd keys from [0..N)
        template <class Set>
        class DeleteThread: public CppUnitMini::TestThread
        {
            Set&     m_Set;

            virtual DeleteThread *    clone()
            {
                return new DeleteThread( *this );
            }
        public:
            size_t  m_nDeleteSuccess;
            size_t  m_nDeleteFailed;

        public:
            DeleteThread( CppUnitMini::ThreadPool& pool, Set& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Set( rMap )
            {}
            DeleteThread( DeleteThread& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_DelOdd&  getTest()
            {
                return reinterpret_cast<Set_DelOdd&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                std::vector<size_t>& arrData = getTest().m_arrData;
                if ( m_nThreadNo & 1 ) {
                    for ( size_t k = 0; k < c_nInsThreadCount; ++k ) {
                        for ( size_t i = 0; i < arrData.size(); ++i ) {
                            if ( arrData[i] & 1 ) {
                                if ( rSet.erase_with( arrData[i], key_less() ))
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                        }
                        if ( getTest().m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
                else {
                    for ( size_t k = 0; k < c_nInsThreadCount; ++k ) {
                        for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                            if ( arrData[i] & 1 ) {
                                if ( rSet.erase_with( arrData[i], key_less() ))
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                        }
                        if ( getTest().m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
            }
        };

        // Extracts odd keys from [0..N)
        template <typename GC, class Set>
        class ExtractThread: public CppUnitMini::TestThread
        {
            Set&     m_Set;

            virtual ExtractThread *    clone()
            {
                return new ExtractThread( *this );
            }
        public:
            size_t  m_nExtractSuccess;
            size_t  m_nExtractFailed;

        public:
            ExtractThread( CppUnitMini::ThreadPool& pool, Set& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Set( rMap )
            {}
            ExtractThread( ExtractThread& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_DelOdd&  getTest()
            {
                return reinterpret_cast<Set_DelOdd&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nExtractSuccess =
                    m_nExtractFailed = 0;

                typename Set::guarded_ptr gp;

                std::vector<size_t>& arrData = getTest().m_arrData;
                if ( m_nThreadNo & 1 ) {
                    for ( size_t k = 0; k < c_nInsThreadCount; ++k ) {
                        for ( size_t i = 0; i < arrData.size(); ++i ) {
                            if ( arrData[i] & 1 ) {
                                gp = rSet.extract_with( arrData[i], key_less());
                                if ( gp )
                                    ++m_nExtractSuccess;
                                else
                                    ++m_nExtractFailed;
                                gp.release();
                            }
                        }
                        if ( getTest().m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
                else {
                    for ( size_t k = 0; k < c_nInsThreadCount; ++k ) {
                        for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                            if ( arrData[i] & 1 ) {
                                gp = rSet.extract_with( arrData[i], key_less());
                                if ( gp )
                                    ++m_nExtractSuccess;
                                else
                                    ++m_nExtractFailed;
                                gp.release();
                            }
                        }
                        if ( getTest().m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
            }
        };

        template <typename RCU, class Set>
        class ExtractThread< cds::urcu::gc<RCU>, Set >: public CppUnitMini::TestThread
        {
            Set&     m_Set;

            virtual ExtractThread *    clone()
            {
                return new ExtractThread( *this );
            }
        public:
            size_t  m_nExtractSuccess;
            size_t  m_nExtractFailed;

        public:
            ExtractThread( CppUnitMini::ThreadPool& pool, Set& rMap )
                : CppUnitMini::TestThread( pool )
                , m_Set( rMap )
            {}
            ExtractThread( ExtractThread& src )
                : CppUnitMini::TestThread( src )
                , m_Set( src.m_Set )
            {}

            Set_DelOdd&  getTest()
            {
                return reinterpret_cast<Set_DelOdd&>( m_Pool.m_Test );
            }

            virtual void init() { cds::threading::Manager::attachThread()   ; }
            virtual void fini() { cds::threading::Manager::detachThread()   ; }

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nExtractSuccess =
                    m_nExtractFailed = 0;

                typename Set::exempt_ptr xp;

                std::vector<size_t>& arrData = getTest().m_arrData;
                if ( m_nThreadNo & 1 ) {
                    for ( size_t k = 0; k < c_nInsThreadCount; ++k ) {
                        for ( size_t i = 0; i < arrData.size(); ++i ) {
                            if ( arrData[i] & 1 ) {
                                if ( Set::c_bExtractLockExternal ) {
                                    typename Set::rcu_lock l;
                                    xp = rSet.extract_with( arrData[i], key_less() );
                                    if ( xp )
                                        ++m_nExtractSuccess;
                                    else
                                        ++m_nExtractFailed;
                                }
                                else {
                                    xp = rSet.extract_with( arrData[i], key_less() );
                                    if ( xp )
                                        ++m_nExtractSuccess;
                                    else
                                        ++m_nExtractFailed;
                                }
                                xp.release();
                            }
                        }
                        if ( getTest().m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
                else {
                    for ( size_t k = 0; k < c_nInsThreadCount; ++k ) {
                        for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                            if ( arrData[i] & 1 ) {
                                if ( Set::c_bExtractLockExternal ) {
                                    typename Set::rcu_lock l;
                                    xp = rSet.extract_with( arrData[i], key_less() );
                                    if ( xp )
                                        ++m_nExtractSuccess;
                                    else
                                        ++m_nExtractFailed;
                                }
                                else {
                                    xp = rSet.extract_with( arrData[i], key_less() );
                                    if ( xp )
                                        ++m_nExtractSuccess;
                                    else
                                        ++m_nExtractFailed;
                                }
                                xp.release();
                            }
                        }
                        if ( getTest().m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
            }
        };

    protected:
        template <class Set>
        void do_test( size_t nLoadFactor )
        {
            Set  testSet( c_nSetSize, nLoadFactor );
            do_test_with( testSet );
            analyze( testSet );
        }

        template <class Set>
        void do_test_extract( size_t nLoadFactor )
        {
            Set  testSet( c_nSetSize, nLoadFactor );
            do_test_extract_with( testSet );
            analyze( testSet );
        }

        template <class Set>
        void do_test_with( Set& testSet )
        {
            typedef InsertThread<Set> insert_thread;
            typedef DeleteThread<Set> delete_thread;

            m_nInsThreadCount.store( c_nInsThreadCount, atomics::memory_order_release );

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new insert_thread( pool, testSet ), c_nInsThreadCount );
            pool.add( new delete_thread( pool, testSet ), c_nDelThreadCount ? c_nDelThreadCount : cds::OS::topology::processor_count());
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                insert_thread * pThread = dynamic_cast<insert_thread *>( *it );
                if ( pThread ) {
                    nInsertSuccess += pThread->m_nInsertSuccess;
                    nInsertFailed += pThread->m_nInsertFailed;
                }
                else {
                    delete_thread * p = static_cast<delete_thread *>( *it );
                    nDeleteSuccess += p->m_nDeleteSuccess;
                    nDeleteFailed += p->m_nDeleteFailed;
                }
            }

            CPPUNIT_CHECK( nInsertSuccess == c_nSetSize * c_nInsThreadCount );
            CPPUNIT_CHECK( nInsertFailed == 0 );

            CPPUNIT_MSG( "  Totals (success/failed): \n\t"
                      << "      Insert=" << nInsertSuccess << '/' << nInsertFailed << "\n\t"
                      << "      Delete=" << nDeleteSuccess << '/' << nDeleteFailed << "\n\t"
            );
        }

        template <class Set>
        void do_test_extract_with( Set& testSet )
        {
            typedef InsertThread<Set> insert_thread;
            typedef DeleteThread<Set> delete_thread;
            typedef ExtractThread< typename Set::gc, Set > extract_thread;

            m_nInsThreadCount.store( c_nInsThreadCount, atomics::memory_order_release );

            CppUnitMini::ThreadPool pool( *this );
            pool.add( new insert_thread( pool, testSet ), c_nInsThreadCount );
            if ( c_nDelThreadCount )
                pool.add( new delete_thread( pool, testSet ), c_nDelThreadCount );
            if ( c_nExtractThreadCount )
                pool.add( new extract_thread( pool, testSet ), c_nExtractThreadCount );
            pool.run();
            CPPUNIT_MSG( "   Duration=" << pool.avgDuration() );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nExtractSuccess = 0;
            size_t nExtractFailed = 0;
            for ( CppUnitMini::ThreadPool::iterator it = pool.begin(); it != pool.end(); ++it ) {
                insert_thread * pThread = dynamic_cast<insert_thread *>( *it );
                if ( pThread ) {
                    nInsertSuccess += pThread->m_nInsertSuccess;
                    nInsertFailed += pThread->m_nInsertFailed;
                }
                else {
                    delete_thread * p = dynamic_cast<delete_thread *>( *it );
                    if ( p ) {
                        nDeleteSuccess += p->m_nDeleteSuccess;
                        nDeleteFailed += p->m_nDeleteFailed;
                    }
                    else {
                        extract_thread * pExt = dynamic_cast<extract_thread *>( *it );
                        assert( pExt );
                        nExtractSuccess += pExt->m_nExtractSuccess;
                        nExtractFailed += pExt->m_nExtractFailed;
                    }
                }
            }

            CPPUNIT_CHECK( nInsertSuccess == c_nSetSize * c_nInsThreadCount );
            CPPUNIT_CHECK( nInsertFailed == 0 );

            CPPUNIT_MSG( "  Totals (success/failed): \n\t"
                << "      Insert=" << nInsertSuccess << '/' << nInsertFailed << "\n\t"
                << "      Delete=" << nDeleteSuccess << '/' << nDeleteFailed << "\n\t"
                << "      Extract=" << nExtractSuccess << '/' << nExtractFailed << "\n\t"
                );
        }

        template <typename Set>
        void analyze( Set& testSet )
        {
            // All even keys must be in the set
            {
                CPPUNIT_MSG( "  Check even keys..." );
                size_t nErrorCount = 0;
                for ( size_t n = 0; n < c_nSetSize; n +=2 ) {
                    for ( size_t i = 0; i < c_nInsThreadCount; ++i ) {
                        if ( !testSet.find( key_type(n, i) ) ) {
                            if ( ++nErrorCount < 10 ) {
                                CPPUNIT_MSG( "key " << n << "-" << i << " is not found!");
                            }
                        }
                    }
                }
                CPPUNIT_CHECK_EX( nErrorCount == 0, "Totals: " << nErrorCount << " keys is not found");
            }

            check_before_clear( testSet );

            CPPUNIT_MSG( "  Clear map (single-threaded)..." );
            cds::OS::Timer    timer;
            testSet.clear();
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_CHECK_EX( testSet.empty(), ((long long) testSet.size()) );

            additional_check( testSet );
            print_stat( testSet );
            additional_cleanup( testSet );
        }

        template <class Set>
        void test()
        {
            CPPUNIT_MSG( "Insert thread count=" << c_nInsThreadCount
                << " delete thread count=" << c_nDelThreadCount
                << " set size=" << c_nSetSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                CPPUNIT_MSG( "Load factor=" << nLoadFactor );
                do_test<Set>( nLoadFactor );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Set>
        void test_extract()
        {
            CPPUNIT_MSG( "Insert thread count=" << c_nInsThreadCount
                << " delete thread count=" << c_nDelThreadCount
                << " extract thread count=" << c_nExtractThreadCount
                << " set size=" << c_nSetSize
                );

            for ( size_t nLoadFactor = 1; nLoadFactor <= c_nMaxLoadFactor; nLoadFactor *= 2 ) {
                CPPUNIT_MSG( "Load factor=" << nLoadFactor );
                do_test_extract<Set>( nLoadFactor );
                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Set>
        void test_nolf()
        {
            CPPUNIT_MSG( "Insert thread count=" << c_nInsThreadCount
                << " delete thread count=" << c_nDelThreadCount
                << " set size=" << c_nSetSize
                );

            {
                Set s;
                do_test_with( s );
                analyze( s );
            }

            if ( c_bPrintGCState )
                print_gc_state();
        }

        template <class Set>
        void test_nolf_extract()
        {
            CPPUNIT_MSG( "Insert thread count=" << c_nInsThreadCount
                << " delete thread count=" << c_nDelThreadCount
                << " extract thread count=" << c_nExtractThreadCount
                << " set size=" << c_nSetSize
                );

            {
                Set s;
                do_test_extract_with( s );
                analyze( s );
            }

            if ( c_bPrintGCState )
                print_gc_state();
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );

        void run_MichaelSet(const char *in_name, bool invert = false);
        void run_SplitList(const char *in_name, bool invert = false);
        void run_CuckooSet(const char *in_name, bool invert = false);
        void run_SkipListSet(const char *in_name, bool invert = false);
        void run_EllenBinTreeSet(const char *in_name, bool invert = false);

        virtual void myRun(const char *in_name, bool invert = false);


#   include "set2/set_defs.h"
        CDSUNIT_DECLARE_MichaelSet
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_CuckooSet
        CDSUNIT_DECLARE_SkipListSet
        CDSUNIT_DECLARE_EllenBinTreeSet
    };
} // namespace set2
