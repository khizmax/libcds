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
