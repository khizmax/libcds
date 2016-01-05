/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#include "cppunit/thread.h"
#include "set2/set_type.h"

namespace set2 {

#define TEST_CASE(TAG, X)  void X();

    namespace {
        struct key_thread
        {
            uint32_t  nKey;
            uint16_t  nThread;
            uint16_t  pad_;

            key_thread( size_t key, size_t threadNo )
                : nKey( static_cast<uint32_t>(key))
                , nThread( static_cast<uint16_t>(threadNo))
                , pad_(0)
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
    public:
        size_t  c_nSetSize =1000000;          // max set size
        size_t  c_nInsThreadCount = 4;   // insert thread count
        size_t  c_nDelThreadCount = 4;   // delete thread count
        size_t  c_nExtractThreadCount = 4;  // extract thread count
        size_t  c_nMaxLoadFactor = 8;    // maximum load factor
        bool    c_bPrintGCState = true;

        size_t  c_nCuckooInitialSize = 1024;// initial size for CuckooSet
        size_t  c_nCuckooProbesetSize = 16; // CuckooSet probeset size (only for list-based probeset)
        size_t  c_nCuckooProbesetThreshold = 0; // CUckooSet probeset threshold (0 - use default)

        size_t c_nFeldmanSet_HeadBits = 10;
        size_t c_nFeldmanSet_ArrayBits = 4;

        size_t c_nLoadFactor = 2;
        std::vector<size_t>     m_arrData;

    protected:
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

            struct update_functor
            {
                template <typename Q>
                void operator()( bool /*bNew*/, key_value_pair const&, Q const& )
                {}

                void operator()(key_value_pair& /*cur*/, key_value_pair * /*prev*/)
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

