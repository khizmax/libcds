// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/container/fcdeque.h>
#include <boost/container/deque.hpp>

namespace {

    class FCDeque: public ::testing::Test
    {
    protected:
        template <class Deque>
        void test( Deque& dq )
        {
            size_t const c_nSize = 100;
            int total_sum = 0;

            // push_front/pop_front
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i ) {
                EXPECT_TRUE( dq.push_front( i ));
                total_sum += i;
            }
            EXPECT_EQ( dq.size(), c_nSize );

            int sum = 0;
            dq.apply( [&sum]( typename Deque::deque_type const& d )
                {
                    for ( auto const& el : d )
                        sum += el;
                }
            );
            EXPECT_EQ( sum, total_sum );

            size_t nCount = 0;
            int val;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.pop_front( val ));
                ++nCount;
                EXPECT_EQ( static_cast<int>(c_nSize - nCount), val );
            }
            EXPECT_EQ( nCount, c_nSize );

            // push_back/pop_back
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.push_back( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            nCount = 0;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.pop_back( val ));
                ++nCount;
                EXPECT_EQ( static_cast<int>(c_nSize - nCount), val );
            }
            EXPECT_EQ( nCount, c_nSize );

            // push_back/pop_front
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.push_back( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            nCount = 0;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.pop_front( val ));
                EXPECT_EQ( static_cast<int>( nCount ), val );
                ++nCount;
            }
            EXPECT_EQ( nCount, c_nSize );

            // push_front/pop_back
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.push_front( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            nCount = 0;
            while ( !dq.empty()) {
                EXPECT_TRUE( dq.pop_back( val ));
                EXPECT_EQ( static_cast<int>( nCount ), val );
                ++nCount;
            }
            EXPECT_EQ( nCount, c_nSize );

            // clear
            for ( int i = 0; i < static_cast<int>( c_nSize ); ++i )
                EXPECT_TRUE( dq.push_front( i ));
            EXPECT_EQ( dq.size(), c_nSize );

            EXPECT_FALSE( dq.empty());
            dq.clear();
            EXPECT_TRUE( dq.empty());
        }
    };

    TEST_F( FCDeque, std )
    {
        typedef cds::container::FCDeque<int> deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, std_empty_wait_strategy )
    {
        typedef cds::container::FCDeque<int, std::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, std_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCDeque<int, std::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, std_elimination )
    {
        typedef cds::container::FCDeque<int, std::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, std_elimination_single_mutex_single_condvar )
    {
        typedef cds::container::FCDeque<int, std::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<3>>
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, std_statistics )
    {
        typedef cds::container::FCDeque<int, std::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::stat< cds::container::fcdeque::stat<> >
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, std_stat_single_mutex_multi_condvar )
    {
        typedef cds::container::FCDeque<int, std::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::stat< cds::container::fcdeque::stat<> >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<2>>
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, std_mutex )
    {
        struct deque_traits : public
            cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        {
            typedef std::mutex lock_type;
        };
        typedef cds::container::FCDeque<int, std::deque<int>, deque_traits > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int> > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost_empty_wait_strategy )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost_single_mutex_single_condvar )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost_elimination )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost_elimination_single_mutex_multi_condvar )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<5>>
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost_statistics )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::stat< cds::container::fcdeque::stat<> >
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost_mutex )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::lock_type< std::mutex >
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

    TEST_F( FCDeque, boost_mutex_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCDeque<int, boost::container::deque<int>,
            cds::container::fcdeque::make_traits<
                cds::opt::enable_elimination< true >
                , cds::opt::lock_type< std::mutex >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        > deque_type;

        deque_type dq;
        test( dq );
    }

} // namespace
