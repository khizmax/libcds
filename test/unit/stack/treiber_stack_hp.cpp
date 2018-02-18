// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_treiber_stack.h"

#include <cds/gc/hp.h>
#include <cds/container/treiber_stack.h>

namespace {

    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class TreiberStack_HP : public cds_test::TreiberStack
    {
        typedef cds_test::TreiberStack base_class;

    protected:
        void SetUp()
        {
            typedef cc::TreiberStack< gc_type, int > stack_type;

            cds::gc::hp::GarbageCollector::Construct( stack_type::c_nHazardPtrCount, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
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

    TEST_F( TreiberStack_HP, defaulted )
    {
        typedef cc::TreiberStack< gc_type, int > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_HP, backoff )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_HP, alloc )
    {
        // allocator must be rebinded for real value type
        struct foo;

        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::allocator< std::allocator< foo >>
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_HP, elimination )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_HP, elimination_backoff )
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

    TEST_F( TreiberStack_HP, elimination_dynamic )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer<void *> >
            >::type
        > stack_type;

        test_dyn<stack_type>( 4 );
    }

    TEST_F( TreiberStack_HP, elimination_stat )
    {
        typedef cc::TreiberStack< gc_type, int
            , typename cc::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::stat< cc::treiber_stack::stat<> >
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( TreiberStack_HP, elimination_dynamic_backoff )
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
