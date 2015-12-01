//$$CDS-header$$

#include <cds/container/williams_queue.h>
#include <mutex>

#include "queue/hdr_queue.h"

namespace queue {
    void HdrTestQueue::WilliamsQueue_default()
    {
        test_no_ic< cds::container::WilliamsQueue< int > >();
    }

    void HdrTestQueue::WilliamsQueue_ic()
    {
        struct queue_traits : public cds::container::williams_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };

        test_ic< cds::container::WilliamsQueue< int, queue_traits > >();
    }
}
