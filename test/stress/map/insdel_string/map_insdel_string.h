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

#define TEST_CASE(TAG, X)  void X();

    class Map_InsDel_string: public cds_test::stress_fixture
    {
    public:
        static size_t s_nMapSize;           // map size
        static size_t s_nInsertThreadCount; // count of insertion thread
        static size_t s_nDeleteThreadCount; // count of deletion thread
        static size_t s_nThreadPassCount;   // pass count for each thread
        static size_t s_nMaxLoadFactor;     // maximum load factor

        static size_t s_nCuckooInitialSize;         // initial size for CuckooMap
        static size_t s_nCuckooProbesetSize;        // CuckooMap probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold;   // CuckooMap probeset threshold (o - use default)

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t  s_nLoadFactor;  // current load factor

        static void SetUpTestCase();
        static void TearDownTestCase();

        static void setup_test_case();
        static std::vector<size_t> get_load_factors();

        typedef std::string key_type;
        typedef size_t      value_type;

        static std::vector<std::string> s_arrKeys;

    protected:
        enum {
            insert_thread,
            delete_thread
        };

        template <class Map>
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;

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

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < s_nThreadPassCount; ++nPass ) {
                        for ( auto it = s_arrKeys.cbegin(), itEnd = s_arrKeys.cend(); it != itEnd; ++it ) {
                            if ( rMap.insert( *it, 0 ))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < s_nThreadPassCount; ++nPass ) {
                        for ( auto it = s_arrKeys.crbegin(), itEnd = s_arrKeys.crend(); it != itEnd; ++it ) {
                            if ( rMap.insert( *it, 1 ))
                                ++m_nInsertSuccess;
                            else
                                ++m_nInsertFailed;
                        }
                    }
                }
            }
        };

        template <class Map>
        class Deleter: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Map&     m_Map;

        public:
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;

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

                if ( id() & 1 ) {
                    for ( size_t nPass = 0; nPass < s_nThreadPassCount; ++nPass ) {
                        for ( auto it = s_arrKeys.cbegin(), itEnd = s_arrKeys.cend(); it != itEnd; ++it ) {
                            if ( rMap.erase( *it ))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
                else {
                    for ( size_t nPass = 0; nPass < s_nThreadPassCount; ++nPass ) {
                        for ( auto it = s_arrKeys.crbegin(), itEnd = s_arrKeys.crend(); it != itEnd; ++it ) {
                            if ( rMap.erase( *it ))
                                ++m_nDeleteSuccess;
                            else
                                ++m_nDeleteFailed;
                        }
                    }
                }
            }
        };

    protected:
        template <typename Hash>
        static void fill_string_array();

        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map>       inserter;
            typedef Deleter<Map>        deleter;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new inserter( pool, testMap ), s_nInsertThreadCount );
            pool.add( new deleter( pool, testMap ), s_nDeleteThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsertThreadCount )
                << std::make_pair( "delete_thread_count", s_nDeleteThreadCount )
                << std::make_pair( "pass_count", s_nThreadPassCount )
                << std::make_pair( "map_size", s_nMapSize );

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
                    inserter& t = static_cast<inserter&>(thr);
                    nInsertSuccess += t.m_nInsertSuccess;
                    nInsertFailed += t.m_nInsertFailed;
                }
                break;
                case delete_thread:
                {
                    deleter& t = static_cast<deleter&>(thr);
                    nDeleteSuccess += t.m_nDeleteSuccess;
                    nDeleteFailed += t.m_nDeleteFailed;
                }
                break;
                default:
                    assert( false );
                }
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "finish_map_size", testMap.size());

            check_before_cleanup( testMap );

            //testMap.clear();
            for ( auto const& str: s_arrKeys )
                testMap.erase( str );
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
    };

    class Map_InsDel_string_stdhash: public Map_InsDel_string
    {
    public:
        static void SetUpTestCase();

        template <class Map>
        void run_test()
        {
            Map_InsDel_string::run_test<Map>();
        }
    };

#if CDS_BUILD_BITS == 64
    class Map_InsDel_string_city32: public Map_InsDel_string
    {
    public:
        static void SetUpTestCase();

        template <class Map>
        void run_test()
        {
            Map_InsDel_string::run_test<Map>();
        }
    };

    class Map_InsDel_string_city64: public Map_InsDel_string
    {
    public:
        static void SetUpTestCase();

        template <class Map>
        void run_test()
        {
            Map_InsDel_string::run_test<Map>();
        }
    };

    class Map_InsDel_string_city128: public Map_InsDel_string
    {
    public:
        static void SetUpTestCase();

        template <class Map>
        void run_test()
        {
            Map_InsDel_string::run_test<Map>();
        }
    };

#endif // #if CDS_BUILD_BITS == 64

    class Map_InsDel_string_LF: public Map_InsDel_string
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Map>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_InsDel_string::run_test<Map>();
        }
    };

} // namespace map
