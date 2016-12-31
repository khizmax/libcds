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

#include <gtest/gtest.h>
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
