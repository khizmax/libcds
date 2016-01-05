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

#include <cds/intrusive/msqueue.h>
#include <cds/gc/hp.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef IntrusiveQueueHeaderTest::base_hook_item< ci::msqueue::node<cds::gc::HP > > base_item_type;
        typedef IntrusiveQueueHeaderTest::member_hook_item< ci::msqueue::node<cds::gc::HP > > member_item_type;

        typedef ci::MSQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MSQueue_HP_default;

        /// HP + item counter
        typedef ci::MSQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        > MSQueue_HP_default_ic;

        /// HP + stat
        typedef ci::MSQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::msqueue::stat<> >
            >::type
        > MSQueue_HP_default_stat;

        // HP base hook
        typedef ci::MSQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MSQueue_HP_base;

        // HP member hook
        typedef ci::MSQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MSQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::MSQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > MSQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::MSQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > MSQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::MSQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::msqueue::stat<> >
            >::type
        > MSQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::MSQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::msqueue::stat<> >
            >::type
        > MSQueue_HP_member_stat;

        // HP base hook + padding
        typedef ci::MSQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::padding< 32 >
            >::type
        > MSQueue_HP_base_align;

        // HP member hook + padding
        typedef ci::MSQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::padding< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MSQueue_HP_member_align;

        // HP base hook + no padding
        struct traits_MSQueue_HP_base_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { padding = co::no_special_padding };
        };
        typedef ci::MSQueue< cds::gc::HP, base_item_type, traits_MSQueue_HP_base_noalign > MSQueue_HP_base_noalign;

        // HP member hook + no padding
        struct traits_MSQueue_HP_member_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::member_hook <
                offsetof( member_item_type, hMember ),
                ci::opt::gc < cds::gc::HP >
            > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { padding = co::no_special_padding };
        };
        typedef ci::MSQueue< cds::gc::HP, member_item_type, traits_MSQueue_HP_member_noalign > MSQueue_HP_member_noalign;


        // HP base hook + cache padding
        struct traits_MSQueue_HP_base_cachealign : public traits_MSQueue_HP_base_noalign
        {
            enum { padding = co::cache_line_padding };
        };
        typedef ci::MSQueue< cds::gc::HP, base_item_type, traits_MSQueue_HP_base_cachealign > MSQueue_HP_base_cachealign;

        // HP member hook + cache padding
        struct traits_MSQueue_HP_member_cachealign : public traits_MSQueue_HP_member_noalign
        {
            enum { padding = co::cache_line_padding };
        };
        typedef ci::MSQueue< cds::gc::HP, member_item_type, traits_MSQueue_HP_member_cachealign > MSQueue_HP_member_cachealign;

    }

    TEST(MSQueue_HP_default)
    TEST(MSQueue_HP_default_ic)
    TEST(MSQueue_HP_default_stat)
    TEST(MSQueue_HP_base)
    TEST(MSQueue_HP_member)
    TEST(MSQueue_HP_base_ic)
    TEST(MSQueue_HP_member_ic)
    TEST(MSQueue_HP_base_stat)
    TEST(MSQueue_HP_member_stat)
    TEST(MSQueue_HP_base_align)
    TEST(MSQueue_HP_member_align)
    TEST(MSQueue_HP_base_noalign)
    TEST(MSQueue_HP_member_noalign)
    TEST(MSQueue_HP_base_cachealign)
    TEST(MSQueue_HP_member_cachealign)
}

