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

#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/tsigas_cycle_queue.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test2<X>(); }

    namespace {
        struct item {
            int nVal;
            int nDisposeCount;

            item()
                : nDisposeCount(0)
            {}
        };

        struct traits_TsigasCycleQueue_static : public cds::intrusive::tsigas_queue::traits
        {
            typedef co::v::static_buffer< int, 1024 > buffer;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef ci::TsigasCycleQueue< item, traits_TsigasCycleQueue_static > TsigasCycleQueue_static;

        struct traits_traits_TsigasCycleQueue_static_ic : public traits_TsigasCycleQueue_static
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::TsigasCycleQueue< item, traits_traits_TsigasCycleQueue_static_ic > TsigasCycleQueue_static_ic;

        class TsigasCycleQueue_dyn
            : public ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                    co::buffer< co::v::dynamic_buffer< int > >
                    ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                >::type
            >
        {
            typedef ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                   co::buffer< co::v::dynamic_buffer< int > >
                    , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                >::type
            > base_class;
        public:
            TsigasCycleQueue_dyn()
                : base_class( 1024 )
            {}
        };

        class TsigasCycleQueue_dyn_ic
            : public ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                    co::buffer< co::v::dynamic_buffer< int > >
                    ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                    ,co::item_counter< cds::atomicity::item_counter >
                >::type
            >
        {
            typedef ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                    co::buffer< co::v::dynamic_buffer< int > >
                    , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                    , co::item_counter< cds::atomicity::item_counter >
                >::type
            > base_class;
        public:
            TsigasCycleQueue_dyn_ic()
                : base_class( 1024 )
            {}
        };
    }

    TEST(TsigasCycleQueue_static)
    TEST(TsigasCycleQueue_static_ic)
    TEST(TsigasCycleQueue_dyn)
    TEST(TsigasCycleQueue_dyn_ic)

} // namespace queue
