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

#include "hdr_segmented_queue.h"
#include <cds/container/segmented_queue.h>
#include <cds/gc/dhp.h>

namespace queue {

    void HdrSegmentedQueue::SegmQueue_DHP()
    {
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item > queue_type;
        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_mutex()
    {
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item,
            cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_shuffle()
    {
        struct queue_traits : public cds::container::segmented_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
        };
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_stat()
    {
        struct queue_traits : public
            cds::container::segmented_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                , cds::opt::stat < cds::container::segmented_queue::stat<> >
            > ::type
        {};
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_cacheline_padding()
    {
        struct queue_traits : public cds::container::segmented_queue::traits
        {
            enum { padding = cds::opt::cache_line_padding };
        };

        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;
        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_mutex_cacheline_padding()
    {
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item,
            cds::container::segmented_queue::make_traits<
                cds::opt::lock_type< std::mutex >
                , cds::opt::padding< cds::opt::cache_line_padding >
            >::type
        > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_shuffle_cacheline_padding()
    {
        struct queue_traits : public cds::container::segmented_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
            enum { padding = cds::opt::cache_line_padding };
        };
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

    void HdrSegmentedQueue::SegmQueue_DHP_stat_cacheline_padding()
    {
        struct queue_traits : public
            cds::container::segmented_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                , cds::opt::stat < cds::container::segmented_queue::stat<> >
                , cds::opt::padding< cds::opt::cache_line_padding >
            > ::type
        {};
        typedef cds::container::SegmentedQueue< cds::gc::DHP, item, queue_traits > queue_type;

        test<queue_type>();
    }

} // namespace queue
