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

#include "map_type.h"

namespace map {

    class Map_InsDel_func: public cds_test::stress_fixture
    {
    public:
        static size_t s_nMapSize;           // map size
        static size_t s_nInsertThreadCount; // count of insertion thread
        static size_t s_nDeleteThreadCount; // count of deletion thread
        static size_t s_nUpdateThreadCount; // count of updating thread
        static size_t s_nThreadPassCount;   // pass count for each thread
        static size_t s_nMaxLoadFactor;     // maximum load factor

        static size_t s_nCuckooInitialSize;         // initial size for CuckooMap
        static size_t s_nCuckooProbesetSize;        // CuckooMap probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold;   // CuckooMap probeset threshold (o - use default)

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t s_nLoadFactor;  // current load factor

        static void SetUpTestCase();
        static void TearDownTestCase();

        typedef size_t  key_type;
        struct value_type {
            size_t      nKey;
            size_t      nData;
            size_t      nUpdateCall;
            atomics::atomic<bool>   bInitialized;
            cds::OS::ThreadId       threadId;   // inserter thread id

            typedef cds::sync::spin_lock< cds::backoff::pause > lock_type;
            mutable lock_type   m_access;

            value_type()
                : nKey(0)
                , nData(0)
                , nUpdateCall(0)
                , bInitialized( false )
                , threadId( cds::OS::get_current_thread_id())
            {}

            value_type( value_type const& s )
                : nKey(s.nKey)
                , nData(s.nData)
                , nUpdateCall( s.nUpdateCall )
                , bInitialized( s.bInitialized.load(atomics::memory_order_relaxed))
                , threadId( cds::OS::get_current_thread_id())
                , m_access()
            {}

            // boost::container::flat_map requires operator =
            // cppcheck-suppress operatorEqVarError
            value_type& operator=( value_type const& v )
            {
                nKey = v.nKey;
                nData = v.nData;
                nUpdateCall = v.nUpdateCall;
                threadId = v.threadId;
                bInitialized.store(v.bInitialized.load(atomics::memory_order_relaxed), atomics::memory_order_relaxed);

                return *this;
            }
        };

        typedef std::vector<key_type>   key_array;
        static key_array                s_arrKeys;

    protected:
        enum {
            insert_thread,
            delete_thread,
            update_thread
        };

        template <class Map>
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

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
                    std::unique_lock< typename value_type::lock_type> ac( v.m_access );

                    v.nKey  = key;
                    v.nData = key * 8;

