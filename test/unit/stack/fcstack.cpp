// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/container/fcstack.h>

#include <vector>
#include <list>

namespace {
    class FCStack : public ::testing::Test
    {
    protected:
        template <class Stack>
        void test()
        {
            typedef typename Stack::value_type  value_type;
            Stack stack;
            value_type v;

            ASSERT_TRUE( stack.empty());
            ASSERT_EQ( stack.size(), 0u );

            ASSERT_TRUE( stack.push( 1 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_EQ( stack.size(), 1u );
            ASSERT_TRUE( stack.push( 2 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_EQ( stack.size(), 2u );
            ASSERT_TRUE( stack.push( 3 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_EQ( stack.size(), 3u );

            ASSERT_TRUE( stack.pop( v ));
            EXPECT_EQ( v, value_type( 3 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_EQ( stack.size(), 2u );
            ASSERT_TRUE( stack.pop( v ));
            EXPECT_EQ( v, value_type( 2 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_EQ( stack.size(), 1u );
            ASSERT_TRUE( stack.pop( v ));
            EXPECT_EQ( v, value_type( 1 ));
            ASSERT_TRUE( stack.empty());
            ASSERT_EQ( stack.size(), 0u );
            v = 1000;
            ASSERT_TRUE( !stack.pop( v ));
            EXPECT_EQ( v, value_type( 1000 ));
            ASSERT_TRUE( stack.empty());
            ASSERT_EQ( stack.size(), 0u );

            ASSERT_TRUE( stack.push( 10 ));
            ASSERT_TRUE( stack.push( 20 ));
            ASSERT_TRUE( stack.push( 30 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_EQ( stack.size(), 3u );

            while ( stack.pop( v ));

            ASSERT_TRUE( stack.empty());
            ASSERT_EQ( stack.size(), 0u );
        }
    };

    TEST_F( FCStack, default_stack )
    {
        typedef cds::container::FCStack< unsigned int > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_based )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>> stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_empty_wait_strategy )
    {
        struct stack_traits: public
            cds::container::fcstack::make_traits <
                cds::opt::wait_strategy<cds::algo::flat_combining::wait_strategy::empty>
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_single_mutex_single_condvar )
    {
        struct stack_traits: public
            cds::container::fcstack::make_traits <
            cds::opt::wait_strategy<cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_single_mutex_multi_condvar )
    {
        struct stack_traits: public
            cds::container::fcstack::make_traits <
            cds::opt::wait_strategy<cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_multi_mutex_multi_condvar )
    {
        struct stack_traits: public
            cds::container::fcstack::make_traits <
            cds::opt::wait_strategy<cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_single_mutex_single_condvar_2ms )
    {
        struct stack_traits: public
            cds::container::fcstack::make_traits <
            cds::opt::wait_strategy<cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<2>>
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_single_mutex_multi_condvar_2ms )
    {
        struct stack_traits: public
            cds::container::fcstack::make_traits <
            cds::opt::wait_strategy<cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<2>>
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_multi_mutex_multi_condvar_3ms )
    {
        struct stack_traits: public
            cds::container::fcstack::make_traits <
            cds::opt::wait_strategy<cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<3>>
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, deque_elimination )
    {
        struct stack_traits : public
            cds::container::fcstack::make_traits <
            cds::opt::enable_elimination < true >
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int>>, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, vector_based )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int>>> stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, vector_empty_wait_strategy )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, vector_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, vector_single_mutex_multi_condvar )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, vector_single_mutex_single_condvar )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, vector_elimination )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int>>,
            cds::container::fcstack::make_traits<
            cds::opt::enable_elimination< true >
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, list_based )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int>>> stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, list_empty_wait_strategy )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, list_single_mutex_single_condvar )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, list_single_mutex_multi_condvar )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, list_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( FCStack, list_elimination )
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int>>,
            cds::container::fcstack::make_traits<
                cds::opt::enable_elimination< true >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<2>>
            >::type
        > stack_type;
        test<stack_type>();
    }
} // namespace
