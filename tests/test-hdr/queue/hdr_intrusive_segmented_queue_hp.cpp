//$$CDS-header$$

#include "hdr_intrusive_segmented_queue.h"
#include <cds/intrusive/segmented_queue.h>
#include <cds/gc/hp.h>

namespace queue {

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_mutex()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item, queue_traits > queue_type;

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

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_cacheline_padding()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding };
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_mutex_cacheline_padding()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
                ,cds::opt::padding< cds::opt::cache_line_padding >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_shuffle_cacheline_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::padding< cds::opt::cache_line_padding >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_stat_cacheline_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                , cds::opt::padding< cds::opt::cache_line_padding >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_256_padding()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = 256 };
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_mutex_256_padding()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
                ,cds::opt::padding< 256 >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_shuffle_256_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::padding< 256 >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_stat_256_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                , cds::opt::padding< 256 >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_cacheline_padding_bigdata()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only };
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, big_item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_mutex_cacheline_padding_bigdata()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
                , cds::opt::padding< cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, big_item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_shuffle_cacheline_padding_bigdata()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, big_item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::padding< cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_HP_stat_cacheline_padding_bigdata()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::HP, big_item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                , cds::opt::padding< cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

} // namespace queue
