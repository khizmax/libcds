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

    class Map_InsDel_item_int: public cds_test::stress_fixture
    {
    public:
        static size_t s_nMapSize;           // map size
        static size_t s_nInsertThreadCount; // count of insertion thread
        static size_t s_nDeleteThreadCount; // count of deletion thread
        static size_t s_nMaxLoadFactor;     // maximum load factor

        static size_t s_nAttemptCount;      // count of SUCCESS insert/delete for each thread
        static size_t s_nGoalItem;

        static size_t s_nCuckooInitialSize;         // initial size for CuckooMap
        static size_t s_nCuckooProbesetSize;        // CuckooMap probeset size (only for list-based probeset)
        static size_t s_nCuckooProbesetThreshold;   // CuckooMap probeset threshold (o - use default)

        static size_t s_nFeldmanMap_HeadBits;
        static size_t s_nFeldmanMap_ArrayBits;

        static size_t  s_nLoadFactor;  // current load factor

        static void SetUpTestCase();
        //static void TearDownTestCase();

        typedef size_t  key_type;
        typedef size_t  value_type;

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

            struct update_func
            {
                void operator()( bool bNew, std::pair<key_type const, value_type>& item ) const
                {
                    if ( bNew )
                        item.second = item.first;
                }

                // for boost::container::flat_map
                void operator()( bool bNew, std::pair<key_type, value_type>& item ) const
                {
                    if ( bNew )
                        item.second = item.first;
                }

                // for BronsonAVLTreeMap
                void operator()( bool bNew, key_type key, value_type& val ) const
                {
                    if ( bNew )
                        val = key;
                }

                // for FeldmanHashMap
                void operator()( std::pair<key_type const, value_type>& item, std::pair<key_type const, value_type>* pOld ) const
                {
                    if ( !pOld )
                        item.second = item.first;
                }
            };

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

                size_t const nGoalItem = s_nGoalItem;
                size_t const nAttemptCount = s_nAttemptCount;

                for ( size_t nAttempt = 0; nAttempt < nAttemptCount; ) {
                    if ( nAttempt % 2  == 0 ) {
                        if ( rMap.insert( nGoalItem, nGoalItem )) {
                            ++m_nInsertSuccess;
                            ++nAttempt;
                        }
                        else
                            ++m_nInsertFailed;
                    }
                    else {
                        std::pair<bool, bool> updateResult = rMap.update( nGoalItem, update_func(), true );
                        if ( updateResult.second ) {
                            ++m_nInsertSuccess;
                            ++nAttempt;
                        }
                        else
                            ++m_nInsertFailed;
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

                size_t const nGoalItem = s_nGoalItem;
                size_t const nAttemptCount = s_nAttemptCount;
                for ( size_t nAttempt = 0; nAttempt < nAttemptCount; ) {
                    if ( rMap.erase( nGoalItem )) {
                        ++m_nDeleteSuccess;
                        ++nAttempt;
                    }
                    else
                        ++m_nDeleteFailed;
                }
            }
        };

    protected:

        template <class Map>
        void do_test( Map& testMap )
        {
            typedef Inserter<Map>       inserter;
            typedef Deleter<Map>        deleter;

            // Fill the map
            {
                std::vector<key_type>   v;
                v.reserve( s_nMapSize );
                for ( size_t i = 0; i < s_nMapSize; ++i )
                    v.push_back( i );
                shuffle( v.begin(), v.end() );
                for ( auto i: v )
                    EXPECT_TRUE( testMap.insert( i, i ));
            }

            cds_test::thread_pool& pool = get_pool();
            pool.add( new inserter( pool, testMap ), s_nInsertThreadCount );
            pool.add( new deleter( pool, testMap ), s_nDeleteThreadCount );

            propout() << std::make_pair( "insert_thread_count", s_nInsertThreadCount )
                << std::make_pair( "delete_thread_count", s_nDeleteThreadCount )
                << std::make_pair( "map_size", s_nMapSize )
                << std::make_pair( "goal_item", s_nGoalItem )
                << std::make_pair( "attempt_count", s_nAttemptCount );


            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;

            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& thr = pool.get( i );
                switch ( thr.type() ) {
                case insert_thread:
                {
                    inserter& t = static_cast<inserter&>(thr);
                    EXPECT_EQ( t.m_nInsertSuccess, s_nAttemptCount ) << "thread=" << t.id();
                    nInsertSuccess += t.m_nInsertSuccess;
                    nInsertFailed += t.m_nInsertFailed;
                }
                break;
                case delete_thread:
                {
                    deleter& t = static_cast<deleter&>(thr);
                    EXPECT_EQ( t.m_nDeleteSuccess, s_nAttemptCount ) << "thread=" << t.id();
                    nDeleteSuccess += t.m_nDeleteSuccess;
                    nDeleteFailed += t.m_nDeleteFailed;
                }
                break;
                default:
                    assert( false );
                }
            }

            EXPECT_EQ( nInsertSuccess, nDeleteSuccess );
            EXPECT_TRUE( testMap.contains( s_nGoalItem ));
            EXPECT_CONTAINER_SIZE( testMap, s_nMapSize );

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed );

            // Check if the map contains all items
            for ( size_t i = 0; i < s_nMapSize; ++i )
                EXPECT_TRUE( testMap.contains( i ) ) << "key=" << i;

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
            do_test<Map>( testMap );
        }
    };

    class Map_InsDel_item_int_LF: public Map_InsDel_item_int
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Set>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Map_InsDel_item_int::run_test<Set>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace map
