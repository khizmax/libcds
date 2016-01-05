/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#include <cds/gc/dhp.h>
#include <cds/container/msqueue.h>

#include "queue/hdr_queue.h"

namespace queue {

    void HdrTestQueue::MSQueue_DHP()
    {
        typedef cds::container::MSQueue< cds::gc::DHP, int > test_queue;
        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_Counted()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_relax()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_Counted_relax()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_seqcst()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model < cds::opt::v::sequential_consistent >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_Counted_seqcst()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::sequential_consistent >
            > ::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_relax_align()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::padding < 16 >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_Counted_relax_align()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::padding < 32 >
            >::type
        > test_queue;

        test_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_seqcst_align()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model< cds::opt::v::sequential_consistent>
                ,cds::opt::padding < cds::opt::no_special_padding >
            > ::type
        > test_queue;

        test_no_ic< test_queue >();
    }

    void HdrTestQueue::MSQueue_DHP_Counted_seqcst_align()
    {
        typedef cds::container::MSQueue < cds::gc::DHP, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model< cds::opt::v::sequential_consistent>
                , cds::opt::padding < cds::opt::cache_line_padding >
            > ::type
        > test_queue;
        test_ic< test_queue >();
    }
}   // namespace queue
