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
#include <cds/gc/dhp.h>
#include <cds/intrusive/optimistic_queue.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        struct base_hook_item: public ci::optimistic_queue::node< cds::gc::DHP >
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            ci::optimistic_queue::node< cds::gc::DHP > hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        // DHP base hook
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > OptimisticQueue_DHP_base;

        // DHP member hook
        struct traits_OptimisticQueue_DHP_member : public OptimisticQueue_DHP_base::traits
        {
            typedef ci::optimistic_queue::member_hook <
                offsetof( member_hook_item, hMember ),
                ci::opt::gc < cds::gc::DHP >
            > hook;
        };
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item, traits_OptimisticQueue_DHP_member > OptimisticQueue_DHP_member;

        /// DHP base hook + item counter
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > OptimisticQueue_DHP_base_ic;

        // DHP member hook + item counter
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< traits_OptimisticQueue_DHP_member >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > OptimisticQueue_DHP_member_ic;

        // DHP base hook + stat
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::optimistic_queue::stat<> >
            >::type
        > OptimisticQueue_DHP_base_stat;

        // DHP member hook + stat
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< OptimisticQueue_DHP_base_stat::traits >
                , ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
            >::type
        > OptimisticQueue_DHP_member_stat;

        // DHP base hook + padding
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::padding< 32 >
            >::type
        > OptimisticQueue_DHP_base_align;

        // DHP member hook + padding
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::padding< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_DHP_member_align;

        // DHP base hook + no padding
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::padding< co::no_special_padding >
            >::type
        > OptimisticQueue_DHP_base_noalign;

        // DHP member hook + no padding
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::padding< co::no_special_padding >
            >::type
        > OptimisticQueue_DHP_member_noalign;


        // DHP base hook + cache padding
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::padding< co::cache_line_padding >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_DHP_base_cachealign;

        // DHP member hook + cache padding
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::padding< co::cache_line_padding >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_DHP_member_cachealign;
    }

    TEST(OptimisticQueue_DHP_base)
    TEST(OptimisticQueue_DHP_member)
    TEST(OptimisticQueue_DHP_base_ic)
    TEST(OptimisticQueue_DHP_member_ic)
    TEST(OptimisticQueue_DHP_base_stat)
    TEST(OptimisticQueue_DHP_member_stat)
    TEST(OptimisticQueue_DHP_base_align)
    TEST(OptimisticQueue_DHP_member_align)
    TEST(OptimisticQueue_DHP_base_noalign)
    TEST(OptimisticQueue_DHP_member_noalign)
    TEST(OptimisticQueue_DHP_base_cachealign)
    TEST(OptimisticQueue_DHP_member_cachealign)
}
