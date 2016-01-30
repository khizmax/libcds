#include <cds/container/cache_aware_queue.h>
#include "queue/hdr_queue.h"

namespace queue {
    void HdrTestQueue::CacheAwareQueue_default()
    {
        test_no_ic< cds::container::CAQueue< int > >();
    }

    void HdrTestQueue::CacheAwareQueue_ic()
    {
        struct queue_traits : public cds::container::cache_aware_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };

        test_ic< cds::container::CAQueue< int, queue_traits > >();
    }
}
