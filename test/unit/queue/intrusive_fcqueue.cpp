// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/intrusive/fcqueue.h>

#include <vector>
#include <boost/intrusive/slist.hpp>

namespace {

    class IntrusiveFCQueue : public ::testing::Test
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

    public:
        struct disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount;
            }
        };

    protected:
        template <typename Queue>
        void test( Queue& q )
        {
            typedef typename Queue::value_type value_type;
            typedef typename Queue::container_type underlying_queue_type;

            size_t const nSize = 100;
            value_type * pv;
            std::vector<value_type> arr;
            arr.resize( nSize );
            int total_sum = 0;
            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].nVal = static_cast<int>( i );
                total_sum += static_cast<int>( i );
            }

            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // pop from empty queue
            pv = q.pop();
            ASSERT_TRUE( pv == nullptr );
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            pv = q.dequeue();
            ASSERT_TRUE( pv == nullptr );
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // push/pop test
            for ( size_t i = 0; i < nSize; ++i ) {
                if ( i & 1 )
                    q.push( arr[i] );
                else
                    q.enqueue( arr[i] );
                ASSERT_FALSE( q.empty());
                ASSERT_EQ( q.size(), i + 1 );
            }

            int sum = 0;
            q.apply( [&sum]( underlying_queue_type& queue )
            {
                for ( auto const& el : queue )
                    sum += el.nVal;
            } );
            EXPECT_EQ( sum, total_sum );


            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_FALSE( q.empty());
                ASSERT_EQ( q.size(), nSize - i );
                if ( i & 1 )
                    pv = q.pop();
                else
                    pv = q.dequeue();
                ASSERT_FALSE( pv == nullptr );
                ASSERT_EQ( pv->nVal, static_cast<int>(i));
            }
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // pop() doesn't call disposer
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 0 );
            }

            // clear with disposer
            for ( size_t i = 0; i < nSize; ++i )
                q.push( arr[i] );

            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), nSize );

            q.clear( true );
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 1 );
            }

            // clear without disposer
            for ( size_t i = 0; i < nSize; ++i )
                q.push( arr[i] );

            q.clear();
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 1 );
            }
        }
    };

    TEST_F( IntrusiveFCQueue, base )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, base_empty_wait_strategy )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct traits: public cds::intrusive::fcqueue::traits
        {
            typedef IntrusiveFCQueue::disposer disposer;
            typedef cds::algo::flat_combining::wait_strategy::empty wait_strategy;
            typedef cds::intrusive::fcqueue::stat<> stat;
        };
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, base_single_mutex_single_condvar )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct traits: public cds::intrusive::fcqueue::traits
        {
            typedef IntrusiveFCQueue::disposer disposer;
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<> wait_strategy;
            typedef cds::intrusive::fcqueue::stat<> stat;
        };
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, base_mutex )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct traits : public cds::intrusive::fcqueue::traits
        {
            typedef IntrusiveFCQueue::disposer disposer;
            typedef std::mutex lock_type;
            typedef cds::intrusive::fcqueue::stat<> stat;
        };
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, base_mutex_single_mutex_multi_condvar )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct traits: public cds::intrusive::fcqueue::traits
        {
            typedef IntrusiveFCQueue::disposer disposer;
            typedef std::mutex lock_type;
            typedef cds::intrusive::fcqueue::stat<> stat;
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<> wait_strategy;
        };
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, base_elimination )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct traits : public
            cds::intrusive::fcqueue::make_traits <
                cds::intrusive::opt::disposer< disposer >
                , cds::opt::enable_elimination < true >
            > ::type
        {};
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, base_elimination_multi_mutex_multi_condvar )
    {
        typedef base_hook_item< boost::intrusive::list_base_hook<> > value_type;
        struct traits: public
            cds::intrusive::fcqueue::make_traits <
                cds::intrusive::opt::disposer< disposer >
                , cds::opt::enable_elimination < true >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<>>
            > ::type
        {};
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type >, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, member )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, member_mutex )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        struct traits : public cds::intrusive::fcqueue::traits
        {
            typedef IntrusiveFCQueue::disposer disposer;
            typedef std::mutex lock_type;
            typedef cds::intrusive::fcqueue::stat<> stat;
        };
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, member_empty_wait_strategy )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, member_single_mutex_single_condvar )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<2>>
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, member_multi_mutex_multi_condvar )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, member_elimination )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::enable_elimination< true >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, member_elimination_single_mutex_multi_condvar )
    {
        typedef member_hook_item< boost::intrusive::list_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::list_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::list< value_type, member_option >,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::enable_elimination< true >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<2>>
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, slist_base )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<>> value_type;
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::slist< value_type, boost::intrusive::cache_last< true >>,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, slist_base_elimination )
    {
        typedef base_hook_item< boost::intrusive::slist_base_hook<> > value_type;
        struct traits : public
            cds::intrusive::fcqueue::make_traits <
                cds::intrusive::opt::disposer< disposer >
                , cds::opt::enable_elimination < true >
                , cds::opt::lock_type< std::mutex >
            > ::type
        {};
        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::slist< value_type, boost::intrusive::cache_last< true >>, traits > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, slist_member )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::slist< value_type, member_option, boost::intrusive::cache_last< true >>,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( IntrusiveFCQueue, slist_member_elimination )
    {
        typedef member_hook_item< boost::intrusive::slist_member_hook<> > value_type;
        typedef boost::intrusive::member_hook<value_type, boost::intrusive::slist_member_hook<>, &value_type::hMember> member_option;

        typedef cds::intrusive::FCQueue< value_type, boost::intrusive::slist< value_type, member_option, boost::intrusive::cache_last< true >>,
            cds::intrusive::fcqueue::make_traits<
                cds::intrusive::opt::disposer< disposer >
                ,cds::opt::enable_elimination< true >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

} // namepace
