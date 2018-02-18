// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_fcpqueue.h"
#include <cds/container/fcpriority_queue.h>

namespace cds_test {

    TEST_F( FCPQueue, vector )
    {
        typedef cds::container::FCPriorityQueue< FCPQueue::value_type > pqueue_type;
        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, vector_empty_wait_strategy )
    {
        struct pqueue_traits : public cds::container::fcpqueue::traits
        {
            typedef cds::container::fcpqueue::stat<> stat;
            typedef cds::algo::flat_combining::wait_strategy::empty wait_strategy;
        };

        typedef cds::container::FCPriorityQueue<
            value_type
            , std::priority_queue<
                value_type
                , std::vector<value_type>
                , less
            >
            , pqueue_traits
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, vector_multi_mutex_multi_condvar )
    {
        struct pqueue_traits : public cds::container::fcpqueue::traits
        {
            typedef cds::container::fcpqueue::stat<> stat;
            typedef cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<> wait_strategy;
        };

        typedef cds::container::FCPriorityQueue<
            value_type
            , std::priority_queue<
                value_type
                , std::vector<value_type>
                , less
            >
            , pqueue_traits
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, vector_stat )
    {
        struct pqueue_traits : public cds::container::fcpqueue::traits
        {
            typedef cds::container::fcpqueue::stat<> stat;
        };

        typedef cds::container::FCPriorityQueue<
            value_type
            , std::priority_queue<
                value_type
                , std::vector<value_type>
                , less
            >
            , pqueue_traits
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, vector_stat_single_mutex_multi_condvar )
    {
        struct pqueue_traits : public cds::container::fcpqueue::traits
        {
            typedef cds::container::fcpqueue::stat<> stat;
            typedef cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<42> wait_strategy;
        };

        typedef cds::container::FCPriorityQueue<
            value_type
            , std::priority_queue<
                value_type
                , std::vector<value_type>
                , less
            >
            , pqueue_traits
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, vector_mutex )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue< value_type >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, vector_single_mutex_single_condvar )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue< value_type >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::lock_type< std::mutex >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_single_condvar<1000>>
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

} // namespace cds_test
