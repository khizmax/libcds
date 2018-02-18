// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_segmented_queue.h"

#include <cds/gc/hp.h>
#include <cds/intrusive/segmented_queue.h>
#include <vector>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveSegmentedQueue_HP : public cds_test::intrusive_segmented_queue
    {
        typedef cds_test::intrusive_segmented_queue base_class;

    protected:
        static const size_t c_QuasiFactor = 15;

        void SetUp()
        {
            typedef ci::SegmentedQueue< gc_type, item > queue_type;

            cds::gc::hp::GarbageCollector::Construct( queue_type::c_nHazardPtrCount, 1, 16 );
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

    TEST_F( IntrusiveSegmentedQueue_HP, defaulted )
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
        };
        typedef cds::intrusive::SegmentedQueue< gc_type, item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveSegmentedQueue_HP, mutex )
    {
        struct queue_traits : public
            cds::intrusive::segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::SegmentedQueue< gc_type, item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveSegmentedQueue_HP, shuffle )
    {
        typedef cds::intrusive::SegmentedQueue< gc_type, item,
            cds::intrusive::segmented_queue::make_traits<
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::item_counter< cds::atomicity::item_counter >
                ,cds::opt::permutation_generator< cds::opt::v::random_shuffle_permutation<> >
            >::type
        > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveSegmentedQueue_HP, padding )
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding };
            typedef ci::segmented_queue::stat<> stat;
        };
        typedef cds::intrusive::SegmentedQueue< gc_type, item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveSegmentedQueue_HP, bigdata_padding )
    {
        struct queue_traits : public cds::intrusive::segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only };
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::intrusive::SegmentedQueue< gc_type, big_item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

} // namespace

