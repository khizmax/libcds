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
#include <cds/gc/hp.h>
#include <cds/intrusive/optimistic_queue.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        struct base_hook_item: public ci::optimistic_queue::node< cds::gc::HP >
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
            ci::optimistic_queue::node< cds::gc::HP > hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        struct traits_OptimisticQueue_HP_default : public ci::optimistic_queue::traits
        {
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
        };
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item, traits_OptimisticQueue_HP_default > OptimisticQueue_HP_default;

        /// HP + item counter
        struct traits_OptimisticQueue_HP_default_ic : public traits_OptimisticQueue_HP_default
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item, traits_OptimisticQueue_HP_default_ic > OptimisticQueue_HP_default_ic;

        /// HP + stat
        struct traits_OptimisticQueue_HP_default_stat : public
            ci::optimistic_queue::make_traits <
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                , co::stat< ci::optimistic_queue::stat<> >
                , co::memory_model< co::v::sequential_consistent >
            > ::type
        {};
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item, traits_OptimisticQueue_HP_default_stat > OptimisticQueue_HP_default_stat;

        // HP base hook
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > OptimisticQueue_HP_base;

        // HP member hook
        struct traits_OptimisticQueue_HP_member : public OptimisticQueue_HP_base::traits
        {
            typedef ci::optimistic_queue::member_hook <
                offsetof( member_hook_item, hMember ),
                ci::opt::gc < cds::gc::HP >
            > hook;
        };
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item, traits_OptimisticQueue_HP_member > OptimisticQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > OptimisticQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< traits_OptimisticQueue_HP_member >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > OptimisticQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::optimistic_queue::stat<> >
            >::type
        > OptimisticQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< OptimisticQueue_HP_base_stat::traits >
                , ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            >::type
        > OptimisticQueue_HP_member_stat;

        // HP base hook + padding
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::padding< 32 >
            >::type
        > OptimisticQueue_HP_base_align;

        // HP member hook + padding
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::padding< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_HP_member_align;

        // HP base hook + no padding
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::padding< co::no_special_padding >
            >::type
        > OptimisticQueue_HP_base_noalign;

        // HP member hook + no padding
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::padding< co::no_special_padding >
            >::type
        > OptimisticQueue_HP_member_noalign;


        // HP base hook + cache padding
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::padding< co::cache_line_padding >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_HP_base_cachealign;

        // HP member hook + cache padding
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::padding< co::cache_line_padding >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_HP_member_cachealign;
    }

    TEST(OptimisticQueue_HP_default)
    TEST(OptimisticQueue_HP_default_ic)
    TEST(OptimisticQueue_HP_default_stat)
    TEST(OptimisticQueue_HP_base)
    TEST(OptimisticQueue_HP_member)
    TEST(OptimisticQueue_HP_base_ic)
    TEST(OptimisticQueue_HP_member_ic)
    TEST(OptimisticQueue_HP_base_stat)
    TEST(OptimisticQueue_HP_member_stat)
    TEST(OptimisticQueue_HP_base_align)
    TEST(OptimisticQueue_HP_member_align)
    TEST(OptimisticQueue_HP_base_noalign)
    TEST(OptimisticQueue_HP_member_noalign)
    TEST(OptimisticQueue_HP_base_cachealign)
    TEST(OptimisticQueue_HP_member_cachealign)
}
