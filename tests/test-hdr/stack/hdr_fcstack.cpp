//$$CDS-header$$

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

