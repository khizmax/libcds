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

#include "hdr_treiber_stack.h"
#include <cds/gc/hp.h>
#include <cds/container/treiber_stack.h>

namespace stack {
#define TEST(X)         void TestStack::X() { test<defs::X>(); }
#define TEST_DYN(X)     void TestStack::X() { test_elimination<defs::X>(); }

    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
            >::type
        > Elimination_HP;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_HP_dyn;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<void *> >
                , cds::opt::stat< cs::treiber_stack::stat<> >
            >::type
        > Elimination_HP_stat;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Elimination_HP_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,cds::opt::back_off< cds::backoff::yield>
            >::type
        > Elimination_HP_yield;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
                ,cds::opt::enable_elimination<true>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Elimination_HP_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::allocator< std::allocator< bool * > >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_HP_pause_alloc;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::allocator< std::allocator< bool * > >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_HP_pause_alloc_relaxed;

    }}

    TEST(Elimination_HP)
    TEST_DYN(Elimination_HP_dyn)
    TEST_DYN(Elimination_HP_stat)
    TEST(Elimination_HP_yield)
    TEST(Elimination_HP_pause_alloc)

    TEST(Elimination_HP_relaxed)
    TEST(Elimination_HP_yield_relaxed)
    TEST(Elimination_HP_pause_alloc_relaxed)
}
