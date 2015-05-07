/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include <cds/gc/hp.h>
#include <cds/container/basket_queue.h>

#include "queue/hdr_queue.h"

namespace queue {

    void HdrTestQueue::BasketQueue_HP()
    {
        typedef cds::container::BasketQueue< cds::gc::HP, int > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_Counted()
    {
        struct traits : public cds::container::basket_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::BasketQueue< cds::gc::HP, int, traits > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_relax()
    {
        struct traits : public
            cds::container::basket_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            > ::type
        {};
        typedef cds::container::BasketQueue< cds::gc::HP, int, traits > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_Counted_relax()
    {
        typedef cds::container::BasketQueue< cds::gc::HP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::relaxed_ordering>
            >::type
        > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_seqcst()
    {
        struct traits : public cds::container::basket_queue::traits
        {
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::container::BasketQueue< cds::gc::HP, int, traits > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_Counted_seqcst()
    {
        typedef cds::container::BasketQueue < cds::gc::HP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
            >::type
        > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_relax_align()
    {
        typedef cds::container::BasketQueue < cds::gc::HP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                ,cds::opt::alignment< 16 >
            >::type
        > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_Counted_relax_align()
    {
        struct traits : public cds::container::basket_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::relaxed_ordering memory_model;
            enum { alignment = 32 };
        };
        typedef cds::container::BasketQueue < cds::gc::HP, int, traits > queue_type;
        test_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_seqcst_align()
    {
        typedef cds::container::BasketQueue < cds::gc::HP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::memory_model< cds::opt::v::sequential_consistent>
                , cds::opt::alignment< cds::opt::no_special_alignment >
            > ::type
        > queue_type;
        test_no_ic< queue_type >();
    }

    void HdrTestQueue::BasketQueue_HP_Counted_seqcst_align()
    {
        typedef cds::container::BasketQueue < cds::gc::HP, int,
            typename cds::container::basket_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment< cds::opt::cache_line_alignment >
            > ::type
        > queue_type;
        test_ic< queue_type >();
    }

}   // namespace queue
