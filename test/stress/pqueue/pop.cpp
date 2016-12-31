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

#include "pqueue_type.h"
#include "item.h"

namespace {
    static size_t s_nThreadCount = 8;
    static size_t s_nQueueSize = 2000000;

    class pqueue_pop: public cds_test::stress_fixture
    {
        typedef cds_test::stress_fixture base_class;

    protected:
        template <class PQueue>
        class Consumer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Consumer( cds_test::thread_pool& pool, PQueue& queue )
                : base_class( pool )
                , m_Queue( queue )
            {}

            Consumer( Consumer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
            {}

            virtual thread * clone()
            {
                return new Consumer( *this );
            }

            virtual void test()
            {
                typedef typename PQueue::value_type value_type;
                size_t nPrevKey;
                value_type val;
                if ( m_Queue.pop( val )) {
                    ++m_nPopSuccess;
                    nPrevKey = val.key;

                    bool prevPopFailed = false;
                    while ( m_Queue.pop( val )) {
                        ++m_nPopSuccess;
                        if ( val.key > nPrevKey ) {
                            ++m_nPopError;
                            m_arrFailedPops.emplace_back( failed_pops{ nPrevKey, val.key, static_cast<size_t>(-1) } );
                            prevPopFailed = true;
                        }
                        else if ( val.key == nPrevKey ) {
                            ++m_nPopErrorEq;
                            m_arrFailedPops.emplace_back( failed_pops{ nPrevKey, val.key, static_cast<size_t>(-1) } );
                        }
                        else {
                            if ( prevPopFailed )
                                m_arrFailedPops.back().next_key = val.key;
                            prevPopFailed = false;
                        }
                        if ( nPrevKey > val.key )
                            nPrevKey = val.key;
                    }

                }
                else
                    ++m_nPopFailed;
            }

        public:
            PQueue&             m_Queue;
            size_t              m_nPopError = 0;
            size_t              m_nPopErrorEq = 0;
            size_t              m_nPopSuccess = 0;
            size_t              m_nPopFailed = 0;

            struct failed_pops {
                size_t prev_key;
                size_t popped_key;
                size_t next_key;
            };
            std::vector< failed_pops > m_arrFailedPops;
        };

    protected:

        template <class PQueue>
        void test( PQueue& q )
        {
            cds_test::thread_pool& pool = get_pool();

            // push
            {
                std::vector< size_t > arr;
                arr.reserve( s_nQueueSize );
                for ( size_t i = 0; i < s_nQueueSize; ++i )
                    arr.push_back( i );
                shuffle( arr.begin(), arr.end());

                size_t nPushError = 0;
                typedef typename PQueue::value_type value_type;
                for ( auto it = arr.begin(); it != arr.end(); ++it ) {
                    if ( !q.push( value_type( *it )))
                        ++nPushError;
                }
                s_nQueueSize -= nPushError;
            }

            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "push_count", s_nQueueSize );

            // pop
            {
                pool.add( new Consumer<PQueue>( pool, q ), s_nThreadCount );

                std::chrono::milliseconds duration = pool.run();
                propout() << std::make_pair( "consumer_duration", duration );

                // Analyze result
                size_t nTotalPopped = 0;
                size_t nTotalError = 0;
                size_t nTotalErrorEq = 0;
                size_t nTotalFailed = 0;
                for ( size_t i = 0; i < pool.size(); ++i ) {
                    Consumer<PQueue>& cons = static_cast<Consumer<PQueue>&>( pool.get(i));

                    nTotalPopped  += cons.m_nPopSuccess;
                    nTotalError   += cons.m_nPopError;
                    nTotalErrorEq += cons.m_nPopErrorEq;
                    nTotalFailed  += cons.m_nPopFailed;

                    if ( !cons.m_arrFailedPops.empty()) {
                        std::cerr << "Priority violations, thread " << i;
                        for ( size_t k = 0; k < cons.m_arrFailedPops.size(); ++k ) {
                            std::cerr << "\n    " << "prev_key=" << cons.m_arrFailedPops[k].prev_key << " popped_key=" << cons.m_arrFailedPops[k].popped_key;
                            if ( cons.m_arrFailedPops[k].next_key != static_cast<size_t>(-1))
                                std::cerr << " next_key=" << cons.m_arrFailedPops[k].next_key;
                            else
                                std::cerr << " next_key unspecified";
                        }
                        std::cerr << std::endl;
                    }
                }

                propout()
                    << std::make_pair( "total_popped", nTotalPopped )
                    << std::make_pair( "error_pop_double", nTotalErrorEq )
                    << std::make_pair( "error_priority_violation", nTotalError );

                EXPECT_EQ( nTotalPopped, s_nQueueSize );
                EXPECT_EQ( nTotalError, 0u ) << "priority violations";
                EXPECT_EQ( nTotalErrorEq, 0u ) << "double key";
            }

