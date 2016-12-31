/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#include "set_type.h"

namespace set {

#define TEST_CASE(TAG, X)  void X();

    class Set_InsDel_string: public cds_test::stress_fixture
    {
    public:
        static size_t s_nSetSize;               // set size
        static size_t s_nInsertThreadCount;     // count of insertion thread
        static size_t s_nDeleteThreadCount;     // count of deletion thread
        static size_t s_nThreadPassCount;       // pass count for each thread
        static size_t s_nMaxLoadFactor;         // maximum load factor

        static size_t s_nCuckooInitialSize;     // initial size for CuckooSet
        static size_t s_nCuckooProbesetSize;    // CuckooSet probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold; // CUckooSet probeset threshold (0 - use default)

        static size_t s_nFeldmanSet_HeadBits;
        static size_t s_nFeldmanSet_ArrayBits;

        static size_t s_nLoadFactor;
        static std::vector<std::string>  m_arrString;

        static void SetUpTestCase();
        static void TearDownTestCase();

    private:
        typedef std::string key_type;
        typedef size_t      value_type;

        enum {
            insert_thread,
            delete_thread,
            extract_thread
        };

        template <class Set>
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;

            Set&     m_Set;
            typedef typename Set::value_type    keyval_type;

        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;

        public:
            Inserter( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, insert_thread )
                , m_Set( set )
            {}

            Inserter( Inserter& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Inserter( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;

                Set_InsDel_string& fixture = pool().template fixture<Set_InsDel_string>();
                size_t nArrSize = m_arrString.size();
                size_t const nSetSize = fixture.s_nSetSize;
                size_t const nPassCount = fixture.s_nThreadPassCount;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < nSetSize; ++nItem ) {
                            if ( rSet.insert( keyval_type( m_arrString[nItem % nArrSize], nItem * 8 )))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = nSetSize; nItem > 0; --nItem ) {
                            if ( rSet.insert( keyval_type( m_arrString[nItem % nArrSize], nItem * 8 )))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
            }
        };

        template <class Set>
        class Deleter: public cds_test::thread
        {
            typedef cds_test::thread base_class;

            Set&     m_Set;
        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

        public:
            Deleter( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, delete_thread )
                , m_Set( set )
            {}

            Deleter( Deleter& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Deleter( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;

                Set_InsDel_string& fixture = pool().template fixture<Set_InsDel_string>();
                size_t nArrSize = m_arrString.size();
                size_t const nSetSize = fixture.s_nSetSize;
                size_t const nPassCount = fixture.s_nThreadPassCount;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < nSetSize; ++nItem ) {
                            if ( rSet.erase( m_arrString[nItem % nArrSize] ))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = nSetSize; nItem > 0; --nItem ) {
                            if ( rSet.erase( m_arrString[nItem % nArrSize] ))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
            }
        };

        template <typename GC, class Set>
        class Extractor: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Set&     m_Set;

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

        public:
            Extractor( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, extract_thread )
                , m_Set( set )
            {}

