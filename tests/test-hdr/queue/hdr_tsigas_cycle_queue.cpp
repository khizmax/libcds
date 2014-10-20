//$$CDS-header$$

#include "hdr_queue.h"
#include <cds/container/tsigas_cycle_queue.h>

namespace queue {

    void HdrTestQueue::TsigasCycleQueue_static()
    {
        struct queue_traits : public cds::container::tsigas_queue::traits
        {
            typedef cds::opt::v::static_buffer< int, 1024 > buffer;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::container::TsigasCycleQueue< int, queue_traits > queue_type;

        test_bounded_no_ic<queue_type>();
    }

    void HdrTestQueue::TsigasCycleQueue_static_ic()
    {
        struct queue_traits : public cds::container::tsigas_queue::traits
        {
            typedef cds::opt::v::static_buffer< int, 1024 > buffer;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::TsigasCycleQueue< int, queue_traits > queue_type;

        test_bounded_ic<queue_type>();
    }
    void HdrTestQueue::TsigasCycleQueue_dyn()
    {
        class queue_type
            : public cds::container::TsigasCycleQueue< int,
            typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< int,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                >::type
            > base_class;
        public:
            queue_type()
                : base_class( 1024 )
            {}
        };

        test_bounded_no_ic<queue_type>();
    }
    void HdrTestQueue::TsigasCycleQueue_dyn_ic()
    {
        class queue_type
            : public cds::container::TsigasCycleQueue< int,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                    , cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            >
        {
            typedef cds::container::TsigasCycleQueue< int,
                typename cds::container::tsigas_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::dynamic_buffer< int > >
                    , cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > base_class;
        public:
            queue_type()
                : base_class( 1024 )
            {}
        };

        test_bounded_ic<queue_type>();
    }

} // namespace queue