                update_functor f;
                for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                    if ( arrData[i] & 1 ) {
                        rSet.update( key_type( arrData[i], m_nThreadNo ), f, true );
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

            template <typename SetType, bool>
            struct eraser {
                static bool erase( SetType& s, size_t key, size_t /*thread*/)
                {
                    return s.erase_with( key, key_less() );
                }
            };

            template <typename SetType>
            struct eraser<SetType, true> {
                static bool erase(SetType& s, size_t key, size_t thread)
                {
                    return s.erase( key_type(key, thread));
                }
            };

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nDeleteSuccess =
                    m_nDeleteFailed = 0;

                size_t const nInsThreadCount = getTest().c_nInsThreadCount;
                std::vector<size_t>& arrData = getTest().m_arrData;

                if ( m_nThreadNo & 1 ) {
                    for (size_t i = 0; i < arrData.size(); ++i) {
                        if (arrData[i] & 1) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( eraser<Set, Set::c_bEraseExactKey>::erase( rSet, arrData[i], k ))
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
                    for (size_t i = arrData.size() - 1; i > 0; --i) {
                        if (arrData[i] & 1) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if (eraser<Set, Set::c_bEraseExactKey>::erase(rSet, arrData[i], k))
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

            template <typename SetType, bool>
            struct extractor {
                static typename SetType::guarded_ptr extract(SetType& s, size_t key, size_t /*thread*/)
                {
                    return s.extract_with( key, key_less());
                }
            };

            template <typename SetType>
            struct extractor<SetType, true> {
                static typename SetType::guarded_ptr extract(SetType& s, size_t key, size_t thread)
                {
                    return s.extract( key_type(key, thread));
                }
            };

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nExtractSuccess =
                    m_nExtractFailed = 0;

                typename Set::guarded_ptr gp;

                std::vector<size_t>& arrData = getTest().m_arrData;
                size_t const nInsThreadCount = getTest().c_nInsThreadCount;

                if ( m_nThreadNo & 1 ) {
                    for ( size_t i = 0; i < arrData.size(); ++i ) {
                        if (arrData[i] & 1) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                gp = extractor<Set, Set::c_bEraseExactKey>::extract( rSet, arrData[i], k );
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
                    for (size_t i = arrData.size() - 1; i > 0; --i) {
                        if (arrData[i] & 1) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                gp = extractor<Set, Set::c_bEraseExactKey>::extract( rSet, arrData[i], k);
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

            template <typename SetType, bool>
            struct extractor {
                static typename SetType::exempt_ptr extract(SetType& s, size_t key, size_t /*thread*/)
                {
                    return s.extract_with(key, key_less());
                }
            };

            template <typename SetType>
            struct extractor<SetType, true> {
                static typename SetType::exempt_ptr extract(SetType& s, size_t key, size_t thread)
                {
                    return s.extract(key_type(key, thread));
                }
            };

            virtual void test()
            {
                Set& rSet = m_Set;

                m_nExtractSuccess =
                    m_nExtractFailed = 0;

                typename Set::exempt_ptr xp;

                std::vector<size_t>& arrData = getTest().m_arrData;
                size_t const nInsThreadCount = getTest().c_nInsThreadCount;

                if ( m_nThreadNo & 1 ) {
                    for (size_t i = 0; i < arrData.size(); ++i) {
                        if (arrData[i] & 1) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( Set::c_bExtractLockExternal ) {
                                    typename Set::rcu_lock l;
                                    xp = extractor<Set, Set::c_bEraseExactKey>::extract( rSet, arrData[i], k);
                                    if ( xp )
                                        ++m_nExtractSuccess;
                                    else
                                        ++m_nExtractFailed;
                                }
                                else {
                                    xp = extractor<Set, Set::c_bEraseExactKey>::extract(rSet, arrData[i], k);
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
                    for (size_t i = arrData.size() - 1; i > 0; --i) {
                        if (arrData[i] & 1) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( Set::c_bExtractLockExternal ) {
                                    typename Set::rcu_lock l;
                                    xp = extractor<Set, Set::c_bEraseExactKey>::extract(rSet, arrData[i], k);
                                    if ( xp )
                                        ++m_nExtractSuccess;
                                    else
                                        ++m_nExtractFailed;
                                }
                                else {
                                    xp = extractor<Set, Set::c_bEraseExactKey>::extract(rSet, arrData[i], k);
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
                        if ( !testSet.contains( key_type(n, i) ) ) {
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
        void run_test()
        {
            static_assert( !Set::c_bExtractSupported, "Set class must not support extract() method" );

            CPPUNIT_MSG( "Insert thread count=" << c_nInsThreadCount
                << " delete thread count=" << c_nDelThreadCount
                << " set size=" << c_nSetSize
                );

            if ( Set::c_bLoadFactorDepended ) {
                for ( c_nLoadFactor = 1; c_nLoadFactor <= c_nMaxLoadFactor; c_nLoadFactor *= 2 ) {
                    CPPUNIT_MSG( "Load factor=" << c_nLoadFactor );

                    Set  testSet( *this );
                    do_test_with( testSet );
                    analyze( testSet );

                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Set  testSet( *this );
                do_test_with( testSet );
                analyze( testSet );

                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        template <class Set>
        void run_test_extract()
        {
            static_assert( Set::c_bExtractSupported, "Set class must support extract() method" );

            CPPUNIT_MSG( "Insert thread count=" << c_nInsThreadCount
                << " delete thread count=" << c_nDelThreadCount
                << " extract thread count=" << c_nExtractThreadCount
                << " set size=" << c_nSetSize
                );

            if ( Set::c_bLoadFactorDepended ) {
                for ( c_nLoadFactor = 1; c_nLoadFactor <= c_nMaxLoadFactor; c_nLoadFactor *= 2 ) {
                    CPPUNIT_MSG( "Load factor=" << c_nLoadFactor );

                    Set  testSet( *this );
                    do_test_extract_with( testSet );
                    analyze( testSet );

                    if ( c_bPrintGCState )
                        print_gc_state();
                }
            }
            else {
                Set  testSet( *this );
                do_test_extract_with( testSet );
                analyze( testSet );

                if ( c_bPrintGCState )
                    print_gc_state();
            }
        }

        void setUpParams( const CppUnitMini::TestCfg& cfg );
        virtual void endTestCase();

#   include "set2/set_defs.h"
        CDSUNIT_DECLARE_MichaelSet
        CDSUNIT_DECLARE_SplitList
        CDSUNIT_DECLARE_SkipListSet
        CDSUNIT_DECLARE_EllenBinTreeSet
        CDSUNIT_DECLARE_CuckooSet
        CDSUNIT_DECLARE_FeldmanHashSet_fixed
        CDSUNIT_DECLARE_FeldmanHashSet_city

        CPPUNIT_TEST_SUITE_(Set_DelOdd, "Map_DelOdd")
            CDSUNIT_TEST_MichaelSet
            CDSUNIT_TEST_SplitList
            CDSUNIT_TEST_SkipListSet
            CDSUNIT_TEST_EllenBinTreeSet
            CDSUNIT_TEST_FeldmanHashSet_fixed
            CDSUNIT_TEST_FeldmanHashSet_city
            CDSUNIT_TEST_CuckooSet
        CPPUNIT_TEST_SUITE_END();
    };
} // namespace set2
