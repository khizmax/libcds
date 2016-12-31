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
    static size_t s_nPushThreadCount = 4;
    static size_t s_nPopThreadCount = 4;
    static size_t s_nQueueSize = 2000000;

    atomics::atomic<size_t>  s_nProducerCount(0);

    class pqueue_push_pop: public cds_test::stress_fixture
    {
        typedef cds_test::stress_fixture base_class;

    public:
        enum {
            producer_thread,
            consumer_thread
        };

        template <class PQueue>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Producer( cds_test::thread_pool& pool, PQueue& queue )
                : base_class( pool, producer_thread )
                , m_Queue( queue )
            {}

            Producer( Producer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
            {}

            virtual thread * clone()
            {
                return new Producer( *this );
            }

            virtual void test()
            {
                typedef typename PQueue::value_type value_type;

                for ( auto it = m_arr.begin(); it != m_arr.end(); ++it ) {
                    if ( !m_Queue.push( value_type( *it )))
                        ++m_nPushError;
                }

                s_nProducerCount.fetch_sub( 1, atomics::memory_order_relaxed );
            }

            void prepare( size_t nStart, size_t nEnd )
            {
                m_arr.reserve( nEnd - nStart );
                for ( size_t i = nStart; i < nEnd; ++i )
                    m_arr.push_back( i );
                shuffle( m_arr.begin(), m_arr.end());
            }

        public:
            PQueue&             m_Queue;
            size_t              m_nPushError = 0;

            typedef std::vector<size_t> array_type;
            array_type          m_arr;
        };

        template <class PQueue>
        class Consumer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Consumer( cds_test::thread_pool& pool, PQueue& queue )
                : base_class( pool, consumer_thread )
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
                typename PQueue::value_type val;
                while ( s_nProducerCount.load( atomics::memory_order_relaxed ) != 0 || !m_Queue.empty()) {
                    if ( m_Queue.pop( val ))
                        ++m_nPopSuccess;
                    else
                        ++m_nPopFailed;
                }
            }

        public:
            PQueue&             m_Queue;
            size_t              m_nPopSuccess = 0;
            size_t              m_nPopFailed = 0;

            typedef std::vector<size_t> array_type;
            array_type          m_arr;
        };

    protected:

        template <class PQueue>
        void test( PQueue& q )
        {
            size_t const nThreadItemCount = s_nQueueSize / s_nPushThreadCount;
            s_nQueueSize = nThreadItemCount * s_nPushThreadCount;

            propout() << std::make_pair( "producer_count", s_nPushThreadCount )
                << std::make_pair( "consunmer_count", s_nPopThreadCount )
                << std::make_pair( "queue_size", s_nQueueSize );

            cds_test::thread_pool& pool = get_pool();
            pool.add( new Producer<PQueue>( pool, q ), s_nPushThreadCount );

            size_t nStart = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                static_cast<Producer<PQueue>&>(pool.get( i )).prepare( nStart, nStart + nThreadItemCount );
                nStart += nThreadItemCount;
            }

            pool.add( new Consumer<PQueue>( pool, q ), s_nPopThreadCount );

            s_nProducerCount.store( s_nPushThreadCount, atomics::memory_order_release );

            std::chrono::milliseconds duration = pool.run();
            propout() << std::make_pair( "duration", duration );

            // Analyze result
            size_t nTotalPopped = 0;
            size_t nPushFailed = 0;
            size_t nPopFailed = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                cds_test::thread& t = pool.get(i);
                if ( t.type() == consumer_thread ) {
                    Consumer<PQueue>& cons = static_cast<Consumer<PQueue>&>( t );
                    nTotalPopped += cons.m_nPopSuccess;
                    nPopFailed += cons.m_nPopFailed;
                }
                else {
                    assert( t.type() == producer_thread );
                    Producer<PQueue>& prod = static_cast<Producer<PQueue>&>(t);
                    nPushFailed += prod.m_nPushError;
                    EXPECT_EQ( prod.m_nPushError , 0u ) << "producer " << i;
                }
            }

            propout() << std::make_pair( "total_popped", nTotalPopped )
                << std::make_pair( "empty_pop", nPopFailed )
                << std::make_pair( "push_error", nPushFailed );

            EXPECT_EQ( nTotalPopped, s_nQueueSize );
            EXPECT_EQ( nPushFailed, 0u );

            //check_statistics( testQueue.statistics());
            propout() << q.statistics();
        }

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "pqueue_push_pop" );

            s_nPushThreadCount = cfg.get_size_t( "PushThreadCount", s_nPushThreadCount );
            s_nPopThreadCount = cfg.get_size_t( "PopThreadCount", s_nPopThreadCount );
            s_nQueueSize = cfg.get_size_t( "QueueSize", s_nQueueSize );

            if ( s_nPushThreadCount == 0u )
                s_nPushThreadCount = 1;
            if ( s_nPopThreadCount == 0u )
                s_nPopThreadCount = 1;
            if ( s_nQueueSize == 0u )
                s_nQueueSize = 1000;
        }

        //static void TearDownTestCase();
    };

