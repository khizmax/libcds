//$$CDS-header$$

#include <cds/container/optimistic_queue.h>
#include <cds/gc/hp.h>

#include "queue/hdr_queue.h"

namespace queue {

    void Queue_TestHeader::OptimisticQueue_HP()
    {
        testNoItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_Counted()
    {
        testWithItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_relax()
    {
        testNoItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_Counted_relax()
    {
        testWithItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_seqcst()
    {
        testNoItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_Counted_seqcst()
    {
        testWithItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_relax_align()
    {
        testNoItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 16 >
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_Counted_relax_align()
    {
        testWithItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 32 >
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_seqcst_align()
    {
        testNoItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::no_special_alignment >
            >
        >();
    }

    void Queue_TestHeader::OptimisticQueue_HP_Counted_seqcst_align()
    {
        testWithItemCounter<
            cds::container::OptimisticQueue< cds::gc::HP, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::cache_line_alignment >
            >
        >();
    }

}   // namespace queue
