/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#include "cppunit/cppunit_proxy.h"
#include <cds/container/fcstack.h>

#include <vector>
#include <list>

namespace stack {

    class TestFCStack: public CppUnitMini::TestCase
    {
        template <class Stack>
        void test()
        {
            Stack s;
            test_with( s );
        }

        template <class Stack>
        void test_with( Stack& stack)
        {
            typedef typename Stack::value_type  value_type;
            value_type v;

            CPPUNIT_ASSERT( stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 0 );

            CPPUNIT_ASSERT( stack.push(1));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 1 );
            CPPUNIT_ASSERT( stack.push(2));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 2 );
            CPPUNIT_ASSERT( stack.push(3));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 3 );

            CPPUNIT_ASSERT( stack.pop(v) );
            CPPUNIT_ASSERT( v == 3 );
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 2 );
            CPPUNIT_ASSERT( stack.pop(v) );
            CPPUNIT_ASSERT( v == 2 );
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 1 );
            CPPUNIT_ASSERT( stack.pop(v) );
            CPPUNIT_ASSERT( v == 1 );
            CPPUNIT_ASSERT( stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 0 );
            v = 1000;
            CPPUNIT_ASSERT( !stack.pop(v) );
            CPPUNIT_ASSERT( v == 1000 );
            CPPUNIT_ASSERT( stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 0 );

            CPPUNIT_ASSERT( stack.push(10));
            CPPUNIT_ASSERT( stack.push(20));
            CPPUNIT_ASSERT( stack.push(30));
            CPPUNIT_ASSERT( !stack.empty());
            CPPUNIT_ASSERT( stack.size() == 3 );

            while ( stack.pop(v) );

            CPPUNIT_ASSERT( stack.empty() );
            CPPUNIT_ASSERT( stack.size() == 0 );
        }

        void FCStack_default();
        void FCStack_deque();
        void FCStack_deque_elimination();
        void FCStack_vector();
        void FCStack_vector_elimination();
        void FCStack_list();
        void FCStack_list_elimination();

        CPPUNIT_TEST_SUITE(TestFCStack);
            CPPUNIT_TEST( FCStack_default )
            CPPUNIT_TEST( FCStack_deque )
            CPPUNIT_TEST( FCStack_deque_elimination )
            CPPUNIT_TEST( FCStack_vector )
            CPPUNIT_TEST( FCStack_vector_elimination )
            CPPUNIT_TEST( FCStack_list )
            CPPUNIT_TEST( FCStack_list_elimination )
        CPPUNIT_TEST_SUITE_END();
    };

    void TestFCStack::FCStack_default()
    {
        typedef cds::container::FCStack< unsigned int > stack_type;
        test<stack_type>();
    }

    void TestFCStack::FCStack_deque()
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int> > > stack_type;
        test<stack_type>();
    }

    void TestFCStack::FCStack_deque_elimination()
    {
        struct stack_traits : public
            cds::container::fcstack::make_traits <
            cds::opt::enable_elimination < true >
            > ::type
        {};
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::deque<unsigned int> >, stack_traits > stack_type;
        test<stack_type>();
    }

    void TestFCStack::FCStack_vector()
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int> > > stack_type;
        test<stack_type>();
    }

    void TestFCStack::FCStack_vector_elimination()
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::vector<unsigned int> >,
            cds::container::fcstack::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > stack_type;
        test<stack_type>();
    }

    void TestFCStack::FCStack_list()
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int> > > stack_type;
        test<stack_type>();
    }

    void TestFCStack::FCStack_list_elimination()
    {
        typedef cds::container::FCStack< unsigned int, std::stack<unsigned int, std::list<unsigned int> >,
            cds::container::fcstack::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > stack_type;
        test<stack_type>();
    }

    CPPUNIT_TEST_SUITE_REGISTRATION(stack::TestFCStack);
}   // namespace stack

