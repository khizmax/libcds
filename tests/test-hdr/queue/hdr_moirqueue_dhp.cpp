//$$CDS-header$$

#include <cds/gc/dhp.h>
#include <cds/container/moir_queue.h>
#include "queue/hdr_queue.h"

namespace queue {

    void HdrTestQueue::MoirQueue_DHP()
    {
        typedef cds::container::MoirQueue< cds::gc::DHP, int > test_queue;
        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_Counted()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_relax()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_Counted_relax()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_seqcst()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model < cds::opt::v::sequential_consistent >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_Counted_seqcst()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::sequential_consistent >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_relax_align()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::alignment < 16 >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_Counted_relax_align()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::alignment < 32 >
            >::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_seqcst_align()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment < cds::opt::no_special_alignment >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MoirQueue_DHP_Counted_seqcst_align()
    {
        typedef cds::container::MoirQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model< cds::opt::v::sequential_consistent>
                , cds::opt::alignment < cds::opt::cache_line_alignment >
            > ::type
        > test_queue;
        test_ic< test_queue >();
    }
}   // namespace queue
