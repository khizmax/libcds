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
#include <cds/intrusive/basket_queue.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test_basket<X>(); }

    namespace {
        typedef IntrusiveQueueHeaderTest::base_hook_item < ci::basket_queue::node<cds::gc::DHP> > base_hook_item;
        typedef IntrusiveQueueHeaderTest::member_hook_item < ci::basket_queue::node<cds::gc::DHP> > member_hook_item;

        // DHP base hook
        struct traits_BasketQueue_disposer : public
            ci::basket_queue::make_traits < ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer > >::type
        {
            typedef ci::basket_queue::base_hook< ci::opt::gc < cds::gc::DHP > > hook;
        };
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item, traits_BasketQueue_disposer > BasketQueue_DHP_base;

        // DHP member hook
        struct traits_BasketQueue_member : public traits_BasketQueue_disposer
        {
            typedef ci::basket_queue::member_hook< offsetof( member_hook_item, hMember ), ci::opt::gc < cds::gc::DHP > > hook;
        };
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item, traits_BasketQueue_member >BasketQueue_DHP_member;

        /// DHP base hook + item counter
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > BasketQueue_DHP_base_ic;

        // DHP member hook + item counter
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof( member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > BasketQueue_DHP_member_ic;

        // DHP base hook + stat
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::basket_queue::stat<> >
            >::type
        > BasketQueue_DHP_base_stat;

        // DHP member hook + stat
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::basket_queue::stat<> >
            >::type
        > BasketQueue_DHP_member_stat;

        // DHP base hook + padding
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::padding< 32 >
            >::type
        > BasketQueue_DHP_base_align;

        // DHP member hook + padding
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::padding< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_DHP_member_align;

        // DHP base hook + no padding
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::padding< co::no_special_padding >
            >::type
        > BasketQueue_DHP_base_noalign;

        // DHP member hook + no padding
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::padding< co::no_special_padding >
            >::type
        > BasketQueue_DHP_member_noalign;


        // DHP base hook + cache padding
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::padding< co::cache_line_padding >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_DHP_base_cachealign;

        // DHP member hook + cache padding
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::padding< co::cache_line_padding >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_DHP_member_cachealign;
    } // namespace

    TEST(BasketQueue_DHP_base)
    TEST(BasketQueue_DHP_member)
    TEST(BasketQueue_DHP_base_ic)
    TEST(BasketQueue_DHP_member_ic)
    TEST(BasketQueue_DHP_base_stat)
    TEST(BasketQueue_DHP_member_stat)
    TEST(BasketQueue_DHP_base_align)
    TEST(BasketQueue_DHP_member_align)
    TEST(BasketQueue_DHP_base_noalign)
    TEST(BasketQueue_DHP_member_noalign)
    TEST(BasketQueue_DHP_base_cachealign)
    TEST(BasketQueue_DHP_member_cachealign)
}
