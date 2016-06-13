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

#include "test_segmented_queue.h"

#include <cds/gc/dhp.h>
#include <cds/container/segmented_queue.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;


    class SegmentedQueue_DHP : public cds_test::segmented_queue
    {
    protected:
        static const size_t c_QuasiFactor = 15;
        void SetUp()
        {
            typedef cc::SegmentedQueue< gc_type, int > queue_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, queue_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( SegmentedQueue_DHP, defaulted )
    {
        typedef cds::container::SegmentedQueue< gc_type, int > test_queue;

        test_queue q( c_QuasiFactor );
        ASSERT_EQ( q.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ));
        test(q);
    }

    TEST_F( SegmentedQueue_DHP, mutex )
    {
        struct traits : public cds::container::segmented_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
        };
        typedef cds::container::SegmentedQueue< gc_type, int, traits > test_queue;

        test_queue q( c_QuasiFactor );
        ASSERT_EQ( q.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ) );
        test( q );
    }

    TEST_F( SegmentedQueue_DHP, shuffle )
    {
        struct traits : public cds::container::segmented_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
        };
        typedef cds::container::SegmentedQueue< gc_type, int, traits > test_queue;

        test_queue q( c_QuasiFactor );
        ASSERT_EQ( q.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ) );
        test( q );
    }

    TEST_F( SegmentedQueue_DHP, stat )
    {
        struct traits : public
            cds::container::segmented_queue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::permutation_generator< cds::opt::v::random_permutation<> >
                , cds::opt::stat < cds::container::segmented_queue::stat<> >
            > ::type
        {};
        typedef cds::container::SegmentedQueue< gc_type, int, traits > test_queue;

        test_queue q( c_QuasiFactor );
        ASSERT_EQ( q.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ) );
        test( q );
    }

    TEST_F( SegmentedQueue_DHP, move )
    {
        typedef cds::container::SegmentedQueue< gc_type, std::string > test_queue;

        test_queue q( c_QuasiFactor );
        ASSERT_EQ( q.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ) );
        test_string( q );
    }

    TEST_F( SegmentedQueue_DHP, move_item_counting )
    {
        struct traits : public cds::container::segmented_queue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::SegmentedQueue< gc_type, std::string, traits > test_queue;

        test_queue q( c_QuasiFactor );
        ASSERT_EQ( q.quasi_factor(), cds::beans::ceil2( c_QuasiFactor ) );
        test_string( q );
    }

} // namespace

