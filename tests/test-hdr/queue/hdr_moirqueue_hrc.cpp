//$$CDS-header$$

#include <cds/container/moir_queue.h>
#include <cds/gc/hrc.h>

#include "queue/queue_test_header.h"

namespace queue {

    void Queue_TestHeader::MoirQueue_HRC()
    {
        testNoItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_Counted()
    {
        testWithItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_relax()
    {
        testNoItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_Counted_relax()
    {
        testWithItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_seqcst()
    {
        testNoItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_Counted_seqcst()
    {
        testWithItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_relax_align()
    {
        testNoItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 16 >
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_Counted_relax_align()
    {
        testWithItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 32 >
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_seqcst_align()
    {
        testNoItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::no_special_alignment >
            >
        >();
    }

    void Queue_TestHeader::MoirQueue_HRC_Counted_seqcst_align()
    {
        testWithItemCounter<
            cds::container::MoirQueue< cds::gc::HRC, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::cache_line_alignment >
            >
        >();
    }

}   // namespace queue
