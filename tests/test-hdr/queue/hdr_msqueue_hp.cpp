/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include <cds/gc/hp.h>
#include <cds/container/msqueue.h>

#include "queue/hdr_queue.h"

namespace queue {

    void HdrTestQueue::MSQueue_HP()
    {
        typedef cds::container::MSQueue< cds::gc::HP, int > test_queue;
        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_Counted()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_relax()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_Counted_relax()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_seqcst()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model < cds::opt::v::sequential_consistent >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_Counted_seqcst()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::sequential_consistent >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_relax_align()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::alignment < 16 >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_Counted_relax_align()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::alignment < 32 >
            >::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_seqcst_align()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::alignment < cds::opt::no_special_alignment >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_HP_Counted_seqcst_align()
    {
        typedef cds::container::MSQueue < cds::gc::HP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model< cds::opt::v::sequential_consistent>
                , cds::opt::alignment < cds::opt::cache_line_alignment >
            > ::type
        > test_queue;
        test_ic< test_queue >();
    }
}   // namespace queue
