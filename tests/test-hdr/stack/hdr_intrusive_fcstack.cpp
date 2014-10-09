//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/fcstack.h>

#include <boost/intrusive/list.hpp>

namespace stack {

    class TestIntrusiveFCStack: public CppUnitMini::TestCase
    {
    public:
        template <typename Hook>
        struct base_hook_item: public Hook
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        template <typename Hook>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            Hook hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        struct faked_disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount;
            }
        };

        template <class Stack>
        void test()
        {
            Stack stack;
            test_with(stack);
        }

        template <class Stack>
        void test_with( Stack& stack )
        {
            typedef typename Stack::value_type  value_type;

            CPPUNIT_ASSERT( stack.empty() );

            value_type v1, v2, v3;
            v1.nVal = 1;
            v2.nVal = 2;
            v3.nVal = 3;
            CPPUNIT_ASSERT( stack.push(v1));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.push(v2));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.push(v3));
            CPPUNIT_ASSERT( !stack.empty() );

            value_type * pv;
            pv = stack.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( !stack.empty() );
            pv = stack.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !stack.empty() );
            pv = stack.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( stack.empty() );
            pv = stack.pop();
            CPPUNIT_ASSERT( pv == nullptr );

            CPPUNIT_ASSERT( v1.nDisposeCount == 0 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 0 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 0 );

            stack.push(v1);
            stack.push(v2);
            stack.push(v3);

            stack.clear();
            CPPUNIT_ASSERT( stack.empty() );

            if ( !std::is_same<typename Stack::disposer, cds::intrusive::opt::v::empty_disposer>::value ) {
                CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v3.nDisposeCount == 1 );
            }
        }

        void FCStack_slist();
        void FCStack_slist_mutex();
        void FCStack_slist_elimination();
        void FCStack_slist_elimination_stat();
        void FCStack_slist_member();
        void FCStack_slist_member_elimination();
        void FCStack_slist_member_elimination_stat();
        void FCStack_list();
        void FCStack_list_mutex();
        void FCStack_list_elimination();
        void FCStack_list_elimination_stat();
        void FCStack_list_member();
        void FCStack_list_member_elimination();
        void FCStack_list_member_elimination_stat();

        CPPUNIT_TEST_SUITE(TestIntrusiveFCStack)
            CPPUNIT_TEST(FCStack_slist)
            CPPUNIT_TEST(FCStack_slist_mutex)
            CPPUNIT_TEST(FCStack_slist_elimination)
            CPPUNIT_TEST(FCStack_slist_elimination_stat)
            CPPUNIT_TEST(FCStack_slist_member)
            CPPUNIT_TEST(FCStack_slist_member_elimination)
            CPPUNIT_TEST(FCStack_slist_member_elimination_stat)
            CPPUNIT_TEST(FCStack_list)
            CPPUNIT_TEST(FCStack_list_mutex)
            CPPUNIT_TEST(FCStack_list_elimination)
            CPPUNIT_TEST(FCStack_list_elimination_stat)
            CPPUNIT_TEST(FCStack_list_member)
            CPPUNIT_TEST(FCStack_list_member_elimination)
            CPPUNIT_TEST(FCStack_list_member_elimination_stat)
        CPPUNIT_TEST_SUITE_END()
    };

    void TestIntrusiveFCStack::FCStack_slist()
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type > > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_slist_mutex()
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits : public cds::intrusive::fcstack::traits
        {
            typedef std::mutex lock_type;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_slist_elimination()
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits : public
            cds::intrusive::fcstack::make_traits <
                cds::opt::enable_elimination < true >
            > ::type
        {};
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_slist_elimination_stat()
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::stat< cds::intrusive::fcstack::stat<> >
            >::type
        > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_slist_member()
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option > > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_slist_member_elimination()
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_slist_member_elimination_stat()
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::stat< cds::intrusive::fcstack::stat<> >
            >::type
        > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_list()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type > > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_list_mutex()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > stack_type;
        test<stack_type>();
    }


    void TestIntrusiveFCStack::FCStack_list_elimination()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_list_elimination_stat()
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::stat< cds::intrusive::fcstack::stat<> >
            >::type
        > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_list_member()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type, member_option > > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_list_member_elimination()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > stack_type;
        test<stack_type>();
    }

    void TestIntrusiveFCStack::FCStack_list_member_elimination_stat()
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::stat< cds::intrusive::fcstack::stat<> >
            >::type
        > stack_type;
        test<stack_type>();
    }

    CPPUNIT_TEST_SUITE_REGISTRATION(stack::TestIntrusiveFCStack);
}   // namespace stack

