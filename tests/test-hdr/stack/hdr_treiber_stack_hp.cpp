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
#define TEST(X)     void TestStack::X() { test<defs::X>(); }
    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::HP, int > Treiber_HP;
        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Treiber_HP_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
            >::type
        > Treiber_HP_yield;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Treiber_HP_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::allocator< std::allocator< bool * > >
            >::type
        > Treiber_HP_pause_alloc;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::allocator< std::allocator< unsigned long > >
            >::type
        > Treiber_HP_pause_alloc_relaxed;
    }}

    TEST(Treiber_HP)
    TEST(Treiber_HP_yield)
    TEST(Treiber_HP_pause_alloc)

    TEST(Treiber_HP_relaxed)
    TEST(Treiber_HP_yield_relaxed)
    TEST(Treiber_HP_pause_alloc_relaxed)
}
CPPUNIT_TEST_SUITE_REGISTRATION(stack::TestStack);
