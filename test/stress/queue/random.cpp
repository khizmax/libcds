// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "queue_type.h"

#include <vector>

// Multi-threaded queue test for random push/pop operation
namespace {

    static size_t s_nThreadCount = 16;
    static size_t s_nQueueSize = 10000000;

    std::atomic< size_t > s_nProducerCount(0);

    class queue_random: public cds_test::stress_fixture
    {
        typedef cds_test::stress_fixture base_class;

    protected:
        struct value_type {
            size_t      nNo;
            size_t      nThread;

            value_type() {}
            value_type( size_t n ) : nNo( n ) {}
        };

        template <class Queue>
        class Strain: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Strain( cds_test::thread_pool& pool, Queue& q, size_t nPushCount, size_t nSpread = 0 )
                : base_class( pool )
                , m_Queue( q )
                , m_nSpread( nSpread )
                , m_nTotalPushCount( nPushCount )
            {}

            Strain( Strain& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
                , m_nSpread( src.m_nSpread )
                , m_nTotalPushCount( src.m_nTotalPushCount )
            {}

            virtual thread * clone()
            {
                return new Strain( *this );
            }

            virtual void test()
            {
                size_t const nThreadCount = s_nThreadCount;
                size_t const nTotalPush = m_nTotalPushCount;

                m_arrLastRead.resize( nThreadCount, 0 );
                m_arrPopCountPerThread.resize( nThreadCount, 0 );

                value_type node;

                while ( m_nPushCount < nTotalPush ) {
                    if ( ( std::rand() & 3) != 3 ) {
                        node.nThread = id();
                        node.nNo = ++m_nPushCount;
                        if ( !m_Queue.push( node )) {
                            ++m_nPushError;
                            --m_nPushCount;
                        }
                    }
                    else
                        pop( nThreadCount );
                }

                s_nProducerCount.fetch_sub( 1, std::memory_order_relaxed );

                while ( !m_Queue.empty() || s_nProducerCount.load( std::memory_order_relaxed ) != 0 )
                    pop( nThreadCount );
            }

            bool pop( size_t nThreadCount )
            {
                value_type node;
                node.nThread = nThreadCount;
                node.nNo = ~0;
                if ( m_Queue.pop( node )) {
                    ++m_nPopCount;
                    if ( node.nThread < nThreadCount ) {
                        m_arrPopCountPerThread[ node.nThread ] += 1;
                        if ( m_nSpread ) {
                            if ( m_arrLastRead[ node.nThread ] > node.nNo ) {
                                if ( m_arrLastRead[ node.nThread ] - node.nNo > m_nSpread )
                                    ++m_nRepeatValue;
                            }
                            else if ( m_arrLastRead[ node.nThread ] == node.nNo )
                                ++m_nRepeatValue;
                            m_arrLastRead[ node.nThread ] = node.nNo;
                        }
                        else {
                            if ( m_arrLastRead[ node.nThread ] < node.nNo )
                                m_arrLastRead[ node.nThread ] = node.nNo;
                            else
                                ++m_nRepeatValue;
                        }
                    }
                    else
                        ++m_nUndefWriter;
                }
                else {
                    ++m_nEmptyPop;
                    return false;
                }
                return true;
            }

        public:
            Queue&              m_Queue;

            size_t  m_nPushCount = 0;
            size_t  m_nPopCount  = 0;
            size_t  m_nEmptyPop  = 0;

            size_t  m_nUndefWriter = 0;
            size_t  m_nRepeatValue = 0;
            size_t  m_nPushError   = 0;

            std::vector<size_t> m_arrLastRead;
            std::vector<size_t> m_arrPopCountPerThread;

            size_t const m_nSpread;
            size_t const m_nTotalPushCount;
        };

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "queue_random" );

            s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
            s_nQueueSize = cfg.get_size_t( "QueueSize", s_nQueueSize );