            propout() << q.statistics();
        }

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "pqueue_pop" );

            s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
            s_nQueueSize = cfg.get_size_t( "QueueSize", s_nQueueSize );

            if ( s_nThreadCount == 0u )
                s_nThreadCount = 1;
            if ( s_nQueueSize == 0u )
                s_nQueueSize = 1000;
        }

        //static void TearDownTestCase();
    };

#define CDSSTRESS_MSPriorityQueue( fixture_t, pqueue_t ) \
    TEST_F( fixture_t, pqueue_t ) \
    { \
        typedef pqueue::Types<pqueue::simple_value>::pqueue_t pqueue_type; \
        pqueue_type pq( s_nQueueSize + 1 ); \
        test( pq ); \
    }
    CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_dyn_less )
    CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_dyn_less_stat )
    CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_dyn_cmp )
    //CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_dyn_mutex ) // too slow

#define CDSSTRESS_MSPriorityQueue_static( fixture_t, pqueue_t ) \
    TEST_F( fixture_t, pqueue_t ) \
    { \
        typedef pqueue::Types<pqueue::simple_value>::pqueue_t pqueue_type; \
        std::unique_ptr< pqueue_type > pq( new pqueue_type ); \
        test( *pq.get()); \
    }
    //CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_static_less )
    //CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_static_less_stat )
    //CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_static_cmp )
    //CDSSTRESS_MSPriorityQueue( pqueue_pop, MSPriorityQueue_static_mutex )


#define CDSSTRESS_PriorityQueue( fixture_t, pqueue_t ) \
    TEST_F( fixture_t, pqueue_t ) \
    { \
        typedef pqueue::Types<pqueue::simple_value>::pqueue_t pqueue_type; \
        pqueue_type pq; \
        test( pq ); \
    }
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_vector )
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_vector_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_deque )
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_deque_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_boost_deque )
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_boost_deque_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_boost_stable_vector )
    CDSSTRESS_PriorityQueue( pqueue_pop, FCPQueue_boost_stable_vector_stat )

    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_HP_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_HP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_HP_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_HP_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_DHP_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_DHP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_DHP_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_DHP_min_stat )
    // CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpi_max )
    // CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpi_max_stat )
    // CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpi_min )
    // CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpi_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpb_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpb_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpb_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpb_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpt_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpt_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpt_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_gpt_min_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_shb_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_shb_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_shb_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_shb_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_sht_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_sht_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_sht_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, EllenBinTree_RCU_sht_min_stat )
#endif

    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_HP_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_HP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_HP_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_HP_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_DHP_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_DHP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_DHP_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_DHP_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_gpi_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_gpi_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_gpb_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_gpb_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_gpt_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_gpt_min )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_shb_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_shb_min )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_sht_max )
    CDSSTRESS_PriorityQueue( pqueue_pop, SkipList_RCU_sht_min )
#endif

    CDSSTRESS_PriorityQueue( pqueue_pop, StdPQueue_vector_spin )
    CDSSTRESS_PriorityQueue( pqueue_pop, StdPQueue_vector_mutex )
    CDSSTRESS_PriorityQueue( pqueue_pop, StdPQueue_deque_spin )
    CDSSTRESS_PriorityQueue( pqueue_pop, StdPQueue_deque_mutex )

} // namespace
