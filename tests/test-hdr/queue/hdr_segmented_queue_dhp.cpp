//$$CDS-header$$

#include "hdr_segmented_queue.h"
#include <cds/container/segmented_queue.h>
#include <cds/gc/dhp.h>

namespace queue {

    void HdrSegmentedQueue::SegmQueue_DHP()
    {
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item > queue_type;
        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_mutex()
    {
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item,
            cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_shuffle()
    {
        struct queue_traits : public cds::container::segmented_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
        };
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_stat()
    {
        struct queue_traits : public
            cds::container::segmented_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                , cds::opt::stat < cds::container::segmented_queue::stat<> >
            > ::type
        {};
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_cacheline_padding()
    {
        struct queue_traits : public cds::container::segmented_queue::traits
        {
            enum { padding = cds::opt::cache_line_padding };
        };

        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;
        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_mutex_cacheline_padding()
    {
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item,
            cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
                , cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_shuffle_cacheline_padding()
    {
        struct queue_traits : public cds::container::segmented_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
            enum { padding = cds::opt::cache_line_padding };
        };
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_stat_cacheline_padding()
    {
        struct queue_traits : public
            cds::container::segmented_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                , cds::opt::stat < cds::container::segmented_queue::stat<> >
                , cds::opt::padding< cds::opt::cache_line_padding >
            > ::type
        {};
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

} // namespace queue
