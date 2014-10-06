//$$CDS-header$$

#include <cds/container/basket_queue.h>
#include <cds/gc/ptb.h>

#include "queue/hdr_queue.h"

namespace queue {

    void Queue_TestHeader::BasketQueue_PTB()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_Counted()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_relax()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_Counted_relax()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_seqcst()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_Counted_seqcst()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_relax_align()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 16 >
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_Counted_relax_align()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 32 >
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_seqcst_align()
    {
        testNoItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::no_special_alignment >
            >
        >();
    }

    void Queue_TestHeader::BasketQueue_PTB_Counted_seqcst_align()
    {
        testWithItemCounter<
            cds::container::BasketQueue< cds::gc::PTB, int
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::cache_line_alignment >
            >
        >();
    }

}   // namespace queue
