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

// find int test in map<int> in mutithreaded mode
namespace map {

    class Map_find_int: public cds_test::stress_fixture
    {
    public:
        static size_t s_nThreadCount;       // thread count
        static size_t s_nMapSize;           // map size (count of searching item)
        static size_t s_nPercentExists;     // percent of existing keys in searching sequence
        static size_t s_nPassCount;
        static size_t s_nMaxLoadFactor;     // maximum load factor

        static size_t s_nCuckooInitialSize;         // initial size for CuckooMap
        static size_t s_nCuckooProbesetSize;        // CuckooMap probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold;   // CUckooMap probeset threshold (o - use default)

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t s_nLoadFactor;  // current load factor

        typedef size_t   key_type;
        struct value_type {
            key_type    nKey;   // key
            bool        bExists;   // true - key in map, false - key not in map
        };

        typedef std::vector<value_type> value_vector;
        static value_vector s_Data;

        static void SetUpTestCase();
        static void TearDownTestCase();

    private:
        static size_t s_nRealMapSize;

        static void generateSequence();

        template <typename Iterator, typename Map>
        static bool check_result( Iterator const& it, Map const& map )
        {
            return it != map.end();
        }
        template <typename Map>
        static bool check_result( bool b, Map const& )
        {
            return b;
        }

        template <class Map>
        class Worker: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&  m_Map;

        public:
            struct Stat {
                size_t      nSuccess = 0;
                size_t      nFailed = 0;
            };

            Stat    m_KeyExists;
            Stat    m_KeyNotExists;

        public:
            Worker( cds_test::thread_pool& pool, Map& map )
                : base_class( pool )
                , m_Map( map )
                , m_KeyExists()
                , m_KeyNotExists()
            {}

            Worker( Worker& src )
                : base_class( src )
                , m_Map( src.m_Map )
                , m_KeyExists()
                , m_KeyNotExists()
            {}

            virtual thread * clone()
            {
                return new Worker( *this );
            }

            virtual void test()
            {
                size_t const nPassCount = s_nPassCount;

                Map& rMap = m_Map;
                for ( size_t nPass = 0; nPass < nPassCount; ++nPass ) {
                    if ( id() & 1 ) {
                        auto itEnd = s_Data.cend();
                        for ( auto it = s_Data.cbegin(); it != itEnd; ++it ) {
                            auto bFound = rMap.contains( it->nKey );
                            if ( it->bExists ) {
                                if ( check_result( bFound, rMap ))
                                    ++m_KeyExists.nSuccess;
                                else
                                    ++m_KeyExists.nFailed;
                            }
                            else {
                                if ( check_result( bFound, rMap ))
                                    ++m_KeyNotExists.nFailed;
                                else
                                    ++m_KeyNotExists.nSuccess;
                            }
                        }
                    }
                    else {
                        auto itEnd = s_Data.crend();
                        for ( auto it = s_Data.crbegin(); it != itEnd; ++it ) {
                            auto bFound = rMap.contains( it->nKey );
                            if ( it->bExists ) {
                                if ( check_result( bFound, rMap ))
                                    ++m_KeyExists.nSuccess;
                                else
                                    ++m_KeyExists.nFailed;
                            }
                            else {
                                if ( check_result( bFound, rMap ))
                                    ++m_KeyNotExists.nFailed;
                                else
                                    ++m_KeyNotExists.nSuccess;
                            }
                        }
                    }
                }
            }
        };

    protected:

        template <class Map>
        void test( Map& testMap )
        {
            typedef Worker<Map> worker;

            // Fill the map
            for ( auto const& it: s_Data ) {
                if ( it.bExists ) {
                    EXPECT_TRUE( check_result( testMap.insert( it.nKey, it ), testMap ));
                }
            }

            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "map_size", s_nMapSize )
                << std::make_pair( "percent_exist", s_nPercentExists )
                << std::make_pair( "pass_count", s_nPassCount );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new worker( pool, testMap ), s_nThreadCount );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nExistSuccess = 0;
            size_t nExistFailed = 0;
            size_t nMissingSuccess = 0;
            size_t nMissingFailed = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                worker& w = static_cast<worker&>( pool.get( i ));
                nExistSuccess += w.m_KeyExists.nSuccess;
                nExistFailed += w.m_KeyExists.nFailed;
                nMissingSuccess += w.m_KeyNotExists.nSuccess;
                nMissingFailed += w.m_KeyNotExists.nFailed;

                EXPECT_EQ( w.m_KeyExists.nFailed, 0u ) << "thread " << i;
                EXPECT_EQ( w.m_KeyExists.nSuccess, s_nRealMapSize * s_nPassCount ) << "thread " << i;
                EXPECT_EQ( w.m_KeyNotExists.nFailed, 0u ) << "thread " << i;
                EXPECT_EQ( w.m_KeyNotExists.nSuccess, (s_Data.size() - s_nRealMapSize) * s_nPassCount ) << "thread " << i;
            }

            propout()
                << std::make_pair( "exist_found", nExistSuccess )
                << std::make_pair( "exist_not_found",  nExistFailed )  // must = 0
                << std::make_pair( "missing_not_found", nMissingSuccess )
                << std::make_pair( "missing_found", nMissingFailed );  // must = 0

            check_before_cleanup( testMap );

            testMap.clear();
            additional_check( testMap );
            print_stat( propout(), testMap );
            additional_cleanup( testMap );
        }

        template <class Map>
        void run_test()
        {
            Map testMap( *this );
            test( testMap );
        }
    };

    class Map_find_int_LF: public Map_find_int
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Map>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_find_int::run_test<Map>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace map
