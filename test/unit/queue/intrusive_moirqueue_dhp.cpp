// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_msqueue.h"

#include <cds/gc/dhp.h>
#include <cds/intrusive/moir_queue.h>
#include <vector>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;


    class IntrusiveMoirQueue_DHP : public cds_test::intrusive_msqueue
    {
        typedef cds_test::intrusive_msqueue base_class;

    protected:
        typedef typename base_class::base_hook_item< ci::msqueue::node<gc_type>> base_item_type;
        typedef typename base_class::member_hook_item< ci::msqueue::node<gc_type>> member_item_type;

        void SetUp()
        {
            typedef ci::MoirQueue< gc_type, base_item_type,
                typename ci::msqueue::make_traits<
                    ci::opt::hook< ci::msqueue::base_hook< ci::opt::gc<gc_type>>>
                >::type
            > queue_type;

            cds::gc::dhp::smr::construct( queue_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }

        template <typename V>
        void check_array( V& arr )
        {
            for ( size_t i = 0; i < arr.size() - 1; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 2 );
            }
            ASSERT_EQ( arr.back().nDisposeCount, 1 );
        }
    };

    TEST_F( IntrusiveMoirQueue_DHP, base_hook )
    {
        typedef cds::intrusive::MoirQueue< gc_type, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< mock_disposer >
                ,ci::opt::hook< ci::msqueue::base_hook< ci::opt::gc<gc_type>>>
            >::type
        > test_queue;

        std::vector<base_item_type> arr;
        arr.resize(100);
        {
            test_queue q;
            test(q, arr);
        }
        gc_type::scan();
        check_array( arr );
    }

    TEST_F( IntrusiveMoirQueue_DHP, base_item_counting )
    {
        typedef cds::intrusive::MoirQueue< gc_type, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< mock_disposer >
                , cds::opt::item_counter< cds::atomicity::item_counter >
                , ci::opt::hook< ci::msqueue::base_hook< ci::opt::gc<gc_type>>>
            >::type
        > test_queue;

        std::vector<base_item_type> arr;
        arr.resize(100);
        {
            test_queue q;
            test(q, arr);
        }
        gc_type::scan();
        check_array( arr );
    }

    TEST_F( IntrusiveMoirQueue_DHP, base_stat )
    {
        struct traits : public ci::msqueue::traits
        {
            typedef ci::msqueue::base_hook< ci::opt::gc<gc_type>> hook;
            typedef mock_disposer disposer;
            typedef cds::atomicity::item_counter item_counter;
            typedef ci::msqueue::stat<> stat;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::intrusive::MoirQueue< gc_type, base_item_type, traits > test_queue;

        std::vector<base_item_type> arr;
        arr.resize(100);
        {
            test_queue q;
            test(q, arr);
        }
        gc_type::scan();
        check_array( arr );
    }

    TEST_F( IntrusiveMoirQueue_DHP, member_hook )
    {
        typedef cds::intrusive::MoirQueue< gc_type, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< mock_disposer >
                ,ci::opt::hook< ci::msqueue::member_hook<
                    offsetof( member_item_type, hMember ),
                    ci::opt::gc<gc_type>
                >>
            >::type
        > test_queue;

        std::vector<member_item_type> arr;
        arr.resize( 100 );
        {
            test_queue q;
            test( q, arr );
        }
        gc_type::scan();
        check_array( arr );
    }

    TEST_F( IntrusiveMoirQueue_DHP, member_hook_stat )
    {
        struct traits : public ci::msqueue::traits
        {
            typedef ci::msqueue::member_hook<
                offsetof( member_item_type, hMember ),
                ci::opt::gc<gc_type>
            > hook;
            typedef mock_disposer disposer;
            typedef cds::atomicity::item_counter item_counter;
            typedef ci::msqueue::stat<> stat;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::intrusive::MoirQueue< gc_type, member_item_type, traits > test_queue;

        std::vector<member_item_type> arr;
        arr.resize( 100 );
        {
            test_queue q;
            test( q, arr );
        }
        gc_type::scan();
        check_array( arr );
    }

} // namespace

