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

#include "priority_queue/hdr_intrusive_pqueue.h"
#include <cds/intrusive/mspriority_queue.h>

namespace priority_queue {
    namespace intrusive_pqueue {
        template <typename T, typename Traits>
        struct constants<cds::intrusive::MSPriorityQueue<T, Traits> > {
            static size_t const nCapacity = c_nCapacity - 1;
        };
    }

    namespace {
        typedef cds::opt::v::dynamic_buffer< char > buffer_type;
    }

    void IntrusivePQueueHdrTest::MSPQueue_dyn()
    {
        struct pqueue_traits : public cds::intrusive::mspriority_queue::traits
        {
            typedef buffer_type buffer;
        };
        typedef cds::intrusive::MSPriorityQueue< IntrusivePQueueHdrTest::key_type, pqueue_traits > pqueue;

        test_msq_dyn<pqueue>();
    }

    void IntrusivePQueueHdrTest::MSPQueue_dyn_cmp()
    {
        struct pqueue_traits : public cds::intrusive::mspriority_queue::traits
        {
            typedef buffer_type buffer;
            typedef IntrusivePQueueHdrTest::compare compare;
        };
        typedef cds::intrusive::MSPriorityQueue< IntrusivePQueueHdrTest::key_type, pqueue_traits > pqueue;

        test_msq_dyn<pqueue>();
    }

    void IntrusivePQueueHdrTest::MSPQueue_dyn_less()
    {
        struct pqueue_traits : public
            cds::intrusive::mspriority_queue::make_traits <
                cds::opt::buffer< buffer_type >
                , cds::opt::less < std::less<IntrusivePQueueHdrTest::key_type> >
            > ::type
        {};
        typedef cds::intrusive::MSPriorityQueue< IntrusivePQueueHdrTest::key_type, pqueue_traits > pqueue;

        test_msq_dyn<pqueue>();
    }

    void IntrusivePQueueHdrTest::MSPQueue_dyn_cmpless()
    {
        typedef cds::intrusive::MSPriorityQueue< IntrusivePQueueHdrTest::key_type,
            cds::intrusive::mspriority_queue::make_traits<
                cds::opt::buffer< buffer_type >
                ,cds::opt::less< std::less<IntrusivePQueueHdrTest::key_type> >
                ,cds::opt::compare< IntrusivePQueueHdrTest::compare >
            >::type
        > pqueue;

        test_msq_dyn<pqueue>();
    }

    void IntrusivePQueueHdrTest::MSPQueue_dyn_cmp_mtx()
    {
        typedef cds::intrusive::MSPriorityQueue< IntrusivePQueueHdrTest::key_type,
            cds::intrusive::mspriority_queue::make_traits<
                cds::opt::buffer< buffer_type >
                ,cds::opt::compare< IntrusivePQueueHdrTest::compare >
                ,cds::opt::lock_type<std::mutex>
            >::type
        > pqueue;

        test_msq_dyn<pqueue>();
    }

} // namespace priority_queue
