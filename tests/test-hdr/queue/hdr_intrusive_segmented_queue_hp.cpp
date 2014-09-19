//$$CDS-header$$

#include "hdr_intrusive_segmented_queue.h"
#include <cds/intrusive/segmented_queue.h>
#include <cds/gc/hp.h>

namespace queue {

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_mutex()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_shuffle()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_stat()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

} // namespace queue

CPPUNIT_TEST_SUITE_REGISTRATION(queue::HdrIntrusiveSegmentedQueue);
