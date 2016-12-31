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

#include "stack_type.h"

namespace {

    static size_t s_nPushThreadCount = 4;
    static size_t s_nPopThreadCount = 4;
    static size_t s_nStackSize = 1000000;
    static size_t s_nEliminationSize = 4;

    static atomics::atomic<size_t>  s_nWorkingProducers( 0 );

    class stack_push_pop : public cds_test::stress_fixture
    {
    protected:
        enum thread_type
        {
            producer_thread,
            consumer_thread
        };

        struct value_type {
            size_t      nNo;
            size_t      nThread;

            value_type()
                : nNo( 0 )
                , nThread( 0 )
            {}
            value_type( size_t n )
                : nNo( n )
                , nThread( 0 )
            {}
        };

        static size_t const c_nValArraySize = 1024;

        template <class Stack>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Producer( cds_test::thread_pool& pool, Stack& stack, size_t push_count )
                : base_class( pool, producer_thread )
                , m_stack( stack )
                , m_nItemCount( push_count )
                , m_nPushError( 0 )
            {}

            Producer( Producer& src )
                : base_class( src )
                , m_stack( src.m_stack )
                , m_nItemCount( src.m_nItemCount )
                , m_nPushError( 0 )
            {}

            virtual thread * clone()
            {
                return new Producer( *this );
            }

            virtual void test()
            {
                memset( m_arrPush, 0, sizeof( m_arrPush ));

                value_type v;
                v.nThread = id();
                for ( size_t i = 0; i < m_nItemCount; ++i ) {
                    v.nNo = i % c_nValArraySize;
                    if ( m_stack.push( v ))
                        ++m_arrPush[v.nNo];
                    else
                        ++m_nPushError;
                }

                s_nWorkingProducers.fetch_sub( 1, atomics::memory_order_release );
            }

        public:
            Stack&  m_stack;
            size_t const m_nItemCount;
            size_t  m_nPushError;
            size_t  m_arrPush[c_nValArraySize];
        };

        template <class Stack>
        class Consumer : public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Consumer( cds_test::thread_pool& pool, Stack& stack )
                : base_class( pool, consumer_thread )
                , m_stack( stack )
                , m_nPopCount( 0 )
                , m_nPopEmpty( 0 )
                , m_nDirtyPop( 0 )
            {}

            Consumer( Consumer& src )
                : base_class( src )
                , m_stack( src.m_stack )
                , m_nPopCount( 0 )
                , m_nPopEmpty( 0 )
                , m_nDirtyPop( 0 )
            {}

            virtual thread * clone()
            {
                return new Consumer( *this );
            }

            virtual void test()
            {
                memset( m_arrPop, 0, sizeof( m_arrPop ));

                value_type v;
                while ( !( s_nWorkingProducers.load( atomics::memory_order_acquire ) == 0 && m_stack.empty())) {
                    if ( m_stack.pop( v )) {
                        ++m_nPopCount;
                        if ( v.nNo < sizeof( m_arrPop ) / sizeof( m_arrPop[0] ))
                            ++m_arrPop[v.nNo];
                        else
                            ++m_nDirtyPop;
                    }
                    else
                        ++m_nPopEmpty;
                }
            }

