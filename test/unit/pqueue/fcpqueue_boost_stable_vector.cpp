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
#include <boost/container/stable_vector.hpp>
#include <boost/container/deque.hpp>

namespace cds_test {

    TEST_F( FCPQueue, stable_vector )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::stable_vector<value_type>
                ,less
            >
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, stable_vector_stat )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::stable_vector<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, stable_vector_empty_wait_strategy )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::stable_vector<value_type>
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

    TEST_F( FCPQueue, stable_vector_single_mutex_single_condvar )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::stable_vector<value_type>
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

    TEST_F( FCPQueue, stable_vector_single_mutex_multi_condvar )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::stable_vector<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
                , cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::single_mutex_multi_condvar<4>>
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, boost_deque )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::deque<value_type>
                ,less
            >
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, boost_deque_stat )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::deque<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, boost_deque_empty_wait_strategy )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::deque<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
                ,cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::empty >
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

    TEST_F( FCPQueue, boost_deque_multi_mutex_multi_condvar )
    {
        typedef cds::container::FCPriorityQueue<
            value_type
            ,std::priority_queue<
                value_type
                ,boost::container::deque<value_type>
                ,less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
                ,cds::opt::wait_strategy< cds::algo::flat_combining::wait_strategy::multi_mutex_multi_condvar<6>>
            >::type
        > pqueue_type;

        pqueue_type pq;
        test( pq );
    }

} // namespace cds_test
