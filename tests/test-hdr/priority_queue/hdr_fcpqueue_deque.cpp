/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "priority_queue/hdr_pqueue.h"
#include <cds/container/fcpriority_queue.h>
#include <deque>

namespace priority_queue {

    void PQueueHdrTest::FCPQueue_deque()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,std::deque<PQueueHdrTest::value_type>
                ,PQueueHdrTest::less
            >
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

    void PQueueHdrTest::FCPQueue_deque_stat()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,std::deque<PQueueHdrTest::value_type>
                ,PQueueHdrTest::less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
            >::type
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

    void PQueueHdrTest::FCPQueue_deque_mutex()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,std::deque<PQueueHdrTest::value_type>
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

} // namespace priorty_queue
