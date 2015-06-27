//$$CDS-header$$

#include <cds/container/optimistic_queue.h>
#include <cds/gc/dhp.h>

#include "queue/hdr_queue.h"

namespace queue {

    void HdrTestQueue::OptimisticQueue_DHP()
    {
        test_no_ic<cds::container::OptimisticQueue< cds::gc::DHP, int > >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_Counted()
    {
        struct queue_traits : public cds::container::optimistic_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        test_ic<cds::container::OptimisticQueue< cds::gc::DHP, int, queue_traits > >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_relax()
    {
        struct queue_traits : public cds::container::optimistic_queue::traits
        {
            typedef cds::opt::v::relaxed_ordering memory_model;
        };
        test_no_ic<cds::container::OptimisticQueue< cds::gc::DHP, int, queue_traits > >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_Counted_relax()
    {
        struct queue_traits : public cds::container::optimistic_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::relaxed_ordering memory_model;
        };
        test_ic <
            cds::container::OptimisticQueue< cds::gc::DHP, int, queue_traits > >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_seqcst()
    {
        test_no_ic<
            cds::container::OptimisticQueue< cds::gc::DHP, int,
                typename cds::container::optimistic_queue::make_traits<
                    cds::opt::memory_model< cds::opt::v::sequential_consistent>
                >::type
            >
        >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_Counted_seqcst()
    {
        test_ic<
            cds::container::OptimisticQueue< cds::gc::DHP, int,
                typename cds::container::optimistic_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                    ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                >::type
            >
        >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_relax_align()
    {
        test_no_ic<
            cds::container::OptimisticQueue< cds::gc::DHP, int,
                typename cds::container::optimistic_queue::make_traits<
                    cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                    ,cds::opt::padding< 16 >
                >::type
            >
        >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_Counted_relax_align()
    {
        test_ic<
            cds::container::OptimisticQueue< cds::gc::DHP, int,
                typename cds::container::optimistic_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                    ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                    ,cds::opt::padding< 32 >
                >::type
            >
        >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_seqcst_align()
    {
        test_no_ic<
            cds::container::OptimisticQueue< cds::gc::DHP, int,
                typename cds::container::optimistic_queue::make_traits<
                    cds::opt::memory_model< cds::opt::v::sequential_consistent>
                    ,cds::opt::padding< cds::opt::no_special_padding >
                >::type
            >
        >();
    }

    void HdrTestQueue::OptimisticQueue_DHP_Counted_seqcst_align()
    {
        test_ic<
            cds::container::OptimisticQueue< cds::gc::DHP, int,
                typename cds::container::optimistic_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                    ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                    ,cds::opt::padding< cds::opt::cache_line_padding >
                >::type
            >
        >();
    }

}   // namespace queue
