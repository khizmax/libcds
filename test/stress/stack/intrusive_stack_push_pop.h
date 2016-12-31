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

#include "intrusive_stack_type.h"

namespace cds_test {

    class intrusive_stack_push_pop : public cds_test::stress_fixture
    {
    protected:
        static size_t s_nPushThreadCount;
        static size_t s_nPopThreadCount;
        static size_t s_nStackSize;
        static size_t s_nEliminationSize;
        static bool s_bFCIterative;
        static unsigned int s_nFCCombinePassCount;
        static unsigned int s_nFCCompactFactor;

        static atomics::atomic<size_t>  s_nWorkingProducers;

        static constexpr const size_t c_nValArraySize = 1024;
        static constexpr const size_t c_nBadConsumer = 0xbadc0ffe;

        enum thread_type
        {
            producer_thread,
            consumer_thread
        };

        struct empty
        {};

        template <typename Base = empty >
        struct value_type : public Base
        {
            atomics::atomic<size_t> nNo;
            size_t      nProducer;
            size_t      nConsumer;

            value_type() {}
            value_type( size_t n ) : nNo( n ) {}
        };


        template <class Stack>
        class Producer : public cds_test::thread
        {
            typedef cds_test::thread base_class;
        public:
            Stack&              m_Stack;
            size_t              m_nPushError;
            size_t              m_arrPush[c_nValArraySize];

            // Interval in m_arrValue
            typename Stack::value_type *       m_pStart;
            typename Stack::value_type *       m_pEnd;

        public:
            Producer( cds_test::thread_pool& pool, Stack& s )
                : base_class( pool, producer_thread )
                , m_Stack( s )
                , m_nPushError( 0 )
                , m_pStart( nullptr )
                , m_pEnd( nullptr )
            {}

            Producer( Producer& src )
                : base_class( src )
                , m_Stack( src.m_Stack )
                , m_nPushError( 0 )
                , m_pStart( nullptr )
                , m_pEnd( nullptr )
            {}

            virtual thread * clone()
            {
                return new Producer( *this );
            }

            virtual void test()
            {
                m_nPushError = 0;
                memset( m_arrPush, 0, sizeof( m_arrPush ));

                size_t i = 0;
                for ( typename Stack::value_type * p = m_pStart; p < m_pEnd; ++p, ++i ) {
                    p->nProducer = id();
                    size_t no;
                    p->nNo.store( no = i % c_nValArraySize, atomics::memory_order_release );
                    if ( m_Stack.push( *p ))
                        ++m_arrPush[no];
                    else
                        ++m_nPushError;
                }

                s_nWorkingProducers.fetch_sub( 1, atomics::memory_order_release );
            }
        };

        template <class Stack>
        class Consumer : public cds_test::thread
        {
            typedef cds_test::thread base_class;
        public:
            Stack&              m_Stack;
            size_t              m_nPopCount;
            size_t              m_nPopEmpty;
            size_t              m_arrPop[c_nValArraySize];
            size_t              m_nDirtyPop;
        public:
            Consumer( cds_test::thread_pool& pool, Stack& s )
                : base_class( pool, consumer_thread )
                , m_Stack( s )
                , m_nPopCount( 0 )
                , m_nPopEmpty( 0 )
                , m_nDirtyPop( 0 )
            {}

            Consumer( Consumer& src )
                : base_class( src )
                , m_Stack( src.m_Stack )
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
                m_nPopEmpty = 0;
                m_nPopCount = 0;
                m_nDirtyPop = 0;
                memset( m_arrPop, 0, sizeof( m_arrPop ));

                while ( !(s_nWorkingProducers.load( atomics::memory_order_acquire ) == 0 && m_Stack.empty())) {
                    typename Stack::value_type * p = m_Stack.pop();
                    if ( p ) {
                        p->nConsumer = id();
                        ++m_nPopCount;
                        size_t no = p->nNo.load( atomics::memory_order_acquire );
                        if ( no < sizeof( m_arrPop ) / sizeof( m_arrPop[0] ))
                            ++m_arrPop[no];
                        else
                            ++m_nDirtyPop;
                    }
                    else
                        ++m_nPopEmpty;
                }
            }
        };

