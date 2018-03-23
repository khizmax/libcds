// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/container/fcqueue.h>
#include <test/include/cds_test/fc_hevy_value.h>

#include <list>

namespace {

    class FCQueue: public ::testing::Test
    {
    protected:
        template <class Queue>
        void test( Queue& q )
        {
            typedef typename Queue::value_type value_type;
            value_type it;

            const int nSize = 100;

            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // enqueue/dequeue
            for ( int i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.enqueue( value_type(i)));
                ASSERT_EQ( q.size(), static_cast<size_t>(i + 1));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), static_cast<size_t>( nSize ));

            for ( int i = 0; i < nSize; ++i ) {
                it = value_type( -1 );
                ASSERT_TRUE( q.dequeue( it ));
                ASSERT_EQ( it, value_type( i ));
                ASSERT_EQ( q.size(), static_cast<size_t>( nSize - i - 1 ));
            }
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // push/pop
            for ( int i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.push( value_type(i)));
                ASSERT_EQ( q.size(), static_cast<size_t>( i + 1 ));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), static_cast<size_t>( nSize ));

            for ( int i = 0; i < nSize; ++i ) {
                it = value_type( -1 );
                ASSERT_TRUE( q.pop( it ));
                ASSERT_EQ( it, value_type( i ));
                ASSERT_EQ( q.size(), static_cast<size_t>( nSize - i - 1 ));
            }
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // clear
            for ( int i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.push( value_type( i )));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), static_cast<size_t>( nSize ));

            q.clear();
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // pop from empty queue
            it = value_type( nSize * 2 );
            ASSERT_FALSE( q.pop( it ));
            ASSERT_EQ( it, static_cast<value_type>( nSize * 2 ));
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            ASSERT_FALSE( q.dequeue( it ));
            ASSERT_EQ( it, static_cast<value_type>( nSize * 2 ));
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );
        }

        template <class Queue>
        void test_string( Queue& q )
        {
            std::string str[3];
            str[0] = "one";
            str[1] = "two";
            str[2] = "three";
            const size_t nSize = sizeof( str ) / sizeof( str[0] );

            // move push
            for ( size_t i = 0; i < nSize; ++i ) {
                std::string s = str[i];
                ASSERT_FALSE( s.empty());
                ASSERT_TRUE( q.enqueue( std::move( s )));
                ASSERT_FALSE( s.empty());
                ASSERT_EQ( q.size(), i + 1 );
            }
            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), nSize );

            for ( size_t i = 0; i < nSize; ++i ) {
                std::string s;
                ASSERT_TRUE( q.pop( s ));
                ASSERT_EQ( q.size(), nSize - i - 1 );
                ASSERT_EQ( s, str[i] );
            }
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );
        }

        template <class Queue>
        void test_heavy( Queue& q )
        {
            typedef typename Queue::value_type value_type;
            value_type it;

            const int nSize = 100;

            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // enqueue/dequeue
            for ( int i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.enqueue( value_type( i )));
                ASSERT_EQ( q.size(), static_cast<size_t>( i + 1 ));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), static_cast<size_t>( nSize ));

            for ( int i = 0; i < nSize; ++i ) {
                it.value = -1;
                ASSERT_TRUE( q.dequeue( it ));
                ASSERT_EQ( it.value, i );
                ASSERT_EQ( q.size(), static_cast<size_t>( nSize - i - 1 ));
            }
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // push/pop
            for ( int i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.push( value_type( i )));
                ASSERT_EQ( q.size(), static_cast<size_t>( i + 1 ));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), static_cast<size_t>( nSize ));

            for ( int i = 0; i < nSize; ++i ) {
                it.value = -1;
                ASSERT_TRUE( q.pop( it ));
                ASSERT_EQ( it.value, i );
                ASSERT_EQ( q.size(), static_cast<size_t>( nSize - i - 1 ));
            }
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // clear
            for ( int i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.push( value_type( i )));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_EQ( q.size(), static_cast<size_t>( nSize ));

            q.clear();
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            // pop from empty queue
            it = value_type( nSize * 2 );
            ASSERT_FALSE( q.pop( it ));
            ASSERT_EQ( it.value, nSize * 2 );
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );

            ASSERT_FALSE( q.dequeue( it ));
            ASSERT_EQ( it.value, nSize * 2 );
            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );
        }

    };

    TEST_F( FCQueue, std_deque )
    {
        typedef cds::container::FCQueue<int> queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_deque_move )
    {
        typedef cds::container::FCQueue<std::string> queue_type;

        queue_type q;
        test_string( q );
    }

    TEST_F( FCQueue, std_empty_wait_strategy )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int>>,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_deque_heavy_value )
    {
        typedef fc_test::heavy_value<> ValueType;
        typedef cds::container::FCQueue<ValueType> queue_type;

        queue_type q;
        test_heavy( q );
    }

    TEST_F( FCQueue, std_empty_wait_strategy_heavy_value )
    {
        typedef fc_test::heavy_value<> ValueType;
        typedef cds::container::FCQueue<ValueType, std::queue< ValueType, std::deque<ValueType>>,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > queue_type;

        queue_type q;
        test_heavy( q );
    }

    TEST_F( FCQueue, std_single_mutex_single_condvar_heavy_value )
    {
        typedef fc_test::heavy_value<> ValueType;
        typedef cds::container::FCQueue<ValueType, std::queue< ValueType, std::deque<ValueType>>,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<> >
            >::type
        > queue_type;

        queue_type q;
        test_heavy( q );
    }

    TEST_F( FCQueue, std_single_mutex_multi_condvar_heavy_value )
    {
        typedef fc_test::heavy_value<> ValueType;
        typedef cds::container::FCQueue<ValueType, std::queue< ValueType, std::deque<ValueType>>,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<> >
            >::type
        > queue_type;

        queue_type q;
        test_heavy( q );
    }

    TEST_F( FCQueue, std_multi_mutex_multi_condvar_heavy_value )
    {
        typedef fc_test::heavy_value<> ValueType;
        typedef cds::container::FCQueue<ValueType, std::queue< ValueType, std::deque<ValueType>>,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<> >
            >::type
        > queue_type;

        queue_type q;
        test_heavy( q );
    }

    TEST_F( FCQueue, std_single_mutex_single_condvar )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int>>,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_deque_elimination )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int>>,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_deque_elimination_single_mutex_multi_condvar )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int>>,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<2>>
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_deque_elimination_move )
    {
        typedef cds::container::FCQueue<std::string, std::queue< std::string, std::deque<std::string>>,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > queue_type;

        queue_type q;
        test_string( q );
    }

    TEST_F( FCQueue, std_deque_elimination_move_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCQueue<std::string, std::queue< std::string, std::deque<std::string>>,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<>>
            >::type
        > queue_type;

        queue_type q;
        test_string( q );
    }

    TEST_F( FCQueue, std_deque_mutex )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int>>,
            cds::container::fcqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_list )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int>>> queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_list_move )
    {
        typedef cds::container::FCQueue<std::string, std::queue< std::string, std::list<std::string>>> queue_type;

        queue_type q;
        test_string( q );
    }

    TEST_F( FCQueue, std_list_empty_wait_strategy )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_list_single_mutex_single_condvar )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<5>>
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_list_elimination )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_list_elimination_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
                ,cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<5>>
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

    TEST_F( FCQueue, std_list_elimination_move )
    {
        typedef cds::container::FCQueue<std::string, std::queue< std::string, std::list<std::string> >,
            cds::container::fcqueue::make_traits<
            cds::opt::enable_elimination< true >
            >::type
        > queue_type;

        queue_type q;
        test_string( q );
    }

    TEST_F( FCQueue, std_list_mutex )
    {
        typedef cds::container::FCQueue<int, std::queue<int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        queue_type q;
        test( q );
    }

} // namespace
