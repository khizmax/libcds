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

    class Set_InsDelFind: public cds_test::stress_fixture
    {
    public:
        static size_t s_nSetSize;           // initial set size
        static size_t s_nThreadCount;       // thread count
        static size_t s_nMaxLoadFactor;     // maximum load factor
        static unsigned int s_nInsertPercentage;
        static unsigned int s_nDeletePercentage;
        static unsigned int s_nDuration;   // test duration, seconds

        static size_t  s_nCuckooInitialSize;        // initial size for CuckooSet
        static size_t  s_nCuckooProbesetSize;       // CuckooSet probeset size (only for list-based probeset)
        static size_t  s_nCuckooProbesetThreshold;  // CUckooSet probeset threshold (0 - use default)

        static size_t s_nFeldmanSet_HeadBits;
        static size_t s_nFeldmanSet_ArrayBits;

        static size_t s_nLoadFactor;

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
        actions m_arrShuffle[c_nShuffleSize];

    protected:
        typedef size_t  key_type;
        typedef size_t  value_type;

        template <class Set>
        class Worker: public cds_test::thread
        {
            typedef cds_test::thread base_class;
            Set&     m_Set;

        public:
            size_t  m_nInsertSuccess = 0;
            size_t  m_nInsertFailed = 0;
            size_t  m_nDeleteSuccess = 0;
            size_t  m_nDeleteFailed = 0;
            size_t  m_nFindSuccess = 0;
            size_t  m_nFindFailed = 0;

        public:
            Worker( cds_test::thread_pool& pool, Set& set )
                : base_class( pool )
                , m_Set( set )
            {}

            Worker( Worker& src )
                : base_class( src )
                , m_Set( src.m_Set )
            {}

            virtual thread * clone()
            {
                return new Worker( *this );
            }

            virtual void test()
            {
                Set& rSet = m_Set;
                Set_InsDelFind& fixture = pool().template fixture<Set_InsDelFind>();

                actions * pAct = fixture.m_arrShuffle;
                unsigned int i = 0;
                size_t const nNormalize = size_t(-1) / ( fixture.s_nSetSize * 2);

                size_t nRand = 0;
                while ( !time_elapsed() ) {
                    nRand = cds::bitop::RandXorShift(nRand);
                    size_t n = nRand / nNormalize;
                    switch ( pAct[i] ) {
                    case do_find:
                        if ( rSet.contains( n ))
                            ++m_nFindSuccess;
                        else
                            ++m_nFindFailed;
                        break;
                    case do_insert:
                        if ( rSet.insert( n ))
                            ++m_nInsertSuccess;
                        else
                            ++m_nInsertFailed;
                        break;
                    case do_delete:
                        if ( rSet.erase( n ))
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
        template <class Set>
        void do_test( Set& testSet )
        {
            typedef Worker<Set> work_thread;

            // fill map - only odd number
            {
                size_t * pInitArr = new size_t[ s_nSetSize ];
                size_t * pEnd = pInitArr + s_nSetSize;
                for ( size_t i = 0; i < s_nSetSize; ++i )
                    pInitArr[i] = i * 2 + 1;
                shuffle( pInitArr, pEnd );
                for ( size_t * p = pInitArr; p < pEnd; ++p )
                    testSet.insert( typename Set::value_type( *p, *p ) );
                delete [] pInitArr;
            }

            cds_test::thread_pool& pool = get_pool();
            pool.add( new work_thread( pool, testSet ), s_nThreadCount );

            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "set_size", s_nSetSize )
                << std::make_pair( "insert_percentage", s_nInsertPercentage )
                << std::make_pair( "delete_percentage", s_nDeletePercentage )
                << std::make_pair( "total_duration", s_nDuration );

            std::chrono::milliseconds duration = pool.run( std::chrono::seconds( s_nDuration ));

            propout() << std::make_pair( "duration", duration );

            size_t nInsertSuccess = 0;
            size_t nInsertFailed = 0;
            size_t nDeleteSuccess = 0;
            size_t nDeleteFailed = 0;
            size_t nFindSuccess = 0;
            size_t nFindFailed = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                work_thread& thr = static_cast<work_thread&>( pool.get( i ));
                nInsertSuccess += thr.m_nInsertSuccess;
                nInsertFailed  += thr.m_nInsertFailed;
                nDeleteSuccess += thr.m_nDeleteSuccess;
                nDeleteFailed  += thr.m_nDeleteFailed;
                nFindSuccess   += thr.m_nFindSuccess;
                nFindFailed    += thr.m_nFindFailed;
            }

            propout()
                << std::make_pair( "insert_success", nInsertSuccess )
                << std::make_pair( "insert_failed", nInsertFailed )
                << std::make_pair( "delete_success", nDeleteSuccess )
                << std::make_pair( "delete_failed", nDeleteFailed )
                << std::make_pair( "find_success", nFindSuccess )
                << std::make_pair( "find_failed", nFindFailed );

            {
                ASSERT_TRUE( std::chrono::duration_cast<std::chrono::seconds>(duration).count() > 0 );
                size_t nTotalOps = nInsertSuccess + nInsertFailed + nDeleteSuccess + nDeleteFailed + nFindSuccess + nFindFailed;
                propout() << std::make_pair( "avg_speed", nTotalOps / std::chrono::duration_cast<std::chrono::seconds>(duration).count() );
            }


            testSet.clear();
            EXPECT_TRUE( testSet.empty()) << "set size=" << testSet.size();

            additional_check( testSet );
            print_stat( propout(), testSet );
            additional_cleanup( testSet );
        }

        template <class Set>
        void run_test()
        {
            Set s( *this );
            do_test( s );
        }
    };

    class Set_InsDelFind_LF: public Set_InsDelFind
        , public ::testing::WithParamInterface<size_t>
    {
    public:
        template <class Set>
        void run_test()
        {
            s_nLoadFactor = GetParam();
            propout() << std::make_pair( "load_factor", s_nLoadFactor );
            Set_InsDelFind::run_test<Set>();
        }

        static std::vector<size_t> get_load_factors();
    };

} // namespace set