#define CDSSTRESS_MSPriorityQueue( fixture_t, pqueue_t ) \
    TEST_F( fixture_t, pqueue_t ) \
    { \
        typedef pqueue::Types<pqueue::simple_value>::pqueue_t pqueue_type; \
        pqueue_type pq( s_nQueueSize ); \
        test( pq ); \
    }
    CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_dyn_less )
    CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_dyn_less_stat )
    CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_dyn_cmp )
    //CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_dyn_mutex ) // too slow

#define CDSSTRESS_MSPriorityQueue_static( fixture_t, pqueue_t ) \
    TEST_F( fixture_t, pqueue_t ) \
    { \
        typedef pqueue::Types<pqueue::simple_value>::pqueue_t pqueue_type; \
        std::unique_ptr< pqueue_type > pq( new pqueue_type ); \
        test( *pq.get()); \
    }
    //CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_static_less )
    //CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_static_less_stat )
    //CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_static_cmp )
    //CDSSTRESS_MSPriorityQueue( pqueue_push_pop, MSPriorityQueue_static_mutex )


#define CDSSTRESS_PriorityQueue( fixture_t, pqueue_t ) \
    TEST_F( fixture_t, pqueue_t ) \
    { \
        typedef pqueue::Types<pqueue::simple_value>::pqueue_t pqueue_type; \
        pqueue_type pq; \
        test( pq ); \
    }
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_vector )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_vector_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_deque )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_deque_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_boost_deque )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_boost_deque_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_boost_stable_vector )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, FCPQueue_boost_stable_vector_stat )

    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_HP_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_HP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_HP_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_HP_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_DHP_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_DHP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_DHP_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_DHP_min_stat )
    // CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpi_max )
    // CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpi_max_stat )
    // CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpi_min )
    // CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpi_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpb_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpb_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpb_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpb_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpt_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpt_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpt_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_gpt_min_stat )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_shb_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_shb_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_shb_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_shb_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_sht_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_sht_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_sht_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, EllenBinTree_RCU_sht_min_stat )
#endif

    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_HP_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_HP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_HP_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_HP_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_DHP_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_DHP_max_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_DHP_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_DHP_min_stat )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_gpi_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_gpi_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_gpb_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_gpb_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_gpt_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_gpt_min )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_shb_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_shb_min )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_sht_max )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, SkipList_RCU_sht_min )
#endif

    CDSSTRESS_PriorityQueue( pqueue_push_pop, StdPQueue_vector_spin )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, StdPQueue_vector_mutex )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, StdPQueue_deque_spin )
    CDSSTRESS_PriorityQueue( pqueue_push_pop, StdPQueue_deque_mutex )

} // namespace
