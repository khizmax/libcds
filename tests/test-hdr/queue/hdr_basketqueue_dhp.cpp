//$$CDS-header$$

#include <cds/gc/dhp.h>
#include <cds/container/basket_queue.h>

#include "queue/hdr_queue.h"

namespace queue {

    void HdrTestQueue::BasketQueue_DHP()
    {
        typedef cds::container::BasketQueue< cds::gc::DHP, int > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_Counted()
    {
        struct traits : public cds::container::basket_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::BasketQueue< cds::gc::DHP, int, traits > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_relax()
    {
        struct traits : public
            cds::container::basket_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            > ::type
        {};
        typedef cds::container::BasketQueue< cds::gc::DHP, int, traits > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_Counted_relax()
    {
        typedef cds::container::BasketQueue< cds::gc::DHP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >::type
        > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_seqcst()
    {
        struct traits : public cds::container::basket_queue::traits
        {
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::container::BasketQueue< cds::gc::DHP, int, traits > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_Counted_seqcst()
    {
        typedef cds::container::BasketQueue < cds::gc::DHP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >::type
        > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_relax_align()
    {
        typedef cds::container::BasketQueue < cds::gc::DHP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 16 >
            >::type
        > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_Counted_relax_align()
    {
        struct traits : public cds::container::basket_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::relaxed_ordering memory_model;
            enum { alignment = 32 };
        };
        typedef cds::container::BasketQueue < cds::gc::DHP, int, traits > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_seqcst_align()
    {
        typedef cds::container::BasketQueue < cds::gc::DHP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::sequential_consistent>
                , cds::opt::alignment< cds::opt::no_special_alignment >
            > ::type
        > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_DHP_Counted_seqcst_align()
    {
        typedef cds::container::BasketQueue < cds::gc::DHP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::cache_line_alignment >
            > ::type
        > queue_type;
        test_ic< queue_type >();
    }

}   // namespace queue
