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

#include <gtest/gtest.h>
#include <cds/container/fcqueue.h>

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

            const size_t nSize = 100;

            ASSERT_TRUE( q.empty() );
            ASSERT_EQ( q.size(), 0 );

            // enqueue/dequeue
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.enqueue( static_cast<value_type>(i)));
                ASSERT_EQ( q.size(), i + 1 );
            }
            ASSERT_FALSE( q.empty() );
            ASSERT_EQ( q.size(), nSize );

            for ( size_t i = 0; i < nSize; ++i ) {
                it = -1;
                ASSERT_TRUE( q.dequeue( it ));
                ASSERT_EQ( it, i );
                ASSERT_EQ( q.size(), nSize - i - 1 );
            }
            ASSERT_TRUE( q.empty() );
            ASSERT_EQ( q.size(), 0 );

            // push/pop
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.push( static_cast<value_type>(i)));
                ASSERT_EQ( q.size(), i + 1 );
            }
            ASSERT_FALSE( q.empty() );
            ASSERT_EQ( q.size(), nSize );

            for ( size_t i = 0; i < nSize; ++i ) {
                it = -1;
                ASSERT_TRUE( q.pop( it ) );
                ASSERT_EQ( it, i );
                ASSERT_EQ( q.size(), nSize - i - 1 );
            }
            ASSERT_TRUE( q.empty() );
            ASSERT_EQ( q.size(), 0 );

            // clear
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.push( static_cast<value_type>( i )));
            }
            ASSERT_FALSE( q.empty() );
            ASSERT_EQ( q.size(), nSize );

            q.clear();
            ASSERT_TRUE( q.empty() );
            ASSERT_EQ( q.size(), 0 );

            // pop from empty queue
            it = nSize * 2;
            ASSERT_FALSE( q.pop( it ));
            ASSERT_EQ( it, nSize * 2 );
            ASSERT_TRUE( q.empty() );
            ASSERT_EQ( q.size(), 0 );

            ASSERT_FALSE( q.dequeue( it ) );
            ASSERT_EQ( it, nSize * 2 );
            ASSERT_TRUE( q.empty() );
            ASSERT_EQ( q.size(), 0 );
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
            ASSERT_FALSE( q.empty() );
            ASSERT_EQ( q.size(), nSize );

            for ( size_t i = 0; i < nSize; ++i ) {
                std::string s;
                ASSERT_TRUE( q.pop( s ));
                ASSERT_EQ( q.size(), nSize - i - 1 );
                ASSERT_EQ( s, str[i] );
            }
            ASSERT_TRUE( q.empty() );
            ASSERT_EQ( q.size(), 0 );
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
