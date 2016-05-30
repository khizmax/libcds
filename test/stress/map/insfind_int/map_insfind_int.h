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

#include "map_type.h"

namespace map {

    class Map_InsFind_int: public cds_test::stress_fixture
    {
    public:
        static size_t s_nMapSize;          // initial map size
        static size_t s_nThreadCount;      // thread count
        static size_t s_nMaxLoadFactor;    // maximum load factor

        static size_t s_nCuckooInitialSize;         // initial size for CuckooMap
        static size_t s_nCuckooProbesetSize;        // CuckooMap probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold;   // CuckooMap probeset threshold (o - use default)

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t  s_nLoadFactor;  // current load factor

        static void SetUpTestCase();
        //static void TearDownTestCase();

    private:
        typedef size_t  key_type;
        typedef size_t  value_type;

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
        class Inserter: public cds_test::thread
        {
            typedef cds_test::thread base_class;

            Map&     m_Map;
            std::vector<size_t> m_arrVal;

            void make_array()
            {
                size_t const nThreadCount = s_nThreadCount;
                size_t const nSize = s_nMapSize / nThreadCount + 1;
                m_arrVal.resize( nSize );
                size_t nItem = id();
                for ( size_t i = 0; i < nSize; nItem += nThreadCount, ++i )
                    m_arrVal[i] = nItem;
                shuffle( m_arrVal.begin(), m_arrVal.end() );
            }
        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;
            size_t  m_nFindSuccess = 0;
            size_t  m_nFindFail = 0;

        public:
            Inserter( cds_test::thread_pool& pool, Map& map )
                : base_class( pool )
                , m_Map( map )
            {
                make_array();
            }

            Inserter( Inserter& src )
                : base_class( src )
                , m_Map( src.m_Map )
            {
                make_array();
            }

            virtual thread * clone()
            {
                return new Inserter( *this );
            }

            virtual void test()
            {
                Map& rMap = m_Map;

                size_t const nArrSize = m_arrVal.size();
                for ( size_t i = 0; i < nArrSize; ++i ) {
                    size_t const nItem = m_arrVal[i];
                    if ( check_result( rMap.insert( nItem, nItem * 8 ), rMap ))
                        ++m_nInsertSuccess;
                    else
                        ++m_nInsertFailed;

                    for ( size_t k = 0; k <= i; ++k ) {
                        if ( check_result( rMap.contains( m_arrVal[k] ), rMap ))
                            ++m_nFindSuccess;
                        else
                            ++m_nFindFail;
                    }
                }
            }
        };

    protected:

        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map> inserter;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new inserter( pool, testMap ), s_nThreadCount );

            propout()
                << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "insert_per_thread", s_nMapSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nFindSuccess = 0;
            size_t nFindFailed = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                inserter& thr = static_cast<inserter&>(pool.get( i ));

                EXPECT_EQ( thr.m_nInsertFailed, 0 ) << "thread " << thr.id();
                EXPECT_EQ( thr.m_nFindFail, 0 ) << "thread " << thr.id();

                nInsertSuccess += thr.m_nInsertSuccess;
                nInsertFailed += thr.m_nInsertFailed;
                nFindSuccess += thr.m_nFindSuccess;
                nFindFailed += thr.m_nFindFail;
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "find_success", nFindSuccess )
                << std::make_pair( "find_failed", nFindFailed )
                << std::make_pair( "finish_map_size", testMap.size() );

            EXPECT_EQ( nInsertFailed, 0 );
            EXPECT_EQ( nFindFailed, 0 );

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
            do_test( testMap );
        }
    };

    class Map_InsFind_int_LF: public Map_InsFind_int
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Map>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_InsFind_int::run_test<Map>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace map
