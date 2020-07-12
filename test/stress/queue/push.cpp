// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "queue_type.h"

// Multi-threaded queue test for push operation
namespace {

    static size_t s_nThreadCount = 8;
    static size_t s_nQueueSize = 20000000 ;   // no more than 20 million records

    class queue_push: public cds_test::stress_fixture
    {
    protected:
        struct value_type
        {
            size_t      nNo;

            value_type()
                : nNo( 0 )
            {}

            value_type( size_t n )
                : nNo( n )
            {}
        };

        template <class Queue>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Producer( cds_test::thread_pool& pool, Queue& queue )
                : base_class( pool )
                , m_Queue( queue )
                , m_nStartItem( 0 )
                , m_nEndItem( 0 )
                , m_nPushError( 0 )
            {}

            Producer( Producer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
                , m_nStartItem( 0 )
                , m_nEndItem( 0 )
                , m_nPushError( 0 )
            {}

            virtual thread * clone()
            {
                return new Producer( *this );
            }

            virtual void test()
            {
                for ( size_t nItem = m_nStartItem; nItem < m_nEndItem; ++nItem ) {
                    if ( !m_Queue.push( nItem )) {
                        ++m_nPushError;
                    }
                    //EXPECT_TRUE(false) << "nItem = " << nItem;
                }
            }

        public:
            Queue&              m_Queue;
            size_t              m_nStartItem;
            size_t              m_nEndItem;
            size_t              m_nPushError;
        };

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "queue_push" );

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
        void test( Queue& q )
        {
    try {
            cds_test::thread_pool& pool = get_pool();

            pool.add( new Producer<Queue>( pool, q ), s_nThreadCount );

            size_t nStart = 0;
            size_t nThreadItemCount = s_nQueueSize / s_nThreadCount;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                Producer<Queue>& thread = static_cast<Producer<Queue>&>(pool.get( i ));
                thread.m_nStartItem = nStart;
                nStart += nThreadItemCount;
                thread.m_nEndItem = nStart;
            }

            s_nQueueSize = nThreadItemCount * s_nThreadCount;
            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "push_count", s_nQueueSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            analyze( q );

            propout() << q.statistics();
    }
    catch (std::exception& e)
    {
        EXPECT_TRUE(false) << "Exception catched : " << e.what();
    }
        }

        template <class Queue>
        void analyze( Queue& q )
        {
            size_t nThreadItems = s_nQueueSize / s_nThreadCount;
            cds_test::thread_pool& pool = get_pool();

            for ( size_t i = 0; i < pool.size(); ++i ) {
                Producer<Queue>& thread = static_cast<Producer<Queue>&>(pool.get( i ));
                EXPECT_EQ( thread.m_nPushError, 0u ) << " producer thread " << i;
            }
            EXPECT_TRUE( !q.empty());

            std::unique_ptr< uint8_t[] > arr( new uint8_t[s_nQueueSize] );
            memset( arr.get(), 0, sizeof(arr[0]) * s_nQueueSize );

            size_t nPopped = 0;
            value_type val;

            while ( q.pop( val )) {
                nPopped++;
                ++arr[ val.nNo ];
            }

            size_t nTotalItems = nThreadItems * s_nThreadCount;
            for ( size_t i = 0; i < nTotalItems; ++i ) {
                EXPECT_EQ( arr[i], 1 ) << "i=" << i;
            }
        }
    };


    CDSSTRESS_MSQueue( queue_push )
    CDSSTRESS_SPQueue( queue_push )
    CDSSTRESS_MoirQueue( queue_push )
    CDSSTRESS_BasketQueue( queue_push )
    CDSSTRESS_OptimsticQueue( queue_push )
    CDSSTRESS_FCQueue( queue_push )
    CDSSTRESS_FCDeque( queue_push )
    CDSSTRESS_RWQueue( queue_push )
    CDSSTRESS_StdQueue( queue_push )


#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue( s_nQueueSize ); \
        test( queue ); \
    }

    CDSSTRESS_VyukovQueue( queue_push )

#undef CDSSTRESS_Queue_F


    // ********************************************************************
    // SegmentedQueue test

    class segmented_queue_push
        : public queue_push
        , public ::testing::WithParamInterface< size_t >
    {
        typedef queue_push base_class;

    protected:
        template <typename Queue>
        void test()
        {
            size_t quasi_factor = GetParam();

            Queue q( quasi_factor );
            propout() << std::make_pair( "quasi_factor", quasi_factor );
            base_class::test( q );
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

    CDSSTRESS_SegmentedQueue( segmented_queue_push )

#ifdef CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG
    static std::string get_test_parameter_name( testing::TestParamInfo<size_t> const& p )
    {
        return std::to_string( p.param );
    }
    INSTANTIATE_TEST_CASE_P( SQ,
        segmented_queue_push,
        ::testing::ValuesIn( segmented_queue_push::get_test_parameters()), get_test_parameter_name );
#else
        INSTANTIATE_TEST_CASE_P( SQ,
            segmented_queue_push,
            ::testing::ValuesIn( segmented_queue_push::get_test_parameters()));
#endif


} // namespace
