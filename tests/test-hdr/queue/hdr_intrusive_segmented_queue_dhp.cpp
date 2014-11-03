//$$CDS-header$$

#include "hdr_intrusive_segmented_queue.h"
#include <cds/intrusive/segmented_queue.h>
#include <cds/gc/dhp.h>

namespace queue {

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_mutex()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_shuffle()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_stat()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_cacheline_padding()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding };
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_mutex_cacheline_padding()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                , cds::opt::padding< cds::opt::cache_line_padding >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_shuffle_cacheline_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
                , cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_stat_cacheline_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
                , cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_256_padding()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = 256 };
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_mutex_256_padding()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                , cds::opt::padding< 256 >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_shuffle_256_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
                , cds::opt::padding< 256 >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_stat_256_padding()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
                , cds::opt::padding< 256 >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_cacheline_padding_bigdata()
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding };
        };
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, big_item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_mutex_cacheline_padding_bigdata()
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                , cds::opt::padding< cds::opt::cache_line_padding >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, big_item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_shuffle_cacheline_padding_bigdata()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, big_item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
                , cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrIntrusiveSegmentedQueue::SegmQueue_DHP_stat_cacheline_padding_bigdata()
    {
        typedef cds::intrusive::SegmentedQueue< cds::gc::DHP, big_item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                ,cds::opt::stat< cds::intrusive::segmented_queue::stat<> >
                , cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        > queue_type;

        test<queue_type>();
    }

} // namespace queue
