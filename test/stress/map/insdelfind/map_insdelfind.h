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


    class Map_InsDelFind: public cds_test::stress_fixture
    {
    public:
        static size_t s_nMapSize;           // initial map size
        static size_t s_nThreadCount;       // thread count
        static size_t s_nMaxLoadFactor;     // maximum load factor
        static unsigned int s_nInsertPercentage;
        static unsigned int s_nDeletePercentage;
        static unsigned int s_nDuration;    // test duration, seconds

        static size_t s_nCuckooInitialSize;         // initial size for CuckooMap
        static size_t s_nCuckooProbesetSize;        // CuckooMap probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold;   // CuckooMap probeset threshold (o - use default)

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t  s_nLoadFactor;  // current load factor

        static void SetUpTestCase();
        //static void TearDownTestCase();

    public:
        enum actions
        {
            do_find,
            do_insert,
            do_delete
        };
        static const unsigned int c_nShuffleSize = 100;
        static actions s_arrShuffle[c_nShuffleSize];

    protected:
        typedef size_t  key_type;
        typedef size_t  value_type;

        template <class Map>
        class Worker: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;
            size_t  m_nFindSuccess = 0;
            size_t  m_nFindFailed = 0;

        public:
            Worker( cds_test::thread_pool& pool, Map& map )
                : base_class( pool )
                , m_Map( map )
            {}

            Worker( Worker& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {}

            virtual thread * clone()
            {
                return new Worker( *this );
            }

            typedef std::pair< key_type const, value_type > map_value_type;

            struct update_functor {
                template <typename Q>
                void operator()( bool /*bNew*/, map_value_type& /*cur*/, Q const& /*val*/ ) const
                {}

                // FeldmanHashMap
                void operator()( map_value_type& /*cur*/, map_value_type* /*old*/) const
                {}

                // MichaelMap
                void operator()( bool /*bNew*/, map_value_type& /*cur*/ ) const
                {}

                // BronsonAVLTreeMap
                void operator()( bool /*bNew*/, key_type /*key*/, value_type& /*val*/ ) const
                {}
            };

            virtual void test()
            {
                Map& rMap = m_Map;

                unsigned int i = 0;
                size_t const nNormalize = size_t(-1) / ( s_nMapSize * 2 );

                size_t nRand = 0;
                while ( !time_elapsed()) {
                    nRand = cds::bitop::RandXorShift( nRand );
                    size_t n = nRand / nNormalize;
                    switch ( s_arrShuffle[i] ) {
                    case do_find:
                        if ( rMap.contains( n ))
                            ++m_nFindSuccess;
                        else
                            ++m_nFindFailed;
                        break;
                    case do_insert:
                        if ( n % 2 ) {
                            if ( rMap.insert( n, n ))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                        else {
                            if ( rMap.update( n, update_functor(), true ).first )
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                        break;
                    case do_delete:
                        if ( rMap.erase( n ))
                            ++m_nDeleteSuccess;
                        else
                            ++m_nDeleteFailed;
                        break;
                    }

                    if ( ++i >= c_nShuffleSize )
                        i = 0;
                }
            }
        };

    protected:
        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Worker<Map> worker;

            // fill map - only odd number
            {
                std::vector<size_t> arr;
                arr.reserve( s_nMapSize );
                for ( size_t i = 0; i < s_nMapSize; ++i )
                    arr.push_back( i * 2 + 1);
                shuffle( arr.begin(), arr.end());
                for ( size_t i = 0; i < s_nMapSize; ++i )
                    testMap.insert( arr[i], arr[i] );
            }

            cds_test::thread_pool& pool = get_pool();
            pool.add( new worker( pool, testMap ), s_nThreadCount );

            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "insert_percentage", s_nInsertPercentage )
                << std::make_pair( "delete_percentage", s_nDeletePercentage )
                << std::make_pair( "map_size", s_nMapSize );

            std::chrono::milliseconds duration = pool.run( std::chrono::seconds( s_nDuration ));

            propout() << std::make_pair( "duration", duration );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nFindSuccess = 0;
            size_t nFindFailed = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                worker& thr = static_cast<worker&>( pool.get( i ));

                nInsertSuccess += thr.m_nInsertSuccess;
                nInsertFailed += thr.m_nInsertFailed;
                nDeleteSuccess += thr.m_nDeleteSuccess;
                nDeleteFailed += thr.m_nDeleteFailed;
                nFindSuccess += thr.m_nFindSuccess;
                nFindFailed += thr.m_nFindFailed;
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "find_success", nFindSuccess )
                << std::make_pair( "find_failed", nFindFailed )
                << std::make_pair( "finish_map_size", testMap.size());

            {
                ASSERT_TRUE( std::chrono::duration_cast<std::chrono::seconds>(duration).count() > 0 );
                size_t nTotalOps = nInsertSuccess + nInsertFailed + nDeleteSuccess + nDeleteFailed + nFindSuccess + nFindFailed;
                propout() << std::make_pair( "avg_speed", nTotalOps / std::chrono::duration_cast<std::chrono::seconds>( duration ).count());
            }

            check_before_cleanup( testMap );

            testMap.clear();
            EXPECT_TRUE( testMap.empty());

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
    };

    class Map_InsDelFind_LF: public Map_InsDelFind
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Map>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_InsDelFind::run_test<Map>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace map
