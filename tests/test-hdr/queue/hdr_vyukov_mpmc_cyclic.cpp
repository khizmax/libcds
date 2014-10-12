//$$CDS-header$$

#include <cds/container/vyukov_mpmc_cycle_queue.h>

#include "queue/hdr_queue_new.h"

namespace queue {
    namespace{
        struct traits_VyukovMPMCCyclicQueue_static : public cds::container::vyukov_queue::traits
        {
            typedef cds::opt::v::static_buffer<int, 1024> buffer;
        };
        struct traits_VyukovMPMCCyclicQueue_static_ic : public traits_VyukovMPMCCyclicQueue_static
        {
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void HdrTestQueue::VyukovMPMCCycleQueue_static()
    {
        typedef cds::container::VyukovMPMCCycleQueue< int, traits_VyukovMPMCCyclicQueue_static > queue_type;

        test_bounded_no_ic< queue_type >();
    }

    void HdrTestQueue::VyukovMPMCCycleQueue_static_ic()
    {
        typedef cds::container::VyukovMPMCCycleQueue< int, traits_VyukovMPMCCyclicQueue_static_ic > queue_type;
        test_bounded_ic< queue_type >();
    }

    void HdrTestQueue::VyukovMPMCCycleQueue_dyn()
    {
        test_bounded_no_ic< cds::container::VyukovMPMCCycleQueue< int > >();
    }

    void HdrTestQueue::VyukovMPMCCycleQueue_dyn_ic()
    {
        typedef cds::container::VyukovMPMCCycleQueue < int,
            typename cds::container::vyukov_queue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
        > queue_type;
        test_bounded_ic< queue_type >();
    }

}
