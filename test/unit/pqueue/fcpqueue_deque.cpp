// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_fcpqueue.h"
#include <cds/container/fcpriority_queue.h>
#include <deque>

namespace cds_test {

    TEST_F( FCPQueue, deque )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,std::deque<value_type>
                ,less
            >
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, deque_stat )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,std::deque<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, deque_stat_single_mutex_single_condvar )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,std::deque<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<>>
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, deque_empty_wait_strategy )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,std::deque<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, deque_single_mutex_multi_condvar )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,std::deque<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<2>>
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, deque_mutex )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,std::deque<value_type>
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, deque_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,std::deque<value_type>
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::lock_type< std::mutex >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<1000>>
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

} // namespace cds_test
