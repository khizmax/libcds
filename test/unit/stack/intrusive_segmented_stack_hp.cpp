// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_segmented_stack.h"

#include <cds/gc/hp.h>
#include <cds/intrusive/segmented_stack.h>
#include <vector>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveSegmentedStack_HP : public cds_test::intrusive_segmented_stack
    {
        typedef cds_test::intrusive_segmented_stack base_class;

    protected:
        static const size_t c_QuasiFactor = 16;

        void SetUp()
        {
            typedef ci::SegmentedStack< gc_type, item > stack_type;

            cds::gc::hp::GarbageCollector::Construct( stack_type::c_nHazardPtrCount, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }

        template <typename V>
        void check_array( V& arr )
        {
            for ( size_t i = 0; i < arr.size(); ++i ) {
                EXPECT_EQ( arr[i].nDisposeCount, 2u );
                EXPECT_EQ( arr[i].nDispose2Count, 1u );
            }
        }
    };

    TEST_F( IntrusiveSegmentedStack_HP, defaulted )
    {
        struct stack_traits : public cds::intrusive::segmented_stack::traits
        {
            typedef Disposer disposer;
        };
        typedef cds::intrusive::SegmentedStack< gc_type, item, stack_traits > stack_type;

        std::vector<typename stack_type::value_type> arr;
        {
            stack_type s( c_QuasiFactor );
            test( s, arr );
        }
        stack_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveSegmentedStack_HP, mutex )
    {
        struct stack_traits : public
            cds::intrusive::segmented_stack::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::SegmentedStack< gc_type, item, stack_traits > stack_type;

        std::vector<typename stack_type::value_type> arr;
        {
            stack_type s( c_QuasiFactor );
            test( s, arr );
        }
        stack_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveSegmentedStack_HP, padding )
    {
        struct stack_traits : public cds::intrusive::segmented_stack::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding };
            typedef ci::segmented_stack::stat<> stat;
        };
        typedef cds::intrusive::SegmentedStack< gc_type, item, stack_traits > stack_type;

        std::vector<typename stack_type::value_type> arr;
        {
            stack_type s( c_QuasiFactor );
            test( s, arr );
        }
        stack_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveSegmentedStack_HP, bigdata_padding )
    {
        struct stack_traits : public cds::intrusive::segmented_stack::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only };
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::intrusive::SegmentedStack< gc_type, big_item, stack_traits > stack_type;

        std::vector<typename stack_type::value_type> arr;
        {
            stack_type s( c_QuasiFactor );
            test( s, arr );
        }
        stack_type::gc::force_dispose();
        check_array( arr );
    }

} // namespace

