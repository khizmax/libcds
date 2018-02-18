// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_type.h"
#include <cds/os/topology.h>

namespace map {

    class Map_MinMax: public cds_test::stress_fixture
    {
    public:
        static size_t s_nInsThreadCount;      // insert thread count
        static size_t s_nExtractThreadCount;  // extract thread count
        static size_t s_nMapSize;             // max map size
        static size_t s_nPassCount;

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t  s_nLoadFactor;  // current load factor

        static void SetUpTestCase();
        //static void TearDownTestCase();

    protected:
        typedef int     key_type;
        typedef int     value_type;
        typedef std::pair<key_type const, value_type> pair_type;

        atomics::atomic<size_t> m_nInsThreadCount;
        key_type    m_KeyMin;
        key_type    m_KeyMax;

        enum {
            inserter_thread,
            extractor_thread,
        };

        template <class Map>
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;
            std::vector<key_type> m_arr;

            void init_data()
            {
                Map_MinMax& fixture = pool().template fixture<Map_MinMax>();
                key_type keyMin = fixture.m_KeyMin;
                key_type keyMax = fixture.m_KeyMax;

                for ( key_type i = keyMin + 10; i >= keyMin; --i )
                    m_arr.push_back( i );
                for ( key_type i = keyMax - 10; i <= keyMax; ++i )
                    m_arr.push_back( i );
                shuffle( m_arr.begin(), m_arr.end());
            }

        public:
            size_t m_nInsertMinSuccess = 0;
            size_t m_nInsertMinFailed = 0;
            size_t m_nInsertMaxSuccess = 0;
            size_t m_nInsertMaxFailed = 0;

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
                Map_MinMax& fixture = pool().template fixture<Map_MinMax>();

                key_type keyMin = fixture.m_KeyMin;
                key_type keyMax = fixture.m_KeyMax;

                for ( size_t nPass = 0; nPass < s_nPassCount; ++nPass ) {
                    for ( key_type key : m_arr ) {
                        if ( m_Map.insert( key, key )) {
                            if ( key == keyMin )
                                ++m_nInsertMinSuccess;
                            else if ( key == keyMax )
                                ++m_nInsertMaxSuccess;
                        }
                        else {
                            if ( key == keyMin )
                                ++m_nInsertMinFailed;
                            else if ( key == keyMax )
                                ++m_nInsertMaxFailed;
                        }
                    }
                }

