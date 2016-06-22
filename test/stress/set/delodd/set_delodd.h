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

#include "set_type.h"

namespace set {

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
            : nKey()
            , nThread()
            , pad_( 0 )
        {}
    };

    typedef set_type_base<key_thread, size_t>::key_val     key_value_pair;

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

    template <>
    struct less<set::key_thread>
    {
        bool operator()( set::key_thread const& k1, set::key_thread const& k2 ) const
        {
            if ( k1.nKey <= k2.nKey )
                return k1.nKey < k2.nKey || k1.nThread < k2.nThread;
            return false;
        }
    };

    template <>
    struct hash<set::key_thread>
    {
        typedef size_t             result_type;
        typedef set::key_thread    argument_type;

        size_t operator()( set::key_thread const& k ) const
        {
            return std::hash<size_t>()(k.nKey);
        }

        size_t operator()( size_t k ) const
        {
            return std::hash<size_t>()(k);
        }
    };


    class Set_DelOdd: public cds_test::stress_fixture
    {
    public:
        static size_t s_nSetSize;              // max set size
        static size_t s_nInsThreadCount;       // insert thread count
        static size_t s_nDelThreadCount;       // delete thread count
        static size_t s_nExtractThreadCount;   // extract thread count
        static size_t s_nMaxLoadFactor;        // maximum load factor

        static size_t s_nCuckooInitialSize;    // initial size for CuckooSet
        static size_t s_nCuckooProbesetSize;   // CuckooSet probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold; // CUckooSet probeset threshold (0 - use default)

        static size_t s_nFeldmanSet_HeadBits;
        static size_t s_nFeldmanSet_ArrayBits;

        static size_t s_nLoadFactor;

        static std::vector<size_t> m_arrData;

        static void SetUpTestCase();
        static void TearDownTestCase();

    protected:
        typedef key_thread  key_type;
        typedef size_t      value_type;

        atomics::atomic<size_t> m_nInsThreadCount;

        enum {
            inserter_thread,
            deleter_thread,
            extractor_thread,
        };


        // Inserts keys from [0..N)
        template <class Set>
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Set&     m_Set;

            struct update_functor
            {
                template <typename Q>
                void operator()( bool /*bNew*/, key_value_pair const&, Q const& ) const
                {}

                void operator()(key_value_pair& /*cur*/, key_value_pair * /*prev*/) const
                {}
            };
        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;

        public:
            Inserter( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, inserter_thread )
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
                Set_DelOdd& fixture = pool().template fixture<Set_DelOdd>();

                std::vector<size_t>& arrData = fixture.m_arrData;
                for ( size_t i = 0; i < arrData.size(); ++i ) {
                    if ( rSet.insert( key_type( arrData[i], id() )))
                        ++m_nInsertSuccess;
                    else
                        ++m_nInsertFailed;
                }

                update_functor f;
                for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                    if ( arrData[i] & 1 )
                        rSet.update( key_type( arrData[i], id() ), f, true );
                }

                fixture.m_nInsThreadCount.fetch_sub( 1, atomics::memory_order_release );
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
        class Deleter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Set&     m_Set;

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

        public:
            Deleter( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, deleter_thread )
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

                size_t const nInsThreadCount = s_nInsThreadCount;
                Set_DelOdd& fixture = pool().template fixture<Set_DelOdd>();
                std::vector<size_t>& arrData = fixture.m_arrData;

                if ( id() & 1 ) {
                    for (size_t i = 0; i < arrData.size(); ++i) {
                        if ( arrData[i] & 1 ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( eraser<Set, Set::c_bEraseExactKey>::erase( rSet, arrData[i], k ))
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                        }
                        if ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
                else {
                    for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                        if ( arrData[i] & 1 ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if (eraser<Set, Set::c_bEraseExactKey>::erase(rSet, arrData[i], k))
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                        }
                        if ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
            }
        };

        // Extracts odd keys from [0..N)
        template <typename GC, class Set>
        class Extractor: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Set&     m_Set;

        public:
            size_t  m_nExtractSuccess = 0;
            size_t  m_nExtractFailed = 0;

        public:
            Extractor( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, extractor_thread )
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

                typename Set::guarded_ptr gp;

                Set_DelOdd& fixture = pool().template fixture<Set_DelOdd>();
                std::vector<size_t>& arrData = fixture.m_arrData;
                size_t const nInsThreadCount = s_nInsThreadCount;

                if ( id() & 1 ) {
                    for ( size_t i = 0; i < arrData.size(); ++i ) {
                        if ( arrData[i] & 1 ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                gp = extractor<Set, Set::c_bEraseExactKey>::extract( rSet, arrData[i], k );
                                if ( gp )
                                    ++m_nExtractSuccess;
                                else
                                    ++m_nExtractFailed;
                                gp.release();
                            }
                        }
                        if ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
                else {
                    for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                        if ( arrData[i] & 1 ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                gp = extractor<Set, Set::c_bEraseExactKey>::extract( rSet, arrData[i], k);
                                if ( gp )
                                    ++m_nExtractSuccess;
                                else
                                    ++m_nExtractFailed;
                                gp.release();
                            }
                        }
                        if ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
            }
        };

        template <typename RCU, class Set>
        class Extractor< cds::urcu::gc<RCU>, Set >: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Set&     m_Set;

        public:
            size_t  m_nExtractSuccess = 0;
            size_t  m_nExtractFailed = 0;

        public:
            Extractor( cds_test::thread_pool& pool, Set& set )
                : base_class( pool, extractor_thread )
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

                typename Set::exempt_ptr xp;

                Set_DelOdd& fixture = pool().template fixture<Set_DelOdd>();
                std::vector<size_t>& arrData = fixture.m_arrData;
                size_t const nInsThreadCount = fixture.s_nInsThreadCount;

                if ( id() & 1 ) {
                    for ( size_t i = 0; i < arrData.size(); ++i ) {
                        if ( arrData[i] & 1 ) {
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
                        if ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
                else {
                    for ( size_t i = arrData.size() - 1; i > 0; --i ) {
                        if ( arrData[i] & 1 ) {
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
                        if ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) == 0 )
                            break;
                    }
                }
            }
        };

    protected:
        template <class Set>
        void do_test_with( Set& testSet )
        {
            typedef Inserter<Set> insert_thread;
            typedef Deleter<Set> delete_thread;

            m_nInsThreadCount.store( s_nInsThreadCount, atomics::memory_order_release );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new insert_thread( pool, testSet ), s_nInsThreadCount );
            pool.add( new delete_thread( pool, testSet ), s_nDelThreadCount ? s_nDelThreadCount : cds::OS::topology::processor_count());

            propout() << std::make_pair( "insert_thread_count", s_nInsThreadCount )
                << std::make_pair( "delete_thread_count", s_nDelThreadCount )
                << std::make_pair( "set_size", s_nSetSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                if ( thr.type() == inserter_thread ) {
                    insert_thread& inserter = static_cast<insert_thread&>(thr);
                    nInsertSuccess += inserter.m_nInsertSuccess;
                    nInsertFailed += inserter.m_nInsertFailed;
                }
                else {
                    assert( thr.type() == deleter_thread );
                    delete_thread& deleter = static_cast<delete_thread&>(thr);
                    nDeleteSuccess += deleter.m_nDeleteSuccess;
                    nDeleteFailed += deleter.m_nDeleteFailed;
                }
            }

            EXPECT_EQ( nInsertSuccess, s_nSetSize * s_nInsThreadCount );
            EXPECT_EQ( nInsertFailed, 0 );

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed );
        }

        template <class Set>
        void do_test_extract_with( Set& testSet )
        {
            typedef Inserter<Set> insert_thread;
            typedef Deleter<Set> delete_thread;
            typedef Extractor< typename Set::gc, Set > extract_thread;

            m_nInsThreadCount.store( s_nInsThreadCount, atomics::memory_order_release );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new insert_thread( pool, testSet ), s_nInsThreadCount );
            if ( s_nDelThreadCount )
                pool.add( new delete_thread( pool, testSet ), s_nDelThreadCount );
            if ( s_nExtractThreadCount )
                pool.add( new extract_thread( pool, testSet ), s_nExtractThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsThreadCount )
                << std::make_pair( "delete_thread_count", s_nDelThreadCount )
                << std::make_pair( "extract_thread_count", s_nExtractThreadCount )
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
                switch ( thr.type() ) {
                case inserter_thread:
                    {
                        insert_thread& inserter = static_cast<insert_thread&>( thr );
                        nInsertSuccess += inserter.m_nInsertSuccess;
                        nInsertFailed += inserter.m_nInsertFailed;
                    }
                    break;
                case deleter_thread:
                    {
                        delete_thread& deleter = static_cast<delete_thread&>(thr);
                        nDeleteSuccess += deleter.m_nDeleteSuccess;
                        nDeleteFailed += deleter.m_nDeleteFailed;
                    }
                    break;
                case extractor_thread:
                    {
                        extract_thread& extractor = static_cast<extract_thread&>(thr);
                        nExtractSuccess += extractor.m_nExtractSuccess;
                        nExtractFailed += extractor.m_nExtractFailed;
                    }
                    break;
                default:
                    assert( false );
                }
            }

            EXPECT_EQ( nInsertSuccess, s_nSetSize * s_nInsThreadCount );
            EXPECT_EQ( nInsertFailed, 0 );

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "extract_success", nExtractSuccess )
                << std::make_pair( "extract_failed", nExtractFailed );
        }

        template <typename Set>
        void analyze( Set& testSet )
        {
            // All even keys must be in the set
            {
                for ( size_t n = 0; n < s_nSetSize; n +=2 ) {
                    for ( size_t i = 0; i < s_nInsThreadCount; ++i ) {
                        EXPECT_TRUE( testSet.contains( key_type( n, i ))) << "key=" << n << "/" << i;
                    }
                }
            }

            check_before_clear( testSet );

            testSet.clear();
            EXPECT_TRUE( testSet.empty() ) << "set.size=" << testSet.size();

            additional_check( testSet );
            print_stat( propout(), testSet );
            additional_cleanup( testSet );
        }

        template <class Set>
        void run_test()
        {
            static_assert( !Set::c_bExtractSupported, "Set class must not support extract() method" );

            Set  testSet( *this );
            do_test_with( testSet );
            analyze( testSet );
        }

        template <class Set>
        void run_test_extract()
        {
            static_assert( Set::c_bExtractSupported, "Set class must support extract() method" );

            Set  testSet( *this );
            do_test_extract_with( testSet );
            analyze( testSet );
        }
    };

    class Set_DelOdd_LF: public Set_DelOdd
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Set>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Set_DelOdd::run_test<Set>();
        }

        template <class Set>
        void run_test_extract()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Set_DelOdd::run_test_extract<Set>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace set
