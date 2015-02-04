//$$CDS-header$$

#include <cds/container/williams_queue_spsc.h>

#include "queue/hdr_queue.h"

namespace queue {

    void HdrTestQueue::WilliamsQueue_SPSC_integral()
    {
        typedef cds::container::WilliamsQueue< int > test_queue;
        test_no_ic< test_queue >();
    }

    void HdrTestQueue::WilliamsQueue_SPSC_wrapped()
    {
        typedef cds::container::WilliamsQueue< std::shared_ptr<int> > test_queue;
        test_no_ic< test_queue >();
    }
}   // namespace queue
