// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "queue_type.h"

// Multi-threaded queue test for pop operation
namespace {

    static size_t s_nThreadCount = 8;
    static size_t s_nQueueSize = 20000000 ;   // no more than 20 million records

        struct SimpleValue {
            size_t    nNo;

            SimpleValue(): nNo(0) {}
            SimpleValue( size_t n ): nNo(n) {}
            size_t getNo() const { return  nNo; }
        };

    class queue_pop: public cds_test::stress_fixture
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
        class Consumer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Consumer( cds_test::thread_pool& pool, Queue& queue )
                : base_class( pool )
                , m_Queue( queue )
                , m_arr( new uint8_t[ s_nQueueSize ])
                , m_nPopCount( 0 )
            {}

            Consumer( Consumer& src )
                : base_class( src )
                , m_Queue( src.m_Queue )
                , m_arr( new uint8_t[ s_nQueueSize ])
                , m_nPopCount( 0 )
            {}

            virtual thread * clone()
            {
                return new Consumer( *this );
            }

            virtual void test()
            {
                memset( m_arr.get(), 0, sizeof( m_arr[0] ) * s_nQueueSize );
                typedef typename Queue::value_type value_type;
                value_type value;
                size_t nPopCount = 0;
                while ( m_Queue.pop( value )) {
                    ++m_arr[ value.nNo ];
                    ++nPopCount;
                }
                m_nPopCount = nPopCount;
            }

        public:
            Queue&              m_Queue;
            std::unique_ptr< uint8_t[] > m_arr;
            size_t              m_nPopCount;
        };

    public:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config( "queue_pop" );

            s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
            s_nQueueSize = cfg.get_size_t( "QueueSize", s_nQueueSize );

            if ( s_nThreadCount == 0 )
                s_nThreadCount = 1;
            if ( s_nQueueSize == 0 )
                s_nQueueSize = 1000;
        }

        //static void TearDownTestCase();

    protected:
        template <class Queue>
        void analyze( Queue& q )
        {
            cds_test::thread_pool& pool = get_pool();
            std::unique_ptr< uint8_t[] > arr( new uint8_t[s_nQueueSize] );
            memset(arr.get(), 0, sizeof(arr[0]) * s_nQueueSize );

            size_t nTotalPops = 0;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                Consumer<Queue>& thread = static_cast<Consumer<Queue>&>(pool.get( i ));
                for ( size_t j = 0; j < s_nQueueSize; ++j )
                    arr[j] += thread.m_arr[j];
                nTotalPops += thread.m_nPopCount;
            }
            EXPECT_EQ( nTotalPops, s_nQueueSize );
            EXPECT_TRUE( q.empty());

            for ( size_t i = 0; i < s_nQueueSize; ++i ) {
                EXPECT_EQ( arr[i], 1 ) << "i=" << i;
            }
        }

        template <class Queue>
        void test( Queue& q )
        {
            cds_test::thread_pool& pool = get_pool();

            pool.add( new Consumer<Queue>( pool, q ), s_nThreadCount );

            for ( size_t i = 0; i < s_nQueueSize; ++i ) {
                q.push( i );
            }



            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "push_count", s_nQueueSize );

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            analyze( q );

            propout() << q.statistics();
        }
    };


    CDSSTRESS_MSQueue( queue_pop )
    CDSSTRESS_SPQueue( queue_pop )
    CDSSTRESS_MoirQueue( queue_pop )
    CDSSTRESS_BasketQueue( queue_pop )
    CDSSTRESS_OptimsticQueue( queue_pop )
    CDSSTRESS_FCQueue( queue_pop )
    CDSSTRESS_FCDeque( queue_pop )
    CDSSTRESS_RWQueue( queue_pop )
    CDSSTRESS_StdQueue( queue_pop )

#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F( test_fixture, type_name ) \
    TEST_F( test_fixture, type_name ) \
    { \
        typedef queue::Types< value_type >::type_name queue_type; \
        queue_type queue( s_nQueueSize ); \
        test( queue ); \
    }


    CDSSTRESS_VyukovQueue( queue_pop )


#undef CDSSTRESS_Queue_F


    // ********************************************************************
    // SegmentedQueue test

    class segmented_queue_pop
        : public queue_pop
        , public ::testing::WithParamInterface< size_t >
    {
        typedef queue_pop base_class;

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
            cds_test::config const& cfg = cds_test::stress_fixture::get_config( "queue_pop" );
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

    CDSSTRESS_SegmentedQueue( segmented_queue_pop )


#ifdef CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG
    static std::string get_test_parameter_name( testing::TestParamInfo<size_t> const& p )
    {
        return std::to_string( p.param );
    }
    INSTANTIATE_TEST_CASE_P( SQ,
        segmented_queue_pop,
        ::testing::ValuesIn( segmented_queue_pop::get_test_parameters()), get_test_parameter_name );
#else
    INSTANTIATE_TEST_CASE_P( SQ,
        segmented_queue_pop,
        ::testing::ValuesIn( segmented_queue_pop::get_test_parameters()));
#endif


} // namespace