                    ++nTestFunctorRef;
                    v.bInitialized.store( true, atomics::memory_order_relaxed);
                }
            };

        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;

            size_t  m_nTestFunctorRef = 0;

        public:
            Inserter( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, insert_thread )
                , m_Map( map )
            {}

            Inserter( Inserter& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {}

            virtual thread * clone()
            {
                return new Inserter( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;

                // func is passed by reference
                insert_functor  func;
                size_t const nPassCount = s_nThreadPassCount;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = s_arrKeys.begin(), itEnd = s_arrKeys.end(); it != itEnd; ++it ) {
                            if ( rMap.insert_with( *it, std::ref(func)))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = s_arrKeys.rbegin(), itEnd = s_arrKeys.rend(); it != itEnd; ++it ) {
                            if ( rMap.insert_with( *it, std::ref(func)))
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
        class Updater: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

            struct update_functor {
                size_t  nCreated = 0;
                size_t  nModified = 0;

                update_functor() = default;

                template <typename Key, typename Val>
                void operator()( bool /*bNew*/, Key const& key, Val& v )
                {
                    std::unique_lock<typename value_type::lock_type> ac( v.m_access );
                    if ( !v.bInitialized.load( atomics::memory_order_acquire )) {
                        ++nCreated;
                        v.nKey = key;
                        v.nData = key * 8;
                        v.bInitialized.store( true, atomics::memory_order_relaxed);
                    }
                    else {
                        ++v.nUpdateCall;
                        ++nModified;
                    }
                }

                template <typename Pair>
                void operator()( bool bNew, Pair& val )
                {
                    operator()( bNew, val.first, val.second );
                }

                // For FeldmanHashMap, IterableList
                template <typename Val>
                void operator()( Val& cur, Val * old )
                {
                    if ( old ) {
                        // If a key exists, FeldmanHashMap creates a new node too
                        // We should manually copy important values from old to cur
                        std::unique_lock<typename value_type::lock_type> ac( cur.second.m_access );
                        cur.second.nKey = cur.first;
                        cur.second.nData = cur.first * 8;
                        cur.second.bInitialized.store( true, atomics::memory_order_release );
                    }
                    operator()( old == nullptr, cur.first, cur.second );
                }

            private:
                update_functor(const update_functor& ) = delete;
            };

        public:
            size_t  m_nUpdateFailed = 0;
            size_t  m_nUpdateCreated = 0;
            size_t  m_nUpdateExisted = 0;
            size_t  m_nFunctorCreated = 0;
            size_t  m_nFunctorModified = 0;

        public:
            Updater( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, update_thread )
                , m_Map( map )
            {}

            Updater( Updater& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {}

            virtual thread * clone()
            {
                return new Updater( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;

                update_functor func;
                size_t const nPassCount = s_nThreadPassCount;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = s_arrKeys.begin(), itEnd = s_arrKeys.end(); it != itEnd; ++it ) {
                            std::pair<bool, bool> ret = rMap.update( *it, std::ref( func ));
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
                        for ( key_array::const_reverse_iterator it = s_arrKeys.rbegin(), itEnd = s_arrKeys.rend(); it != itEnd; ++it ) {
                            std::pair<bool, bool> ret = rMap.update( *it, std::ref( func ));
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
        class Deleter: public cds_test::thread
        {
            Map&     m_Map;
            typedef cds_test::thread base_class;
            typedef typename Map::mapped_type value_type;

            struct value_container
            {
                size_t      nKeyExpected;

                size_t      nSuccessItem;
                size_t      nFailedItem;

                value_container()
                    : nKeyExpected()
                    , nSuccessItem(0)
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
                            std::unique_lock< typename value_type::lock_type> ac( v.m_access );

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
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;
            size_t  m_nValueSuccess = 0;
            size_t  m_nValueFailed = 0;

        public:
            Deleter( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, delete_thread )
                , m_Map( map )
            {}

            Deleter( Deleter& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {}

            virtual thread * clone()
            {
                return new Deleter( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;

                erase_functor   func;
                size_t const nPassCount = s_nThreadPassCount;

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_iterator it = s_arrKeys.cbegin(), itEnd = s_arrKeys.cend(); it != itEnd; ++it ) {
                            func.m_cnt.nKeyExpected = *it;
                            if ( rMap.erase( *it, std::ref(func)))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                        for ( key_array::const_reverse_iterator it = s_arrKeys.crbegin(), itEnd = s_arrKeys.crend(); it != itEnd; ++it ) {
                            func.m_cnt.nKeyExpected = *it;
                            if ( rMap.erase( *it, std::ref(func)))
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
            typedef Inserter<Map> inserter;
            typedef Deleter<Map>  deleter;
            typedef Updater<Map>  updater;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new inserter( pool, testMap ), s_nInsertThreadCount );
            pool.add( new deleter( pool, testMap ), s_nDeleteThreadCount );
            if ( s_nUpdateThreadCount )
                pool.add( new updater( pool, testMap ), s_nUpdateThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsertThreadCount )
                << std::make_pair( "delete_thread_count", s_nDeleteThreadCount )
                << std::make_pair( "update_thread_count", s_nUpdateThreadCount )
                << std::make_pair( "pass_count", s_nThreadPassCount )
                << std::make_pair( "map_size", s_nMapSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

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
            size_t nInsFuncCalled = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type()) {
                case insert_thread:
                    {
                        inserter& t = static_cast<inserter&>( thr );
                        nInsertSuccess += t.m_nInsertSuccess;
                        nInsertFailed += t.m_nInsertFailed;
                        nInsFuncCalled += t.m_nTestFunctorRef;
                    }
                    break;
                case delete_thread:
                    {
                        deleter& t = static_cast<deleter&>(thr);
                        nDeleteSuccess += t.m_nDeleteSuccess;
                        nDeleteFailed += t.m_nDeleteFailed;
                        nDelValueSuccess += t.m_nValueSuccess;
                        nDelValueFailed += t.m_nValueFailed;
                    }
                    break;
                case update_thread:
                    {
                        updater& t = static_cast<updater&>(thr);
                        nUpdateCreated += t.m_nUpdateCreated;
                        nUpdateModified += t.m_nUpdateExisted;
                        nUpdateFailed += t.m_nUpdateFailed;
                        nEnsFuncCreated += t.m_nFunctorCreated;
                        nEnsFuncModified += t.m_nFunctorModified;
                    }
                    break;
                default:
                    assert( false );
                }
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed",  nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed",  nDeleteFailed )
                << std::make_pair( "update_success", nUpdateCreated + nUpdateModified )
                << std::make_pair( "update_failed",  nUpdateFailed )
                << std::make_pair( "update_functor_create", nEnsFuncCreated )
                << std::make_pair( "update_functor_modify", nEnsFuncModified )
                << std::make_pair( "finish_map_size", testMap.size());

            EXPECT_EQ( nDelValueFailed, 0u );
            EXPECT_EQ( nDelValueSuccess, nDeleteSuccess );
            EXPECT_EQ( nUpdateFailed, 0u );
            EXPECT_EQ( nUpdateCreated + nUpdateModified, nEnsFuncCreated + nEnsFuncModified );

            // nInsFuncCalled is call count of insert functor
            EXPECT_EQ( nInsFuncCalled, nInsertSuccess );

            check_before_cleanup( testMap );

            for ( size_t nItem = 0; nItem < s_nMapSize; ++nItem )
                testMap.erase( nItem );

            EXPECT_TRUE( testMap.empty());
            EXPECT_EQ( testMap.size(), 0u );

            additional_check( testMap );
            print_stat( propout(), testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test()
        {
            Map testMap( *this );
            do_test( testMap );
        }

        template <class Map>
        void run_test2()
        {
            Map testMap( *this );
            do_test( testMap );

            if ( testMap.size() != 0 ) {
                for ( auto it = testMap.begin(); it != testMap.end(); ++it ) {
                    std::cout << "key=" << it->first << std::endl;
                }
            }
        }
    };

    class Map_InsDel_func_LF: public Map_InsDel_func
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Set>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_InsDel_func::run_test<Set>();
        }

        template <class Set>
        void run_test2()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_InsDel_func::run_test2<Set>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace map
