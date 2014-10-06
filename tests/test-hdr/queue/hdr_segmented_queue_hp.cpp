//$$CDS-header$$

#include "hdr_segmented_queue.h"
#include <cds/container/segmented_queue.h>
#include <cds/gc/hp.h>

namespace queue {

    void HdrSegmentedQueue::SegmQueue_HP()
    {
        typedef cds::container::SegmentedQueue< cds::gc::HP, item > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_HP_mutex()
    {
        typedef cds::container::SegmentedQueue< cds::gc::HP, item,
            cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_HP_shuffle()
    {
        typedef cds::container::SegmentedQueue< cds::gc::HP, item,
            cds::container::segmented_queue::make_traits<
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_HP_stat()
    {
        typedef cds::container::SegmentedQueue< cds::gc::HP, item,
            cds::container::segmented_queue::make_traits<
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::container::segmented_queue::stat<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

} // namespace queue
