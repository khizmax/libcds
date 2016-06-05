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

#include "test_treiber_stack.h"

#include <cds/gc/dhp.h>
#include <cds/container/treiber_stack.h>

namespace {

    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class TreiberStack_DHP : public cds_test::TreiberStack
    {
        typedef cds_test::TreiberStack base_class;

    protected:
        void SetUp()
        {
            typedef cc::TreiberStack< gc_type, int > stack_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, stack_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown() 
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }

        template <typename Stack>
        void test()
        {
            Stack stack;
            base_class::test( stack );
        }

        template <typename Stack>
        void test_dyn( size_t elimination_size )
        {
            Stack stack( elimination_size );
            base_class::test( stack );
        }
    };

    TEST_F( TreiberStack_DHP, defaulted )
    {
        typedef cc::TreiberStack< gc_type, int > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_DHP, backoff )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_DHP, alloc )
    {
        // allocator must be rebinded for real value type
        struct foo
        {
            size_t arr[ 1024 * 1024 ];
            size_t a2[1024 * 1024];
            size_t a3[1024 * 1024];
            size_t a4[1024 * 1024];
        };

        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::allocator< std::allocator< foo >>
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_DHP, elimination )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_DHP, elimination_backoff )
    {
        struct traits : public cc::treiber_stack::traits
        {
            enum {
                enable_elimination = true
            };
            typedef cds::backoff::pause back_off;
        };
        typedef cc::TreiberStack< gc_type, int, traits > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_DHP, elimination_dynamic )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer<void *> >
            >::type
        > stack_type;

        test_dyn<stack_type>( 4 );
    }

    TEST_F( TreiberStack_DHP, elimination_stat )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::stat< cc::treiber_stack::stat<> >
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_DHP, elimination_dynamic_backoff )
    {
        struct traits : public cc::treiber_stack::traits
        {
            enum {
                enable_elimination = true
            };
            typedef cds::opt::v::initialized_dynamic_buffer<void *> buffer;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::TreiberStack< gc_type, int, traits > stack_type;

        test_dyn<stack_type>( 2 );
    }

} // namespace
