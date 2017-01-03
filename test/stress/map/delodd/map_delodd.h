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
#include <cds/os/topology.h>

namespace map {

    namespace {
        struct key_thread
        {
            uint32_t  nKey;
            uint16_t  nThread;

            key_thread( size_t key, size_t threadNo )
                : nKey( static_cast<uint32_t>(key))
                , nThread( static_cast<uint16_t>(threadNo))
            {}

            key_thread()
                : nKey()
                , nThread()
            {}
        };

        static_assert(sizeof( key_thread ) % 8 == 0, "Key size mismatch!!!");
    } // namespace

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
    struct less<key_thread>
    {
        bool operator()( key_thread const& k1, key_thread const& k2 ) const
        {
            if ( k1.nKey <= k2.nKey )
                return k1.nKey < k2.nKey || k1.nThread < k2.nThread;
            return false;
        }
    };

    template <>
    struct hash<key_thread>
    {
        typedef size_t             result_type;
        typedef key_thread    argument_type;

        size_t operator()( key_thread const& k ) const
        {
            return std::hash<size_t>()(k.nKey);
        }
        size_t operator()( size_t k ) const
        {
            return std::hash<size_t>()(k);
        }
    };

    class Map_DelOdd: public cds_test::stress_fixture
    {
    public:
        static size_t s_nInsThreadCount;      // insert thread count
        static size_t s_nDelThreadCount;      // delete thread count
        static size_t s_nExtractThreadCount;  // extract thread count
        static size_t s_nMapSize;             // max map size
        static size_t s_nMaxLoadFactor;       // maximum load factor
        static size_t s_nInsertPassCount;
        static size_t s_nFindThreadCount;     // find thread count

        static size_t s_nCuckooInitialSize;       // initial size for CuckooMap
        static size_t s_nCuckooProbesetSize;      // CuckooMap probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold; // CuckooMap probeset threshold (0 - use default)

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t  s_nLoadFactor;  // current load factor

        static std::vector<size_t> m_arrElements;

        static void SetUpTestCase();
        static void TearDownTestCase();

        template <typename Pred>
        static void prepare_array( std::vector<size_t>& arr, Pred pred )
        {
            arr.reserve( m_arrElements.size());
            for ( auto el : m_arrElements ) {
                if ( pred( el ))
                    arr.push_back( el );
            }
            arr.resize( arr.size());
            shuffle( arr.begin(), arr.end());
        }

    protected:
        typedef key_thread  key_type;
        typedef size_t      value_type;
        typedef std::pair<key_type const, value_type> pair_type;

        atomics::atomic<size_t> m_nInsThreadCount;

        enum {
            inserter_thread,
            deleter_thread,
            extractor_thread,
            find_thread,
        };

        // Inserts keys from [0..N)
        template <class Map>
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

            struct update_func
            {
                template <typename Q>
                void operator()( bool /*bNew*/, Q const& ) const
                {}

                template <typename Q, typename V>
                void operator()( bool /*bNew*/, Q const&, V& ) const
                {}

                // FeldmanHashMap
                template <typename Q>
                void operator()( Q&, Q*) const
                {}
            };

            void init_data()
            {
                prepare_array( m_arr, []( size_t ) -> bool { return true; } );
                for ( size_t i = 0; i < m_arr.size(); ++i ) {
                    if ( m_Map.insert( key_type( m_arr[i], id())))
                        ++m_nInsertInitSuccess;
                    else
                        ++m_nInsertInitFailed;
                }
            }

        public:
            size_t m_nInsertSuccess = 0;
            size_t m_nInsertFailed = 0;
            size_t m_nInsertInitSuccess = 0;
            size_t m_nInsertInitFailed = 0;

            std::vector<size_t> m_arr;

        public:
            Inserter( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, inserter_thread )
                , m_Map( map )
            {
                init_data();
            }