            if ( s_nThreadCount == 0u )
                s_nThreadCount = 1;
            if ( s_nQueueSize == 0u )
                s_nQueueSize = 1000;
        }

        //static void TearDownTestCase();

    protected:
        template <class Queue>
        void analyze( Queue& q  )
        {
            EXPECT_TRUE( q.empty());

            std::vector< size_t > arrPushCount;
            arrPushCount.resize( s_nThreadCount, 0 );

            size_t nPushTotal = 0;
            size_t nPopTotal  = 0;
            size_t nPushError = 0;

            cds_test::thread_pool& pool = get_pool();
            for ( size_t i = 0; i < pool.size(); ++i ) {
                Strain<Queue>& thr = static_cast<Strain<Queue> &>( pool.get(i));
                EXPECT_EQ( thr.m_nUndefWriter, 0u );
                EXPECT_EQ( thr.m_nRepeatValue, 0u );
                EXPECT_EQ( thr.m_nPushError, 0u );
                nPushError += thr.m_nPushError;

                arrPushCount[ thr.id() ] += thr.m_nPushCount;

                nPushTotal += thr.m_nPushCount;
                nPopTotal += thr.m_nPopCount;
            }

            EXPECT_EQ( nPushTotal, s_nQueueSize );
            EXPECT_EQ( nPopTotal, s_nQueueSize );

            size_t const nThreadPushCount = s_nQueueSize / s_nThreadCount;
            for ( size_t i = 0; i < s_nThreadCount; ++i )
                EXPECT_EQ( arrPushCount[i], nThreadPushCount ) << "thread=" << i;
        }

        template <class Queue>
        void test( Queue& q )
        {
            size_t nThreadPushCount = s_nQueueSize / s_nThreadCount;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new Strain<Queue>( pool, q, nThreadPushCount ), s_nThreadCount );

            s_nQueueSize = nThreadPushCount * s_nThreadCount;
            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "push_count", s_nQueueSize );

            s_nProducerCount.store( pool.size(), std::memory_order_release );
            std::chrono::milliseconds duration = pool.run();
            propout() << std::make_pair( "duration", duration );

            analyze( q );

            propout() << q.statistics();
        }
    };

    CDSSTRESS_MSQueue( queue_random )
    CDSSTRESS_SPQueue( queue_random )
    CDSSTRESS_MoirQueue( queue_random )
    CDSSTRESS_BasketQueue( queue_random )
    CDSSTRESS_OptimsticQueue( queue_random )
    CDSSTRESS_FCQueue( queue_random )
    CDSSTRESS_FCDeque( queue_random )
    CDSSTRESS_RWQueue( queue_random )
    CDSSTRESS_StdQueue( queue_random )

#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue( s_nQueueSize ); \
        test( queue ); \
    }

    CDSSTRESS_VyukovQueue( queue_random )

#undef CDSSTRESS_Queue_F

    // ********************************************************************
    // SegmentedQueue test

    class segmented_queue_random
        : public queue_random
        , public ::testing::WithParamInterface< size_t >
    {
        typedef queue_random base_class;

    protected:
        template <typename Queue>
        void test()
        {
            size_t quasi_factor = GetParam();

            Queue q( quasi_factor );
            propout() << std::make_pair( "quasi_factor", quasi_factor );

            size_t nThreadPushCount = s_nQueueSize / s_nThreadCount;

            cds_test::thread_pool& pool = get_pool();
            pool.add( new Strain<Queue>( pool, q, nThreadPushCount, quasi_factor * 2 ), s_nThreadCount );

            s_nQueueSize = nThreadPushCount * s_nThreadCount;
            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "push_count", s_nQueueSize );

            s_nProducerCount.store( pool.size(), std::memory_order_release );
            std::chrono::milliseconds duration = pool.run();
            propout() << std::make_pair( "duration", duration );

            analyze( q );

            propout() << q.statistics();
        }

    public:
        static std::vector< size_t > get_test_parameters()
        {
            cds_test::config const& cfg = cds_test::stress_fixture::get_config( "queue_push" );
            bool bIterative = cfg.get_bool( "SegmentedQueue_Iterate", false );
            size_t quasi_factor = cfg.get_size_t( "SegmentedQueue_SegmentSize", 256 );

            std::vector<size_t> args;
            if ( bIterative && quasi_factor > 4 ) {
                for ( size_t qf = 4; qf <= quasi_factor; qf *= 2 )
                    args.push_back( qf );
            }
            else {
                if ( quasi_factor > 2 )
                    args.push_back( quasi_factor );
                else
                    args.push_back( 2 );
            }

            return args;
        }
    };

#define CDSSTRESS_Queue_F( test_fixture, type_name ) \
    TEST_P( test_fixture, type_name ) \
    { \
        typedef typename queue::Types<value_type>::type_name queue_type; \
        test< queue_type >(); \
    }

    CDSSTRESS_SegmentedQueue( segmented_queue_random )

#ifdef CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG
    static std::string get_test_parameter_name( testing::TestParamInfo<size_t> const& p )
    {
        return std::to_string( p.param );
    }
    INSTANTIATE_TEST_CASE_P( SQ,
        segmented_queue_random,
        ::testing::ValuesIn( segmented_queue_random::get_test_parameters()), get_test_parameter_name );
#else
    INSTANTIATE_TEST_CASE_P( SQ,
        segmented_queue_random,
        ::testing::ValuesIn( segmented_queue_random::get_test_parameters()));
#endif

} // namespace