        public:
            Stack& m_stack;
            size_t m_nPopCount;
            size_t m_nPopEmpty;
            size_t m_arrPop[c_nValArraySize];
            size_t m_nDirtyPop;
        };

    protected:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config("Stack_PushPop");

            s_nPushThreadCount = cfg.get_size_t( "PushThreadCount", s_nPushThreadCount );
            s_nPopThreadCount  = cfg.get_size_t( "PopThreadCount",  s_nPopThreadCount );
            s_nStackSize       = cfg.get_size_t( "StackSize",       s_nStackSize );
            s_nEliminationSize = cfg.get_size_t( "EliminationSize", s_nEliminationSize );

            if ( s_nPushThreadCount == 0 )
                s_nPushThreadCount = 1;
            if ( s_nPopThreadCount == 0 )
                s_nPopThreadCount = 1;
        }

        //static void TearDownTestCase();

        template <typename Stack>
        void test( Stack& stack )
        {
            cds_test::thread_pool& pool = get_pool();
            size_t const nPushCount = s_nStackSize / s_nPushThreadCount;

            pool.add( new Producer<Stack>( pool, stack, nPushCount ), s_nPushThreadCount );
            pool.add( new Consumer<Stack>( pool, stack ), s_nPopThreadCount );

            s_nWorkingProducers.store( s_nPushThreadCount );
            s_nStackSize = nPushCount * s_nPushThreadCount;

            propout() << std::make_pair( "producer_thread_count", s_nPushThreadCount )
                << std::make_pair( "consumer_thread_count", s_nPopThreadCount )
                << std::make_pair( "push_count", s_nStackSize )
;

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            analyze( stack );

            propout() << stack.statistics();
        }

        template <typename Stack>
        void test_elimination( Stack& stack )
        {
            test( stack );
            check_elimination_stat( stack.statistics());
        }

        void check_elimination_stat( cds::container::treiber_stack::empty_stat const& )
        {}

        void check_elimination_stat( cds::container::treiber_stack::stat<> const& s )
        {
            EXPECT_EQ( s.m_PushCount.get() + s.m_ActivePushCollision.get() + s.m_PassivePushCollision.get(), s_nStackSize );
            EXPECT_EQ( s.m_PopCount.get() + s.m_ActivePopCollision.get() + s.m_PassivePopCollision.get(), s_nStackSize );
            EXPECT_EQ( s.m_PushCount.get(), s.m_PopCount.get());
            EXPECT_EQ( s.m_ActivePopCollision.get(), s.m_PassivePushCollision.get());
            EXPECT_EQ( s.m_ActivePushCollision.get(), s.m_PassivePopCollision.get());
        }

        template< class Stack>
        void analyze( Stack& /*stack*/ )
        {
            cds_test::thread_pool& pool = get_pool();

            size_t nPushError = 0;
            size_t nPopEmpty = 0;
            size_t nPopCount = 0;
            size_t arrVal[c_nValArraySize];
            memset( arrVal, 0, sizeof( arrVal ));
            size_t nDirtyPop = 0;

            for ( size_t threadNo = 0; threadNo < pool.size(); ++threadNo ) {
                cds_test::thread& thread = pool.get( threadNo );
                if ( thread.type() == producer_thread ) {
                    Producer<Stack>& producer = static_cast<Producer<Stack>&>( thread );

                    nPushError += producer.m_nPushError;
                    for ( size_t i = 0; i < c_nValArraySize; ++i )
                        arrVal[i] += producer.m_arrPush[i];
                }
                else {
                    ASSERT_EQ( thread.type(), consumer_thread );
                    Consumer<Stack>& consumer = static_cast<Consumer<Stack>&>(thread);

                    nPopEmpty += consumer.m_nPopEmpty;
                    nPopCount += consumer.m_nPopCount;
                    nDirtyPop += consumer.m_nDirtyPop;
                    for ( size_t i = 0; i < c_nValArraySize; ++i )
                        arrVal[i] -= consumer.m_arrPop[i];
                }
            }

            EXPECT_EQ( nPopCount, s_nStackSize );
            EXPECT_EQ( nDirtyPop, 0u );

            for ( size_t i = 0; i < sizeof( arrVal ) / sizeof( arrVal[0] ); ++i ) {
                EXPECT_EQ( arrVal[i], 0u );
            }

            propout() << std::make_pair( "push_count", s_nStackSize )
                      << std::make_pair( "push_error", nPushError )
                      << std::make_pair( "pop_empty", nPopEmpty )
                      << std::make_pair( "dirty_pop", nDirtyPop )
;
        }
    };

    CDSSTRESS_TreiberStack( stack_push_pop )
    CDSSTRESS_EliminationStack( stack_push_pop )
    CDSSTRESS_FCStack( stack_push_pop )
    CDSSTRESS_FCDeque( stack_push_pop )
    CDSSTRESS_StdStack( stack_push_pop )

} // namespace
