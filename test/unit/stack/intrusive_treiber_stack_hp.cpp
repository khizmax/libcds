// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_treiber_stack.h"

#include <cds/gc/hp.h>
#include <cds/intrusive/treiber_stack.h>

namespace {

    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveTreiberStack_HP : public cds_test::IntrusiveTreiberStack
    {
    typedef cds_test::IntrusiveTreiberStack base_class;

    protected:
        void SetUp()
        {
            typedef cds::intrusive::TreiberStack< gc_type, base_hook_item<gc_type>> stack_type;

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

    TEST_F( IntrusiveTreiberStack_HP, defaulted )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            base_hook_item<gc_type>
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, base )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            base_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<gc_type>
                    >
                >
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, base_disposer )
    {
        struct traits:
            ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<gc_type> >
                >
                ,ci::opt::disposer< mock_disposer >
            >::type
        {};
        typedef cds::intrusive::TreiberStack<
            gc_type,
            base_hook_item<gc_type>,
            traits
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, member )
    {
        struct traits
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof( member_hook_item<gc_type>, hMember),
                        ci::opt::gc<gc_type>
                    >
                >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack<
            gc_type,
            member_hook_item<gc_type>,
            traits
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, member_disposer )
    {
        struct traits
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof( member_hook_item<gc_type>, hMember),
                        ci::opt::gc<gc_type>
                    >
                >
                ,ci::opt::disposer< mock_disposer >
            >::type
        {};
        typedef cds::intrusive::TreiberStack<
            gc_type,
            member_hook_item<gc_type>,
            traits
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, relaxed )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            base_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, elimination )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            base_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, elimination_base )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            base_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<gc_type>
                    >
                >
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, elimination_base_dynamic )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            base_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<gc_type>
                    >
                >
                ,ci::opt::buffer< ci::opt::v::initialized_dynamic_buffer<void *> >
            >::type
        > stack_type;

        test_dyn<stack_type>( 2 );
    }

    TEST_F( IntrusiveTreiberStack_HP, elimination_base_disposer )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            base_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<gc_type> >
                >
                ,ci::opt::disposer< mock_disposer >
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, elimination_member )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            member_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof( member_hook_item<gc_type>, hMember),
                        ci::opt::gc<gc_type>
                    >
                >
            >::type
        > stack_type;

        test<stack_type>();
    }

    TEST_F( IntrusiveTreiberStack_HP, elimination_member_dynamic )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            member_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof( member_hook_item<gc_type>, hMember),
                        ci::opt::gc<gc_type>
                    >
                >
                ,ci::opt::buffer< ci::opt::v::initialized_dynamic_buffer<void *> >
            >::type
        > stack_type;

        test_dyn<stack_type>( 2 );
    }

    TEST_F( IntrusiveTreiberStack_HP, elimination_member_disposer )
    {
        typedef cds::intrusive::TreiberStack< gc_type,
            member_hook_item<gc_type>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof( member_hook_item<gc_type>, hMember),
                        ci::opt::gc<gc_type>
                    >
                >
                ,ci::opt::buffer< ci::opt::v::initialized_dynamic_buffer<void *> >
                , ci::opt::disposer< mock_disposer >
            >::type
        > stack_type;

        test_dyn<stack_type>( 2 );
    }

} // namespace

