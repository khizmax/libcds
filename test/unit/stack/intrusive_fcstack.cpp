// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/intrusive/fcstack.h>

#include <boost/intrusive/list.hpp>

namespace {
    class IntrusiveFCStack : public ::testing::Test
    {
    protected:
        template <typename Hook>
        struct base_hook_item : public Hook
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount( 0 )
            {}
        };

        template <typename Hook>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            Hook hMember;

            member_hook_item()
                : nDisposeCount( 0 )
            {}
        };

        struct mock_disposer
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
            typedef typename Stack::value_type  value_type;
            typedef typename Stack::container_type underlying_stack_type;
            Stack stack;

            ASSERT_TRUE( stack.empty());

            value_type v1, v2, v3;
            v1.nVal = 1;
            v2.nVal = 2;
            v3.nVal = 3;
            ASSERT_TRUE( stack.push( v1 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.push( v2 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.push( v3 ));
            ASSERT_TRUE( !stack.empty());

            unsigned sum = 0;
            stack.apply( [&sum]( underlying_stack_type& stack )
            {
                for ( auto const& el : stack )
                    sum += el.nVal;
            } );
            EXPECT_EQ( sum, 6 );

            value_type * pv;
            pv = stack.pop();
            ASSERT_TRUE( pv != nullptr );
            ASSERT_EQ( pv->nVal, 3 );
            ASSERT_TRUE( !stack.empty());
            pv = stack.pop();
            ASSERT_TRUE( pv != nullptr );
            ASSERT_EQ( pv->nVal, 2 );
            ASSERT_TRUE( !stack.empty());
            pv = stack.pop();
            ASSERT_TRUE( pv != nullptr );
            ASSERT_EQ( pv->nVal, 1 );
            ASSERT_TRUE( stack.empty());
            pv = stack.pop();
            ASSERT_TRUE( pv == nullptr );
            ASSERT_TRUE( stack.empty());

            if ( !std::is_same<typename Stack::disposer, cds::intrusive::opt::v::empty_disposer>::value ) {
                int v1disp = v1.nDisposeCount;
                int v2disp = v2.nDisposeCount;
                int v3disp = v3.nDisposeCount;

                ASSERT_TRUE( stack.push( v1 ));
                ASSERT_TRUE( stack.push( v2 ));
                ASSERT_TRUE( stack.push( v3 ));

                stack.clear();
                ASSERT_TRUE( stack.empty());

                EXPECT_EQ( v1.nDisposeCount, v1disp);
                EXPECT_EQ( v2.nDisposeCount, v2disp);
                EXPECT_EQ( v3.nDisposeCount, v3disp);

                ASSERT_TRUE( stack.push( v1 ));
                ASSERT_TRUE( stack.push( v2 ));
                ASSERT_TRUE( stack.push( v3 ));
                ASSERT_TRUE( !stack.empty());

                stack.clear( true );
                ASSERT_TRUE( stack.empty());

                EXPECT_EQ( v1.nDisposeCount, v1disp + 1 );
                EXPECT_EQ( v2.nDisposeCount, v2disp + 1 );
                EXPECT_EQ( v3.nDisposeCount, v3disp + 1 );
            }
        }
    };

    TEST_F( IntrusiveFCStack, slist )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type > > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_empty_wait_strategy )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::empty wait_strategy;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_single_mutex_single_condvar )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<> wait_strategy;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_single_mutex_multi_condvar )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<> wait_strategy;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_multi_mutex_multi_condvar )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<> wait_strategy;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_single_mutex_single_condvar_2ms )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<2> wait_strategy;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_single_mutex_multi_condvar_3ms )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<3> wait_strategy;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_multi_mutex_multi_condvar_2ms )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<2> wait_strategy;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_disposer )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits : public cds::intrusive::fcstack::traits
        {
            typedef mock_disposer disposer;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_mutex )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits : public cds::intrusive::fcstack::traits
        {
            typedef std::mutex lock_type;
        };
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_elimination )
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

    TEST_F( IntrusiveFCStack, slist_elimination_disposer )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct stack_traits : public
            cds::intrusive::fcstack::make_traits <
                cds::opt::enable_elimination < true >,
                cds::intrusive::opt::disposer< mock_disposer >,
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            > ::type
        {};
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_elimination_stat )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type >,
            cds::intrusive::fcstack::make_traits<
                cds::opt::enable_elimination< true >
                , cds::opt::stat< cds::intrusive::fcstack::stat<> >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_member )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option > > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_member_empty_wait_strategy )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::empty wait_strategy;
        };

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_member_single_mutex_single_condvar )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<> wait_strategy;
        };

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_member_single_mutex_multi_condvar )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<> wait_strategy;
        };

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_member_multi_mutex_multi_condvar )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;
        struct stack_traits: public cds::intrusive::fcstack::traits
        {
            typedef cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<> wait_strategy;
        };

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_member_disposer )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;
        struct stack_traits : public cds::intrusive::fcstack::traits
        {
            typedef mock_disposer disposer;
        };

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >, stack_traits > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, slist_member_elimination )
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

    TEST_F( IntrusiveFCStack, slist_member_elimination_stat )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::slist< value_type, member_option >,
            cds::intrusive::fcstack::make_traits<
            cds::opt::enable_elimination< true >
            , cds::opt::stat< cds::intrusive::fcstack::stat<> >
            , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, list )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type > > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, list_mutex )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcstack::make_traits<
            cds::opt::lock_type< std::mutex >
            >::type
        > stack_type;
        test<stack_type>();
    }


    TEST_F( IntrusiveFCStack, list_elimination )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcstack::make_traits<
            cds::opt::enable_elimination< true >
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, list_elimination_stat )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcstack::make_traits<
            cds::opt::enable_elimination< true >
            , cds::opt::stat< cds::intrusive::fcstack::stat<> >
            , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, list_member )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type, member_option > > stack_type;
        test<stack_type>();
    }

    TEST_F( IntrusiveFCStack, list_member_elimination )
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

    TEST_F( IntrusiveFCStack, list_member_elimination_stat )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCStack< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcstack::make_traits<
            cds::opt::enable_elimination< true >
            , cds::opt::stat< cds::intrusive::fcstack::stat<> >
            , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        > stack_type;
        test<stack_type>();
    }

} // namespace
