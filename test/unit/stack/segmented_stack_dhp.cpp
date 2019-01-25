// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_segmented_stack.h"

#include <cds/gc/dhp.h>
#include <cds/container/segmented_stack.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;


    class SegmentedStack_DHP : public cds_test::segmented_stack
    {
    protected:
        static const size_t c_QuasiFactor = 15;
        void SetUp()
        {
            typedef cc::SegmentedStack< gc_type, int > stack_type;

            cds::gc::dhp::smr::construct( stack_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( SegmentedStack_DHP, defaulted )
    {
        typedef cds::container::SegmentedStack< gc_type, int > test_stack;

        test_stack s( c_QuasiFactor );
        ASSERT_EQ( s.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test(s);
    }

    TEST_F( SegmentedStack_DHP, mutex )
    {
        struct traits : public cds::container::segmented_stack::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::SegmentedStack< gc_type, int, traits > test_stack;

        test_stack s( c_QuasiFactor );
        ASSERT_EQ( s.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test( s );
    }

    TEST_F( SegmentedStack_DHP, shuffle )
    {
        struct traits : public cds::container::segmented_stack::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::SegmentedStack< gc_type, int, traits > test_stack;

        test_stack s( c_QuasiFactor );
        ASSERT_EQ( s.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test( s );
    }

    TEST_F( SegmentedStack_DHP, stat )
    {
        struct traits : public
            cds::container::segmented_stack::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::stat < cds::container::segmented_stack::stat<> >
            > ::type
        {};
        typedef cds::container::SegmentedStack< gc_type, int, traits > test_stack;

        test_stack s( c_QuasiFactor );
        ASSERT_EQ( s.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test( s );
    }

} // namespace