            Extractor( Extractor& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Extractor( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;

                typename Set::guarded_ptr gp;

                Set_InsDel_string& fixture = pool().template fixture<Set_InsDel_string>();
                size_t nArrSize = m_arrString.size();
                size_t const nSetSize = fixture.s_nSetSize;
                size_t const nPassCount = fixture.s_nThreadPassCount;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < nSetSize; ++nItem ) {
                            gp = rSet.extract( m_arrString[nItem % nArrSize] );
                            if ( gp )
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                            gp.release();
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = nSetSize; nItem > 0; --nItem ) {
                            gp = rSet.extract( m_arrString[nItem % nArrSize] );
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
        class Extractor<cds::urcu::gc<RCU>, Set >: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Set&     m_Set;

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

        public:
            Extractor( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, extract_thread )
                , m_Set( set )
            {}

            Extractor( Extractor& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Extractor( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;

                typename Set::exempt_ptr xp;

                Set_InsDel_string& fixture = pool().template fixture<Set_InsDel_string>();
                size_t nArrSize = m_arrString.size();
                size_t const nSetSize = fixture.s_nSetSize;
                size_t const nPassCount = fixture.s_nThreadPassCount;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = 0; nItem < nSetSize; ++nItem ) {
                            if ( Set::c_bExtractLockExternal ) {
                                typename Set::rcu_lock l;
                                xp = rSet.extract( m_arrString[nItem % nArrSize] );
                                if ( xp )
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                            else {
                                xp = rSet.extract( m_arrString[nItem % nArrSize] );
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
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( size_t nItem = nSetSize; nItem > 0; --nItem ) {
                            if ( Set::c_bExtractLockExternal ) {
                                typename Set::rcu_lock l;
                                xp = rSet.extract( m_arrString[nItem % nArrSize] );
                                if ( xp )
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                            else {
                                xp = rSet.extract( m_arrString[nItem % nArrSize] );
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
        void do_test( Set& testSet )
        {
            typedef Inserter<Set> InserterThread;
            typedef Deleter<Set>  DeleterThread;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new InserterThread( pool, testSet ), s_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testSet ), s_nDeleteThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsertThreadCount )
                << std::make_pair( "delete_thread_count", s_nDeleteThreadCount )
                << std::make_pair( "thread_pass_count", s_nThreadPassCount )
                << std::make_pair( "set_size", s_nSetSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type()) {
                case insert_thread:
                    {
                        InserterThread& inserter = static_cast<InserterThread&>( thr );
                        nInsertSuccess += inserter.m_nInsertSuccess;
                        nInsertFailed += inserter.m_nInsertFailed;
                    }
                    break;
                case delete_thread:
                    {
                        DeleterThread& deleter = static_cast<DeleterThread&>(thr);
                        nDeleteSuccess += deleter.m_nDeleteSuccess;
                        nDeleteFailed += deleter.m_nDeleteFailed;
                }
                    break;
                default:
                    assert( false ); // Forgot anything?..
                }
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "final_set_size", testSet.size());

            //testSet.clear();
            for (auto const& str: m_arrString )
                testSet.erase( str );
            EXPECT_TRUE( testSet.empty());
            EXPECT_EQ( testSet.size(), 0u );

            additional_check( testSet );
            print_stat( propout(), testSet );
            additional_cleanup( testSet );
        }

        template <class Set>
        void do_test_extract( Set& testSet )
        {
            typedef Inserter<Set> InserterThread;
            typedef Deleter<Set>  DeleterThread;
            typedef Extractor<typename Set::gc, Set> ExtractThread;

            size_t const nDelThreadCount = s_nDeleteThreadCount / 2;
            size_t const nExtractThreadCount = s_nDeleteThreadCount - nDelThreadCount;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new InserterThread( pool, testSet ), s_nInsertThreadCount );
            pool.add( new DeleterThread( pool, testSet ), nDelThreadCount );
            pool.add( new ExtractThread( pool, testSet ), nExtractThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsertThreadCount )
                << std::make_pair( "delete_thread_count", nDelThreadCount )
                << std::make_pair( "extract_thread_count", nExtractThreadCount )
                << std::make_pair( "thread_pass_count", s_nThreadPassCount )
                << std::make_pair( "set_size", s_nSetSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nExtractSuccess = 0;
            size_t nExtractFailed = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type()) {
                case insert_thread:
                    {
                        InserterThread& inserter = static_cast<InserterThread&>(thr);
                        nInsertSuccess += inserter.m_nInsertSuccess;
                        nInsertFailed += inserter.m_nInsertFailed;
                    }
                    break;
                case delete_thread:
                    {
                        DeleterThread& deleter = static_cast<DeleterThread&>(thr);
                        nDeleteSuccess += deleter.m_nDeleteSuccess;
                        nDeleteFailed += deleter.m_nDeleteFailed;
                    }
                    break;
                case extract_thread:
                    {
                        ExtractThread& extractor = static_cast<ExtractThread&>(thr);
                        nExtractSuccess += extractor.m_nDeleteSuccess;
                        nExtractFailed += extractor.m_nDeleteFailed;
                    }
                    break;
                default:
                    assert( false ); // Forgot anything?..
                }
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "extract_success", nExtractSuccess )
                << std::make_pair( "insert_failed",  nInsertFailed )
                << std::make_pair( "delete_failed",  nDeleteFailed )
                << std::make_pair( "extract_failed", nExtractFailed )
                << std::make_pair( "final_set_size", testSet.size());

            //testSet.clear();
            for ( auto const& str : m_arrString )
                testSet.erase( str );
            EXPECT_TRUE( testSet.empty());
            EXPECT_EQ( testSet.size(), 0u );

            additional_check( testSet );
            print_stat( propout(), testSet );
            additional_cleanup( testSet );
        }

        template <class Set>
        void run_test()
        {
            ASSERT_TRUE( m_arrString.size() > 0 );

            Set s( *this );
            do_test( s );
        }

        template <class Set>
        void run_test_extract()
        {
            ASSERT_TRUE( m_arrString.size() > 0 );

            Set s( *this );
            do_test_extract( s );
        }
    };

    class Set_InsDel_string_LF: public Set_InsDel_string
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Set>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Set_InsDel_string::run_test<Set>();
        }

        template <class Set>
        void run_test_extract()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Set_InsDel_string::run_test_extract<Set>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace set
