/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include <cds/container/rwqueue.h>
#include <mutex>

#include "queue/hdr_queue.h"

namespace queue {
    void HdrTestQueue::RWQueue_default()
    {
        test_no_ic< cds::container::RWQueue< int > >();
    }

    void HdrTestQueue::RWQueue_mutex()
    {
        struct queue_traits : public cds::container::rwqueue::traits
        {
            typedef std::mutex lock_type;
        };

        test_no_ic< cds::container::RWQueue< int, queue_traits > >();
    }

    void HdrTestQueue::RWQueue_ic()
    {
        struct queue_traits : public cds::container::rwqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };

        test_ic< cds::container::RWQueue< int, queue_traits > >();
    }

    void HdrTestQueue::RWQueue_ic_mutex()
    {
        struct queue_traits : public cds::container::rwqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef std::mutex lock_type;
        };

        test_ic< cds::container::RWQueue< int, queue_traits > >();
    }

}
