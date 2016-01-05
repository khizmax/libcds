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

#include <cds/container/vyukov_mpmc_cycle_queue.h>

#include "queue/hdr_queue.h"

namespace queue {
    namespace{
        struct traits_VyukovMPMCCyclicQueue_static : public cds::container::vyukov_queue::traits
        {
            typedef cds::opt::v::static_buffer<int, 1024> buffer;
        };
        struct traits_VyukovMPMCCyclicQueue_static_ic : public traits_VyukovMPMCCyclicQueue_static
        {
            typedef cds::atomicity::item_counter item_counter;
        };
    }
    void HdrTestQueue::VyukovMPMCCycleQueue_static()
    {
        typedef cds::container::VyukovMPMCCycleQueue< int, traits_VyukovMPMCCyclicQueue_static > queue_type;

        test_bounded_no_ic< queue_type >();
    }

    void HdrTestQueue::VyukovMPMCCycleQueue_static_ic()
    {
        typedef cds::container::VyukovMPMCCycleQueue< int, traits_VyukovMPMCCyclicQueue_static_ic > queue_type;
        test_bounded_ic< queue_type >();
    }

    void HdrTestQueue::VyukovMPMCCycleQueue_dyn()
    {
        class queue_type : public cds::container::VyukovMPMCCycleQueue < int >
        {
            typedef cds::container::VyukovMPMCCycleQueue < int > base_class;
        public:
            queue_type() : base_class( 1024 ) {}
        };
        test_bounded_no_ic< queue_type >();
    }

    void HdrTestQueue::VyukovMPMCCycleQueue_dyn_ic()
    {
        class queue_type :
            public cds::container::VyukovMPMCCycleQueue < int,
            typename cds::container::vyukov_queue::make_traits <
            cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
            >
        {
            typedef cds::container::VyukovMPMCCycleQueue < int,
                typename cds::container::vyukov_queue::make_traits <
                    cds::opt::item_counter < cds::atomicity::item_counter >
                > ::type
            > base_class;
        public:
            queue_type() : base_class( 1024 ) {}
        };
        test_bounded_ic< queue_type >();
    }

}
