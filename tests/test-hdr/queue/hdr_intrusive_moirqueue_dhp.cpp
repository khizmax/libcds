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
#include <cds/intrusive/moir_queue.h>
#include <cds/gc/dhp.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef IntrusiveQueueHeaderTest::base_hook_item< ci::msqueue::node<cds::gc::DHP > > base_item_type;
        typedef IntrusiveQueueHeaderTest::member_hook_item< ci::msqueue::node<cds::gc::DHP > > member_item_type;

        // DHP base hook
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_DHP_base;

        // DHP member hook
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_DHP_member;

        /// DHP base hook + item counter
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , co::item_counter< cds::atomicity::item_counter >
            , co::memory_model< co::v::relaxed_ordering >
            >::type
        > MoirQueue_DHP_base_ic;

        // DHP member hook + item counter
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , co::item_counter< cds::atomicity::item_counter >
            >::type
        > MoirQueue_DHP_member_ic;

        // DHP base hook + stat
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , co::stat< ci::msqueue::stat<> >
            >::type
        > MoirQueue_DHP_base_stat;

        // DHP member hook + stat
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , co::stat< ci::msqueue::stat<> >
            >::type
        > MoirQueue_DHP_member_stat;

        // DHP base hook + padding
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , co::padding< 32 >
            >::type
        > MoirQueue_DHP_base_align;

        // DHP member hook + padding
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , co::padding< 32 >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_DHP_member_align;

        // DHP base hook + no padding
        struct traits_MoirQueue_DHP_base_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { padding = co::no_special_padding };
        };
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type, traits_MoirQueue_DHP_base_noalign > MoirQueue_DHP_base_noalign;

        // DHP member hook + no padding
        struct traits_MoirQueue_DHP_member_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::member_hook <
                offsetof( member_item_type, hMember ),
                ci::opt::gc < cds::gc::DHP >
            > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { padding = co::no_special_padding };
        };
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type, traits_MoirQueue_DHP_member_noalign > MoirQueue_DHP_member_noalign;


        // DHP base hook + cache padding
        struct traits_MoirQueue_DHP_base_cachealign : public traits_MoirQueue_DHP_base_noalign
        {
            enum { padding = co::cache_line_padding };
        };
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type, traits_MoirQueue_DHP_base_cachealign > MoirQueue_DHP_base_cachealign;

        // DHP member hook + cache padding
        struct traits_MoirQueue_DHP_member_cachealign : public traits_MoirQueue_DHP_member_noalign
        {
            enum { padding = co::cache_line_padding };
        };
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type, traits_MoirQueue_DHP_member_cachealign > MoirQueue_DHP_member_cachealign;
    }   // namespace

    TEST(MoirQueue_DHP_base)
    TEST(MoirQueue_DHP_member)
    TEST(MoirQueue_DHP_base_ic)
    TEST(MoirQueue_DHP_member_ic)
    TEST(MoirQueue_DHP_base_stat)
    TEST(MoirQueue_DHP_member_stat)
    TEST(MoirQueue_DHP_base_align)
    TEST(MoirQueue_DHP_member_align)
    TEST(MoirQueue_DHP_base_noalign)
    TEST(MoirQueue_DHP_member_noalign)
    TEST(MoirQueue_DHP_base_cachealign)
    TEST(MoirQueue_DHP_member_cachealign)

} // namespace queue