            Inserter( Inserter& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {
                init_data();
            }

            virtual thread * clone()
            {
                return new Inserter( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;
                Map_DelOdd& fixture = pool().template fixture<Map_DelOdd>();

                update_func f;

                for ( size_t nPass = 0; nPass < s_nInsertPassCount; ++nPass ) {
                    if ( nPass & 1 ) {
                        // insert pass
                        for ( auto el : m_arr ) {
                            if ( el & 1 ) {
                                if ( rMap.insert( key_type( el, id())))
                                    ++m_nInsertSuccess;
                                else
                                    ++m_nInsertFailed;
                            }
                        }
                    }
                    else {
                        // update pass
                        for ( auto el : m_arr ) {
                            if ( el & 1 ) {
                                bool success;
                                bool inserted;
                                std::tie( success, inserted ) = rMap.update( key_type( el, id()), f );
                                if ( success && inserted )
                                    ++m_nInsertSuccess;
                                else
                                    ++m_nInsertFailed;
                            }
                        }
                    }
                }

                fixture.m_nInsThreadCount.fetch_sub( 1, atomics::memory_order_release );
                m_arr.resize( 0 );
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

            typedef key_equal equal_to;
        };

        // Deletes odd keys from [0..N)
        template <class Map>
        class Deleter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

            void init_data()
            {
                prepare_array( m_arr, []( size_t el ) ->bool { return ( el & 1 ) != 0; } );
            }

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

            std::vector<size_t> m_arr;

        public:
            Deleter( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, deleter_thread )
                , m_Map( map )
            {
                init_data();
            }

            Deleter( Deleter& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {
                init_data();
            }

            virtual thread * clone()
            {
                return new Deleter( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;

                Map_DelOdd& fixture = pool().template fixture<Map_DelOdd>();
                size_t const nInsThreadCount = s_nInsThreadCount;

                do {
                    if ( id() & 1 ) {
                        for ( auto el: m_arr ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( rMap.erase( key_type( el, k )))
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                        }
                    }
                    else {
                        for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                            for ( auto el: m_arr ) {
                                if ( rMap.erase( key_type( el, k )))
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                            }
                        }
                    }
                } while ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) != 0 );

                m_arr.resize( 0 );
            }
        };

        // Deletes odd keys from [0..N)
        template <class GC, class Map >
        class Extractor: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

            void init_data()
            {
                prepare_array( m_arr, []( size_t el ) ->bool { return ( el & 1 ) != 0; } );
            }

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

            std::vector<size_t> m_arr;

        public:
            Extractor( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, extractor_thread )
                , m_Map( map )
            {
                init_data();
            }

            Extractor( Extractor& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {
                init_data();
            }

            virtual thread * clone()
            {
                return new Extractor( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;

                typename Map::guarded_ptr gp;
                Map_DelOdd& fixture = pool().template fixture<Map_DelOdd>();
                size_t const nInsThreadCount = s_nInsThreadCount;

                do {
                    if ( id() & 1 ) {
                        for ( auto el : m_arr ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                gp = rMap.extract( key_type( el, k ));
                                if ( gp )
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                                gp.release();
                            }
                        }
                    }
                    else {
                        for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                            for ( auto el: m_arr ) {
                                gp = rMap.extract( key_type( el, k ));
                                if ( gp )
                                    ++m_nDeleteSuccess;
                                else
                                    ++m_nDeleteFailed;
                                gp.release();
                            }
                        }
                    }
                } while ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) != 0 );

