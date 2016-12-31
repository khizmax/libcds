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

    static size_t s_nThreadCount = 8;
    static size_t s_nStackSize = 10000000;
    static size_t s_nEliminationSize = 4;

    class stack_push : public cds_test::stress_fixture
    {
    protected:

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

        template <class Stack>
        class Producer: public cds_test::thread
        {
            typedef cds_test::thread base_class;

        public:
            Producer( cds_test::thread_pool& pool, Stack& stack )
                : base_class( pool )
                , m_stack( stack )
                , m_nStartItem( 0 )
                , m_nEndItem( 0 )
                , m_nPushError( 0 )
            {}

            Producer( Producer& src )
                : base_class( src )
                , m_stack( src.m_stack )
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
                value_type v;
                v.nThread = id();
                for ( v.nNo = m_nStartItem; v.nNo < m_nEndItem; ++v.nNo ) {
                    if ( !m_stack.push( v ))
                        ++m_nPushError;
                }
            }

        public:
            Stack&  m_stack;
            size_t  m_nStartItem;
            size_t  m_nEndItem;
            size_t  m_nPushError;
        };

    protected:
        static void SetUpTestCase()
        {
            cds_test::config const& cfg = get_config("Stack_Push");

            s_nThreadCount     = cfg.get_size_t( "ThreadCount",     s_nThreadCount );
            s_nStackSize       = cfg.get_size_t( "StackSize",       s_nStackSize );
            s_nEliminationSize = cfg.get_size_t( "EliminationSize", s_nEliminationSize );

            if ( s_nThreadCount == 0 )
                s_nThreadCount = 1;
        }

        //static void TearDownTestCase();

        template <typename Stack>
        void test( Stack& stack )
        {
            cds_test::thread_pool& pool = get_pool();

            pool.add( new Producer<Stack>( pool, stack ), s_nThreadCount );

            size_t nStart = 0;
            size_t nThreadItemCount = s_nStackSize / s_nThreadCount;
            for ( size_t i = 0; i < pool.size(); ++i ) {
                Producer<Stack>& thread = static_cast<Producer<Stack>&>( pool.get( i ));
                thread.m_nStartItem = nStart;
                nStart += nThreadItemCount;
                thread.m_nEndItem = nStart;
            }

            propout() << std::make_pair( "thread_count", s_nThreadCount )
                << std::make_pair( "push_count", s_nStackSize );

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
            EXPECT_EQ( s.m_PushCount.get(), s.m_PopCount.get());
        }

        template <class Stack>
        void analyze( Stack& testStack )
        {
            cds_test::thread_pool& pool = get_pool();

            size_t nThreadItems = s_nStackSize / s_nThreadCount;
            std::vector<size_t> aThread;
            aThread.resize( s_nThreadCount );

            for ( size_t i = 0; i < pool.size(); ++i ) {
                Producer<Stack>& producer = static_cast<Producer<Stack>&>( pool.get( i ));
                EXPECT_EQ( producer.m_nPushError, 0u ) << "Producer=" << i;
                aThread[producer.id()] = producer.m_nEndItem - 1;
            }
            EXPECT_FALSE( testStack.empty());

            std::unique_ptr< uint8_t[] > uarr( new uint8_t[s_nStackSize] );
            uint8_t * arr = uarr.get();
            memset( arr, 0, sizeof( arr[0] ) * s_nStackSize );

            auto time_start = std::chrono::steady_clock::now();
            size_t nPopped = 0;
            value_type val;
            while ( testStack.pop( val )) {
                nPopped++;
                ASSERT_LT( val.nNo, s_nStackSize );
                ++arr[val.nNo];
                ASSERT_LT( val.nThread, s_nThreadCount );
                ASSERT_EQ( aThread[val.nThread], val.nNo );
                aThread[val.nThread]--;
            }
            propout() << std::make_pair( "pop_duration", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_start));

            size_t nTotalItems = nThreadItems * s_nThreadCount;
            size_t nError = 0;
            for ( size_t i = 0; i < nTotalItems; ++i ) {
                EXPECT_EQ( arr[i], 1 ) << "i=" << i;
                if ( ++nError > 10 )
                    ASSERT_EQ( arr[i], 1 );
            }
        }
    };

    CDSSTRESS_TreiberStack( stack_push )
    CDSSTRESS_EliminationStack( stack_push )
    CDSSTRESS_FCStack( stack_push )
    CDSSTRESS_FCDeque( stack_push )
    CDSSTRESS_StdStack( stack_push )

} // namespace
