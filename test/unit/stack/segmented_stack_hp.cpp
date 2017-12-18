/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#include "test_segmented_stack.h"

#include <cds/gc/hp.h>
#include <cds/container/segmented_stack.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;


    class SegmentedStack_HP : public cds_test::segmented_stack
    {
    protected:
        static const size_t c_QuasiFactor = 15;
        void SetUp()
        {
            typedef cc::SegmentedStack< gc_type, int > stack_type;

            cds::gc::hp::GarbageCollector::Construct( stack_type::c_nHazardPtrCount, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( SegmentedStack_HP, defaulted )
    {
        typedef cds::container::SegmentedStack< gc_type, int > test_stack;

        test_stack s( c_QuasiFactor );
        ASSERT_EQ( s.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test(s);
    }

    TEST_F( SegmentedStack_HP, mutex )
    {
        struct traits : public cds::container::segmented_stack::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::SegmentedStack< cds::gc::HP, int, traits > test_stack;

        test_stack s( c_QuasiFactor );
        ASSERT_EQ( s.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test( s );
    }

    TEST_F( SegmentedStack_HP, stat )
    {
        struct traits : public
            cds::container::segmented_stack::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::stat < cds::container::segmented_stack::stat<> >
            > ::type
        {};
        typedef cds::container::SegmentedStack< cds::gc::HP, int, traits > test_stack;

        test_stack s( c_QuasiFactor );
        ASSERT_EQ( s.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test( s );
    }

} // namespace

