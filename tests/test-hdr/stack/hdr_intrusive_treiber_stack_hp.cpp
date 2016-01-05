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

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/hp.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }

    namespace defs { namespace {
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
        > Treiber_HP_default;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_HP_default_relaxed;

        // HZP GC + base hook
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            >::type
        > Treiber_HP_base;

        struct traits_Treiber_HP_base_relaxed
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>,
            traits_Treiber_HP_base_relaxed
        > Treiber_HP_base_relaxed;

        // HZP GC + base hook + disposer
        struct traits_Treiber_HP_base_disposer
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>,
            traits_Treiber_HP_base_disposer
        > Treiber_HP_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            ,typename ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_HP_base_disposer_relaxed;

        // HZP GC + member hook
        struct traits_Treiber_HP_member
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member
        > Treiber_HP_member;

        struct traits_Treiber_HP_member_relaxed
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member_relaxed
        > Treiber_HP_member_relaxed;

        // HZP GC + member hook + disposer
        struct traits_Treiber_HP_member_disposer
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member_disposer
        > Treiber_HP_member_disposer;

        struct traits_Treiber_HP_member_disposer_relaxed
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member_disposer_relaxed
        > Treiber_HP_member_disposer_relaxed;
    }}

    TEST(Treiber_HP_default)
    TEST(Treiber_HP_base)
    TEST(Treiber_HP_base_disposer)
    TEST(Treiber_HP_member)
    TEST(Treiber_HP_member_disposer)

    TEST(Treiber_HP_default_relaxed)
    TEST(Treiber_HP_base_relaxed)
    TEST(Treiber_HP_base_disposer_relaxed)
    TEST(Treiber_HP_member_relaxed)
    TEST(Treiber_HP_member_disposer_relaxed)

} // namespace stack

CPPUNIT_TEST_SUITE_REGISTRATION(stack::TestIntrusiveStack);