                fixture.m_nInsThreadCount.fetch_sub( 1, atomics::memory_order_release );
            }
        };

        // Deletes odd keys from [0..N)
        template <class GC, class Map >
        class Extractor: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

        public:
            size_t  m_nDeleteMin = 0;
            size_t  m_nDeleteMinFailed = 0;
            size_t  m_nDeleteMax = 0;
            size_t  m_nDeleteMaxFailed = 0;

        public:
            Extractor( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, extractor_thread )
                , m_Map( map )
            {}

            Extractor( Extractor& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {}

            virtual thread * clone()
            {
                return new Extractor( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;

                typename Map::guarded_ptr gp;
                Map_MinMax& fixture = pool().template fixture<Map_MinMax>();

                key_type keyMin = fixture.m_KeyMin;
                key_type keyMax = fixture.m_KeyMax;

                do {
                    gp = rMap.extract_min();
                    if ( gp ) {
                        if ( gp->first == keyMin )
                            ++m_nDeleteMin;
                    }
                    else
                        ++m_nDeleteMinFailed;

                    gp = rMap.extract_max();
                    if ( gp ) {
                        if ( gp->first == keyMax )
                            ++m_nDeleteMax;
                    }
                    else
                        ++m_nDeleteMaxFailed;

                } while ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) != 0 );

                gp = rMap.extract_min();
                if ( gp ) {
                    if ( gp->first == keyMin )
                        ++m_nDeleteMin;
                }
                else
                    ++m_nDeleteMinFailed;

                gp = rMap.extract_max();
                if ( gp ) {
                    if ( gp->first == keyMax )
                        ++m_nDeleteMax;
                }
                else
                    ++m_nDeleteMaxFailed;
            }
        };

        template <class RCU, class Map >
        class Extractor< cds::urcu::gc<RCU>, Map > : public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

        public:
            size_t  m_nDeleteMin = 0;
            size_t  m_nDeleteMinFailed = 0;
            size_t  m_nDeleteMax = 0;
            size_t  m_nDeleteMaxFailed = 0;

        public:
            Extractor( cds_test::thread_pool& pool, Map& map )
                : base_class( pool, extractor_thread )
                , m_Map( map )
            {}

            Extractor( Extractor& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {}

            virtual thread * clone()
            {
                return new Extractor( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;
                Map_MinMax& fixture = pool().template fixture<Map_MinMax>();

                key_type keyMin = fixture.m_KeyMin;
                key_type keyMax = fixture.m_KeyMax;

                static_assert( !Map::c_bExtractLockExternal, "No external RCU locking required" );

                do {
                    auto res = rMap.extract_min_key();
                    if ( res.second ) {
                        if ( res.first == keyMin )
                            ++m_nDeleteMin;
                    }
                    else
                        ++m_nDeleteMinFailed;

                    res = rMap.extract_max_key();
                    if ( res.second ) {
                        if ( res.first == keyMax )
                            ++m_nDeleteMax;
                    }
                    else
                        ++m_nDeleteMaxFailed;
                } while ( fixture.m_nInsThreadCount.load( atomics::memory_order_acquire ) != 0 );

                auto res = rMap.extract_min_key();
                if ( res.second ) {
                    if ( res.first == keyMin )
                        ++m_nDeleteMin;
                }
                else
                    ++m_nDeleteMinFailed;

                res = rMap.extract_max_key();
                if ( res.second ) {
                    if ( res.first == keyMax )
                        ++m_nDeleteMax;
                }
                else
                    ++m_nDeleteMaxFailed;
            }
        };

    protected:
        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map> insert_thread;
            typedef Extractor< typename Map::gc, Map > extract_thread;

            m_nInsThreadCount.store( s_nInsThreadCount, atomics::memory_order_release );

            {
                std::vector<key_type> arr;
                arr.resize( s_nMapSize );
                for ( int i = 0; i < static_cast<int>( s_nMapSize ); ++i )
                    arr[i] = i;;
                shuffle( arr.begin(), arr.end());

                for ( key_type key : arr )
                    testMap.insert( key, key );
            }

            m_KeyMin = 0;
            m_KeyMax = static_cast<int>( s_nMapSize - 1 );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new insert_thread( pool, testMap ), s_nInsThreadCount );
            pool.add( new extract_thread( pool, testMap ), s_nExtractThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsThreadCount )
                << std::make_pair( "extract_thread_count", s_nExtractThreadCount )
                << std::make_pair( "map_size", s_nMapSize )
                << std::make_pair( "pass_count", s_nPassCount );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertMinSuccess = 0;
            size_t nInsertMinFailed = 0;
            size_t nInsertMaxSuccess = 0;
            size_t nInsertMaxFailed = 0;
            size_t nDeleteMin = 0;
            size_t nDeleteMinFailed = 0;
            size_t nDeleteMax = 0;
            size_t nDeleteMaxFailed = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type()) {
                case inserter_thread:
                {
                    insert_thread& inserter = static_cast<insert_thread&>(thr);
                    nInsertMinSuccess += inserter.m_nInsertMinSuccess;
                    nInsertMinFailed += inserter.m_nInsertMinFailed;
                    nInsertMaxSuccess += inserter.m_nInsertMaxSuccess;
                    nInsertMaxFailed += inserter.m_nInsertMaxFailed;
                }
                break;
                case extractor_thread:
                {
                    extract_thread& extractor = static_cast<extract_thread&>(thr);
                    nDeleteMin += extractor.m_nDeleteMin;
                    nDeleteMinFailed += extractor.m_nDeleteMinFailed;
                    nDeleteMax += extractor.m_nDeleteMax;
                    nDeleteMaxFailed += extractor.m_nDeleteMaxFailed;
                }
                break;
                default:
                    assert( false );
                }
            }

            EXPECT_EQ( nDeleteMinFailed, 0u );
            EXPECT_EQ( nDeleteMaxFailed, 0u );

            EXPECT_EQ( nDeleteMin, nInsertMinSuccess + 1 );
            EXPECT_EQ( nDeleteMax, nInsertMaxSuccess + 1 );

            propout()
                << std::make_pair( "insert_min", nInsertMinSuccess + 1 )
                << std::make_pair( "insert_min_double", nInsertMinFailed )
                << std::make_pair( "insert_max", nInsertMaxSuccess + 1 )
                << std::make_pair( "insert_max_double", nInsertMaxFailed )
                << std::make_pair( "extract_min", nDeleteMin )
                << std::make_pair( "extract_min_failed", nDeleteMinFailed )
                << std::make_pair( "extract_max", nDeleteMax )
                << std::make_pair( "extract_max_failed", nDeleteMaxFailed );

            analyze( testMap );
        }

        template <class Map>
        void analyze( Map& testMap )
        {
            print_stat( propout(), testMap );

            check_before_cleanup( testMap );
            testMap.clear();
            EXPECT_TRUE( testMap.empty()) << "map.size=" << testMap.size();

            additional_check( testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test()
        {
            static_assert( Map::c_bExtractSupported, "Map class must support extract() method" );
            Map testMap( *this );
            do_test( testMap );
        }
    };

} // namespace map