        template <typename T>
        class value_array
        {
            std::unique_ptr< T[] > m_pArr;

        public:
            value_array( size_t nSize )
                : m_pArr( new T[nSize] )
            {}

            T * get() const { return m_pArr.get(); }
        };

    public:
        static void SetUpTestCase();
        //static void TearDownTestCase();

    protected:
        template <class Stack>
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
                    Producer<Stack>& producer = static_cast<Producer<Stack>&>(thread);
                    nPushError += producer.m_nPushError;
                    for ( size_t i = 0; i < sizeof( arrVal ) / sizeof( arrVal[0] ); ++i )
                        arrVal[i] += producer.m_arrPush[i];
                }
                else {
                    ASSERT_TRUE( thread.type() == consumer_thread );
                    Consumer<Stack>& consumer = static_cast<Consumer<Stack>&>(thread);
                    nPopEmpty += consumer.m_nPopEmpty;
                    nPopCount += consumer.m_nPopCount;
                    nDirtyPop += consumer.m_nDirtyPop;
                    for ( size_t i = 0; i < sizeof( arrVal ) / sizeof( arrVal[0] ); ++i )
                        arrVal[i] -= consumer.m_arrPop[i];
                }
            }

            EXPECT_EQ( nPopCount, s_nStackSize );
            EXPECT_EQ( nDirtyPop, 0u );
            EXPECT_EQ( nPushError, 0u );

            for ( size_t i = 0; i < sizeof( arrVal ) / sizeof( arrVal[0] ); ++i ) {
                EXPECT_EQ( arrVal[i], 0u ) << "i=" << i;
            }

            propout() << std::make_pair( "push_count", s_nStackSize )
                << std::make_pair( "push_error", nPushError )
                << std::make_pair( "pop_count", nPopCount )
                << std::make_pair( "pop_empty", nPopEmpty )
                << std::make_pair( "dirty_pop", nDirtyPop )
;

        }

        template <typename Stack>
        void do_test( Stack& stack, value_array<typename Stack::value_type>& arrValue )
        {
            cds_test::thread_pool& pool = get_pool();

            s_nWorkingProducers.store( s_nPushThreadCount, atomics::memory_order_release );
            size_t const nPushCount = s_nStackSize / s_nPushThreadCount;

            typename Stack::value_type * pValStart = arrValue.get();
            typename Stack::value_type * pValEnd = pValStart + s_nStackSize;

            pool.add( new Producer<Stack>( pool, stack ), s_nPushThreadCount );
            {
                for ( typename Stack::value_type * it = pValStart; it != pValEnd; ++it )
                    it->nConsumer = c_nBadConsumer;

                typename Stack::value_type * pStart = pValStart;
                for ( size_t thread_no = 0; thread_no < pool.size(); ++thread_no ) {
                    static_cast<Producer<Stack>&>(pool.get( thread_no )).m_pStart = pStart;
                    pStart += nPushCount;
                    static_cast<Producer<Stack>&>(pool.get( thread_no )).m_pEnd = pStart;
                }
            }
            pool.add( new Consumer<Stack>( pool, stack ), s_nPopThreadCount );

            propout() << std::make_pair( "producer_thread_count", s_nPushThreadCount )
                << std::make_pair( "consumer_thread_count", s_nPopThreadCount )
                << std::make_pair( "push_count", nPushCount * s_nPushThreadCount )
;

            std::chrono::milliseconds duration = pool.run();

            propout() << std::make_pair( "duration", duration );

            s_nStackSize = nPushCount * s_nPushThreadCount;

            {
                typename Stack::value_type * pEnd = pValStart + s_nStackSize;
                size_t const nBadConsumer = c_nBadConsumer;
                for ( typename Stack::value_type * it = pValStart; it != pEnd; ++it )
                    EXPECT_NE( it->nConsumer, nBadConsumer );
            }

            analyze( stack );

            propout() << stack.statistics();
        }
    };
} // namespace cds_test
