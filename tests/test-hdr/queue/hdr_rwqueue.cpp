//$$CDS-header$$

#include <cds/container/rwqueue.h>

#include "queue/queue_test_header.h"

namespace queue {
    void Queue_TestHeader::RWQueue_()
    {
        testNoItemCounter<
            cds::container::RWQueue<
                int
                ,cds::opt::lock_type< cds::SpinLock >
            >
        >();
    }

    void Queue_TestHeader::RWQueue_Counted()
    {
        testWithItemCounter<
            cds::container::RWQueue<
            int
            ,cds::opt::lock_type< cds::SpinLock >
            ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        >();
    }
}