                m_arr.resize( 0 );
            }
        };

        template <class RCU, class Map >
        class Extractor< cds::urcu::gc<RCU>, Map > : public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

            void init_data()
            {
                prepare_array( m_arr, []( size_t el ) -> bool { return ( el & 1 ) != 0; } );
            }

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

            std::vector<size_t> m_arr;

        public:
            Extractor( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, extractor_thread )
                , m_Map( map )
            {
                init_data();
            }

            Extractor( Extractor& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {
                init_data();
            }

            virtual thread * clone()
            {
                return new Extractor( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;
                Map_DelOdd& fixture = pool().template fixture<Map_DelOdd>();

                typename Map::exempt_ptr xp;
                size_t const nInsThreadCount = s_nInsThreadCount;

                do {
                    if ( id() & 1 ) {
                        for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                            for ( auto el: m_arr ) {
                                if ( Map::c_bExtractLockExternal ) {
                                    typename Map::rcu_lock l;
                                    xp = rMap.extract( key_type( el, k ));
                                    if ( xp )
                                        ++m_nDeleteSuccess;
                                    else
                                        ++m_nDeleteFailed;
                                }
                                else {
                                    xp = rMap.extract( key_type( el, k ));
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
                        for ( auto el : m_arr ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( Map::c_bExtractLockExternal ) {
                                    typename Map::rcu_lock l;
                                    xp = rMap.extract( key_type( el, k ));
                                    if ( xp )
                                        ++m_nDeleteSuccess;
                                    else
                                        ++m_nDeleteFailed;
                                }
                                else {
                                    xp = rMap.extract( key_type( el, k ));
                                    if ( xp )
                                        ++m_nDeleteSuccess;
                                    else
                                        ++m_nDeleteFailed;
                                }
                                xp.release();
                            }
                        }
                    }
                } while ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) != 0 );

                m_arr.resize( 0 );
            }
        };

        // Finds keys
        template <class Map>
        class Observer: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&                m_Map;

        public:
            size_t m_nFindEvenSuccess = 0;
            size_t m_nFindEvenFailed  = 0;
            size_t m_nFindOddSuccess  = 0;
            size_t m_nFindOddFailed   = 0;

        public:
            Observer( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, find_thread )
                , m_Map( map )
            {}

            Observer( Observer& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {}

            virtual thread * clone()
            {
                return new Observer( *this );
            }

            virtual void test()
            {
                Map& map = m_Map;
                Map_DelOdd& fixture = pool().template fixture<Map_DelOdd>();
                std::vector<size_t> const& arr = m_arrElements;
                size_t const nInsThreadCount = s_nInsThreadCount;

                do {
                    for ( size_t key : arr ) {
                        if ( key & 1 ) {
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( map.contains( key_thread( key, k )))
                                    ++m_nFindOddSuccess;
                                else
                                    ++m_nFindOddFailed;
                            }
                        }
                        else {
                            // even keys MUST be in the map
                            for ( size_t k = 0; k < nInsThreadCount; ++k ) {
                                if ( map.contains( key_thread( key, k )))
                                    ++m_nFindEvenSuccess;
                                else
                                    ++m_nFindEvenFailed;
                            }
                        }
                    }
                } while ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) != 0 );
            }
        };

    protected:
        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map> insert_thread;
            typedef Deleter<Map>  delete_thread;
            typedef Observer<Map> observer_thread;

            m_nInsThreadCount.store( s_nInsThreadCount, atomics::memory_order_release );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new insert_thread( pool, testMap ), s_nInsThreadCount );
            pool.add( new delete_thread( pool, testMap ), s_nDelThreadCount ? s_nDelThreadCount : cds::OS::topology::processor_count());
            if ( s_nFindThreadCount )
                pool.add( new observer_thread( pool, testMap ), s_nFindThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsThreadCount )
                << std::make_pair( "delete_thread_count", s_nDelThreadCount )
                << std::make_pair( "find_thread_count", s_nFindThreadCount )
                << std::make_pair( "map_size", s_nMapSize )
                << std::make_pair( "pass_count", s_nInsertPassCount );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertInitFailed = 0;
            size_t nInsertInitSuccess = 0;
            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;

            size_t nFindEvenSuccess = 0;
            size_t nFindEvenFailed = 0;
            size_t nFindOddSuccess = 0;
            size_t nFindOddFailed = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type()) {
                case inserter_thread:
                    {
                        insert_thread& inserter = static_cast<insert_thread&>( thr );
                        nInsertSuccess += inserter.m_nInsertSuccess;
                        nInsertFailed += inserter.m_nInsertFailed;
                        nInsertInitSuccess += inserter.m_nInsertInitSuccess;
                        nInsertInitFailed += inserter.m_nInsertInitFailed;
                    }
                    break;
                case deleter_thread:
                    {
                        delete_thread& deleter = static_cast<delete_thread&>( thr );
                        nDeleteSuccess += deleter.m_nDeleteSuccess;
                        nDeleteFailed += deleter.m_nDeleteFailed;
                    }
                    break;
                case find_thread:
                    {
                        observer_thread& observer = static_cast<observer_thread&>( thr );
                        nFindEvenSuccess = observer.m_nFindEvenSuccess;
                        nFindEvenFailed = observer.m_nFindEvenFailed;
                        nFindOddSuccess = observer.m_nFindOddSuccess;
                        nFindOddFailed = observer.m_nFindOddFailed;
                    }
                    break;
                }
            }

            size_t const nInitialOddKeys = ( s_nMapSize * s_nInsThreadCount ) / 2;

            EXPECT_EQ( nInsertInitFailed, 0u );
            EXPECT_EQ( nInsertInitSuccess, s_nMapSize * s_nInsThreadCount );
            EXPECT_EQ( nFindEvenFailed, 0u );
            EXPECT_GE( nInsertSuccess + nInitialOddKeys, nDeleteSuccess );
            EXPECT_LE( nInsertSuccess, nDeleteSuccess );

            propout()
                << std::make_pair( "insert_init_success", nInsertInitSuccess )
                << std::make_pair( "insert_init_failed", nInsertInitFailed )
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "find_even_success", nFindEvenSuccess )
                << std::make_pair( "find_even_failed", nFindEvenFailed )
                << std::make_pair( "find_odd_success", nFindOddSuccess )
                << std::make_pair( "find_odd_failed", nFindOddFailed );

            analyze( testMap );
        }

        template <class Map>
        void do_test_extract( Map& testMap )
        {
            typedef Inserter<Map> insert_thread;
            typedef Deleter<Map> delete_thread;
            typedef Extractor< typename Map::gc, Map > extract_thread;
            typedef Observer<Map> observer_thread;

            m_nInsThreadCount.store( s_nInsThreadCount, atomics::memory_order_release );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new insert_thread( pool, testMap ), s_nInsThreadCount );
            if ( s_nDelThreadCount )
                pool.add( new delete_thread( pool, testMap ), s_nDelThreadCount );
            if ( s_nExtractThreadCount )
                pool.add( new extract_thread( pool, testMap ), s_nExtractThreadCount );
            if ( s_nFindThreadCount )
                pool.add( new observer_thread( pool, testMap ), s_nFindThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsThreadCount )
                << std::make_pair( "delete_thread_count", s_nDelThreadCount )
                << std::make_pair( "extract_thread_count", s_nExtractThreadCount )
                << std::make_pair( "find_thread_count", s_nFindThreadCount )
                << std::make_pair( "map_size", s_nMapSize )
                << std::make_pair( "pass_count", s_nInsertPassCount );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertInitFailed = 0;
            size_t nInsertInitSuccess = 0;
            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nExtractSuccess = 0;
            size_t nExtractFailed = 0;

            size_t nFindEvenSuccess = 0;
            size_t nFindEvenFailed = 0;
            size_t nFindOddSuccess = 0;
            size_t nFindOddFailed = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type()) {
                case inserter_thread:
                {
                    insert_thread& inserter = static_cast<insert_thread&>(thr);
                    nInsertSuccess += inserter.m_nInsertSuccess;
                    nInsertFailed += inserter.m_nInsertFailed;
                    nInsertInitSuccess += inserter.m_nInsertInitSuccess;
                    nInsertInitFailed += inserter.m_nInsertInitFailed;
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
                    nExtractSuccess += extractor.m_nDeleteSuccess;
                    nExtractFailed += extractor.m_nDeleteFailed;
                }
                break;
                case find_thread:
                {
                    observer_thread& observer = static_cast<observer_thread&>( thr );
                    nFindEvenSuccess = observer.m_nFindEvenSuccess;
                    nFindEvenFailed = observer.m_nFindEvenFailed;
                    nFindOddSuccess = observer.m_nFindOddSuccess;
                    nFindOddFailed = observer.m_nFindOddFailed;
                }
                break;
                default:
                    assert( false );
                }
            }

            size_t const nInitialOddKeys = ( s_nMapSize * s_nInsThreadCount ) / 2;

            EXPECT_EQ( nInsertInitFailed, 0u );
            EXPECT_EQ( nInsertInitSuccess, s_nMapSize * s_nInsThreadCount );
            EXPECT_EQ( nFindEvenFailed, 0u );
            EXPECT_GE( nInsertSuccess + nInitialOddKeys, nDeleteSuccess + nExtractSuccess );
            EXPECT_LE( nInsertSuccess, nDeleteSuccess + nExtractSuccess );

            propout()
                << std::make_pair( "insert_init_success", nInsertInitSuccess )
                << std::make_pair( "insert_init_failed", nInsertInitFailed )
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "extract_success", nExtractSuccess )
                << std::make_pair( "extract_failed", nExtractFailed )
                << std::make_pair( "find_even_success", nFindEvenSuccess )
                << std::make_pair( "find_even_failed", nFindEvenFailed )
                << std::make_pair( "find_odd_success", nFindOddSuccess )
                << std::make_pair( "find_odd_failed", nFindOddFailed );

            analyze( testMap );
        }

        template <class Map>
        void analyze( Map& testMap )
        {
            // All even keys must be in the map
            {
                for ( size_t n = 0; n < s_nMapSize; n +=2 ) {
                    for ( size_t i = 0; i < s_nInsThreadCount; ++i ) {
                        EXPECT_TRUE( testMap.contains( key_type( n, i ))) << "key=" << n << "/" << i;
                    }
                }
            }

            print_stat( propout(), testMap );

            check_before_cleanup( testMap );
            testMap.clear();
            EXPECT_TRUE( testMap.empty()) << "map.size=" << testMap.size();

            additional_check( testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test_extract()
        {
            static_assert( Map::c_bExtractSupported, "Map class must support extract() method" );

            size_t nMapSize = s_nMapSize;
            s_nMapSize *= s_nInsThreadCount;

            Map testMap( *this );

            s_nMapSize = nMapSize;
            do_test_extract( testMap );
        }

        template <class Map>
        void run_test()
        {
            size_t nMapSize = s_nMapSize;
            s_nMapSize *= s_nInsThreadCount;

            Map testMap( *this );

            s_nMapSize = nMapSize;
            do_test( testMap );
        }

        template <class Map>
        void run_feldman();
    };

    class Map_DelOdd_LF: public Map_DelOdd
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Map>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_DelOdd::run_test<Map>();
        }

        template <class Map>
        void run_test_extract()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_DelOdd::run_test_extract<Map>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace map
