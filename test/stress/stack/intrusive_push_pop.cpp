// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "intrusive_stack_push_pop.h"

namespace cds_test {
    /*static*/ size_t intrusive_stack_push_pop::s_nPushThreadCount = 4;
    /*static*/ size_t intrusive_stack_push_pop::s_nPopThreadCount = 4;
    /*static*/ size_t intrusive_stack_push_pop::s_nStackSize = 10000000;
    /*static*/ size_t intrusive_stack_push_pop::s_nEliminationSize = 4;
    /*static*/ bool intrusive_stack_push_pop::s_bFCIterative = false;
    /*static*/ unsigned int intrusive_stack_push_pop::s_nFCCombinePassCount = 64;
    /*static*/ unsigned int intrusive_stack_push_pop::s_nFCCompactFactor = 1024;

    /*static*/ atomics::atomic<size_t> intrusive_stack_push_pop::s_nWorkingProducers( 0 );

    /*static*/ void intrusive_stack_push_pop::SetUpTestCase()
    {
        cds_test::config const& cfg = get_config( "IntrusiveStack_PushPop" );

        s_nPushThreadCount = cfg.get_size_t( "PushThreadCount", s_nPushThreadCount );
        s_nPopThreadCount = cfg.get_size_t( "PopThreadCount", s_nPopThreadCount );
        s_nStackSize = cfg.get_size_t( "StackSize", s_nStackSize );
        s_nEliminationSize = cfg.get_size_t( "EliminationSize", s_nEliminationSize );
        s_bFCIterative = cfg.get_bool( "FCIterate", s_bFCIterative );
        s_nFCCombinePassCount = cfg.get_uint( "FCCombinePassCount", s_nFCCombinePassCount );
        s_nFCCompactFactor = cfg.get_uint( "FCCompactFactor", s_nFCCompactFactor );

        if ( s_nPushThreadCount == 0 )
            s_nPushThreadCount = 1;
        if ( s_nPopThreadCount == 0 )
            s_nPopThreadCount = 1;
        if ( s_nEliminationSize == 0 )
            s_nEliminationSize = 1;
    }
} // namespace cds_test

namespace {
    class intrusive_stack_push_pop : public cds_test::intrusive_stack_push_pop
    {
        typedef cds_test::intrusive_stack_push_pop base_class;
    protected:
        typedef base_class::value_type<> value_type;
        typedef base_class::value_type< cds::intrusive::treiber_stack::node< cds::gc::HP >> hp_value_type;
        typedef base_class::value_type< cds::intrusive::treiber_stack::node< cds::gc::DHP >> dhp_value_type;

        template <typename Stack>
        void test()
        {
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            {
                Stack stack;
                do_test( stack, arrValue );
            }
            Stack::gc::force_dispose();
        }

        void check_elimination_stat( cds::intrusive::treiber_stack::empty_stat const& )
        {}

        void check_elimination_stat( cds::intrusive::treiber_stack::stat<> const& s )
        {
            EXPECT_EQ( s.m_PushCount.get() + s.m_ActivePushCollision.get() + s.m_PassivePushCollision.get(), s_nStackSize );
            EXPECT_EQ( s.m_PopCount.get() + s.m_ActivePopCollision.get() + s.m_PassivePopCollision.get(), s_nStackSize );
            EXPECT_EQ( s.m_PushCount.get(), s.m_PopCount.get());
            EXPECT_EQ( s.m_ActivePopCollision.get(), s.m_PassivePushCollision.get());
            EXPECT_EQ( s.m_ActivePushCollision.get(), s.m_PassivePopCollision.get());
        }

        template <typename Stack>
        void test_elimination()
        {
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            {
                Stack stack( s_nEliminationSize );
                do_test( stack, arrValue );
                check_elimination_stat( stack.statistics());
            }
            Stack::gc::force_dispose();
        }

        template <typename Stack>
        void test_std()
        {
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            Stack stack;
            do_test( stack, arrValue );
        }
    };

    // TreiberStack<cds::gc::HP>
#define CDSSTRESS_Stack_F( test_fixture, stack_impl ) \
    TEST_F( test_fixture, stack_impl ) \
    { \
        typedef typename istack::Types<hp_value_type>::stack_impl stack_type; \
        test< stack_type >(); \
    }

    CDSSTRESS_TreiberStack_HP( intrusive_stack_push_pop )

#undef CDSSTRESS_Stack_F

    // TreiberStack<cds::gc::DHP>
#define CDSSTRESS_Stack_F( test_fixture, stack_impl ) \
    TEST_F( test_fixture, stack_impl ) \
    { \
        typedef typename istack::Types<dhp_value_type>::stack_impl stack_type; \
        test< stack_type >(); \
    }

    CDSSTRESS_TreiberStack_DHP( intrusive_stack_push_pop )

#undef CDSSTRESS_Stack_F

    // TreiberStack<cds::gc::HP> + elimination enabled
#define CDSSTRESS_Stack_F( test_fixture, stack_impl ) \
    TEST_F( test_fixture, stack_impl ) \
    { \
        typedef typename istack::Types<hp_value_type>::stack_impl stack_type; \
        test_elimination< stack_type >(); \
    }

    CDSSTRESS_EliminationStack_HP( intrusive_stack_push_pop )

#undef CDSSTRESS_Stack_F


    // TreiberStack<cds::gc::DHP> + elimination enabled
#define CDSSTRESS_Stack_F( test_fixture, stack_impl ) \
    TEST_F( test_fixture, stack_impl ) \
    { \
        typedef typename istack::Types<dhp_value_type>::stack_impl stack_type; \
        test_elimination< stack_type >(); \
    }

    CDSSTRESS_EliminationStack_DHP( intrusive_stack_push_pop )

#undef CDSSTRESS_Stack_F


    // StdStack
#define CDSSTRESS_Stack_F( test_fixture, stack_impl ) \
    TEST_F( test_fixture, stack_impl ) \
    { \
        typedef typename istack::Types<value_type>::stack_impl stack_type; \
        test_std< stack_type >(); \
    }

    CDSSTRESS_StdStack( intrusive_stack_push_pop )

#undef CDSSTRESS_Stack_F

    //INSTANTIATE_TEST_CASE_P( a, intrusive_stack_push_pop, ::testing::Values(1));

} // namespace
