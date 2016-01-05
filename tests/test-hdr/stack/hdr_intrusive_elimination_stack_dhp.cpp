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
#include <cds/gc/dhp.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }
#define TEST_DYN(X) void TestIntrusiveStack::X() { test_elimination<defs::X>(); }

    namespace defs {
        // DHP GC + base hook
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
            >::type
        > Elimination_DHP_base;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_DHP_base_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_base_relaxed;

        // DHP GC + base hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_DHP_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_base_disposer_relaxed;

        // DHP GC + member hook
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_member;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_DHP_member_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_member_relaxed;

        // DHP GC + member hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_DHP_member_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_member_disposer_relaxed;
    }

    TEST(Elimination_DHP_base)
    TEST_DYN(Elimination_DHP_base_dyn)
    TEST(Elimination_DHP_base_disposer)
    TEST(Elimination_DHP_member)
    TEST_DYN(Elimination_DHP_member_dyn)
    TEST(Elimination_DHP_member_disposer)

    TEST(Elimination_DHP_base_relaxed)
    TEST(Elimination_DHP_base_disposer_relaxed)
    TEST(Elimination_DHP_member_relaxed)
    TEST(Elimination_DHP_member_disposer_relaxed)

} // namespace stack

