// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "intrusive_stack_push_pop.h"

namespace {

    struct fc_param
    {
        unsigned int nCompactFactor;
        unsigned int nCombinePassCount;
    };

    class intrusive_fcstack_push_pop
        : public cds_test::intrusive_stack_push_pop
        , public ::testing::WithParamInterface< fc_param >
    {
        typedef cds_test::intrusive_stack_push_pop base_class;

    public:

        static std::vector< fc_param > get_test_parameters()
        {
            cds_test::config const& cfg = cds_test::stress_fixture::get_config( "IntrusiveStack_PushPop" );
            bool bFCIterative = cfg.get_bool( "FCIterate", s_bFCIterative );
            unsigned int nFCCombinePassCount = cfg.get_uint( "FCCombinePassCount", s_nFCCombinePassCount );
            unsigned int nFCCompactFactor = cfg.get_uint( "FCCompactFactor", s_nFCCompactFactor );

            std::vector< fc_param > args;
            if ( bFCIterative ) {
                for ( unsigned int nCompactFactor = 1; nCompactFactor <= nFCCompactFactor; nCompactFactor *= 2 ) {
                    for ( unsigned int nPass = 1; nPass <= nFCCombinePassCount; nPass *= 2 )
                        args.push_back( { nCompactFactor, nPass } );
                }
            }

            if ( args.empty()) {
                if ( nFCCompactFactor && nFCCombinePassCount )
                    args.push_back( { nFCCompactFactor, nFCCombinePassCount } );
                else
                    args.push_back( { 0, 0 } );
            }

            return args;
        }

#ifdef CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG
        static std::string get_test_parameter_name( testing::TestParamInfo<fc_param> const& p )
        {
            if ( p.param.nCombinePassCount ) {
                std::stringstream ss;
                ss << "compact_factor" << p.param.nCompactFactor
                   << "__combine_pass_count" << p.param.nCombinePassCount
;
                return ss.str();
            }
            else {
                return std::string( "with_defaults" );
            }
        }
#endif

    protected:
        typedef base_class::value_type<boost::intrusive::slist_base_hook<>> slist_value_type;
        typedef base_class::value_type<boost::intrusive::list_base_hook<>>  list_value_type;

        template <typename Stack>
        void test()
        {
            value_array<typename Stack::value_type> arrValue( s_nStackSize );
            if ( s_bFCIterative ) {
                fc_param arg = GetParam();
                if ( arg.nCombinePassCount ) {
                    propout()
                        << std::make_pair( "compact_factor", arg.nCompactFactor )
                        << std::make_pair( "combine_pass_count", arg.nCombinePassCount );
                    Stack stack( arg.nCompactFactor, arg.nCombinePassCount );
                    do_test( stack, arrValue );
                }
                else {
                    Stack stack;
                    do_test( stack, arrValue );
                }
            }
            else {
                fc_param arg = GetParam();
                if ( arg.nCombinePassCount ) {
                    propout()
                        << std::make_pair( "compact_factor", arg.nCompactFactor )
                        << std::make_pair( "combine_pass_count", arg.nCombinePassCount );
                    Stack stack( arg.nCompactFactor, arg.nCombinePassCount );
                    do_test( stack, arrValue );
                }
                else {
                    Stack stack;
                    do_test( stack, arrValue );
                }
            }
        }
    };

    // FCStack based on boost::intrusive::slist
#define CDSSTRESS_Stack_F( test_fixture, stack_impl ) \
    TEST_P( test_fixture, stack_impl ) \
    { \
        typedef typename istack::Types<slist_value_type>::stack_impl stack_type; \
        test< stack_type >(); \
    }

    CDSSTRESS_FCStack_slist( intrusive_fcstack_push_pop )

#undef CDSSTRESS_Stack_F

    // FCStack based on boost::intrusive::list
#define CDSSTRESS_Stack_F( test_fixture, stack_impl ) \
    TEST_P( test_fixture, stack_impl ) \
    { \
        typedef typename istack::Types<list_value_type>::stack_impl stack_type; \
        test< stack_type >(); \
    }

    CDSSTRESS_FCStack_list( intrusive_fcstack_push_pop )

#undef CDSSTRESS_Stack_F

} // namespace

#ifdef CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG
INSTANTIATE_TEST_CASE_P( FC,
    intrusive_fcstack_push_pop,
    ::testing::ValuesIn( intrusive_fcstack_push_pop::get_test_parameters()),
    intrusive_fcstack_push_pop::get_test_parameter_name );
#else
INSTANTIATE_TEST_CASE_P( FC,
    intrusive_fcstack_push_pop,
    ::testing::ValuesIn( intrusive_fcstack_push_pop::get_test_parameters()));
#endif

