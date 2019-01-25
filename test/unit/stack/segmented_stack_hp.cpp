// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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

