/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include <cds/container/fcqueue.h>
#include "queue/hdr_fcqueue.h"

#include <list>

namespace queue {

    void HdrFCQueue::FCQueue_deque()
    {
        typedef cds::container::FCQueue<int> queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_deque_elimination()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_deque_mutex()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_deque_stat()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::stat< cds::container::fcqueue::stat<> >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    //
    void HdrFCQueue::FCQueue_list()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> > > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_list_elimination()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_list_mutex()
    {
        typedef cds::container::FCQueue<int, std::queue<int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_list_stat()
    {
        struct queue_traits : public cds::container::fcqueue::traits
        {
            typedef cds::container::fcqueue::stat<> stat;
        };
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >, queue_traits > queue_type;
        testFCQueue<queue_type>();
    }

} // namespace queue
