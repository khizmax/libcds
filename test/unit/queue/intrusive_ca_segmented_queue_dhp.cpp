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

#include "test_intrusive_segmented_queue.h"

#include <cds/gc/dhp.h>
#include <cds/intrusive/ca_segmented_queue.h>
#include <vector>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveCASegmentedQueue_DHP : public cds_test::intrusive_segmented_queue
    {
        typedef cds_test::intrusive_segmented_queue base_class;

    protected:
        static const size_t c_QuasiFactor = 15;

        void SetUp()
        {
            typedef ci::CASegmentedQueue< gc_type, item > queue_type;

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
            for ( size_t i = 0; i < arr.size(); ++i ) {
                EXPECT_EQ( arr[i].nDisposeCount, 2u );
                EXPECT_EQ( arr[i].nDispose2Count, 1u );
            }
        }
    };

    TEST_F( IntrusiveCASegmentedQueue_DHP, defaulted )
    {
        struct queue_traits : public cds::intrusive::ca_segmented_queue::traits
        {
            typedef Disposer disposer;
        };
        typedef cds::intrusive::CASegmentedQueue< gc_type, item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveCASegmentedQueue_DHP, mutex )
    {
        struct queue_traits : public
            cds::intrusive::ca_segmented_queue::make_traits <
                cds::intrusive::opt::disposer< Disposer >
                ,cds::opt::lock_type < std::mutex >
            > ::type
        {};
        typedef cds::intrusive::CASegmentedQueue< gc_type, item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveCASegmentedQueue_DHP, padding )
    {
        struct queue_traits : public cds::intrusive::ca_segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding };
            typedef ci::ca_segmented_queue::stat<> stat;
        };
        typedef cds::intrusive::CASegmentedQueue< gc_type, item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

    TEST_F( IntrusiveCASegmentedQueue_DHP, bigdata_padding )
    {
        struct queue_traits : public cds::intrusive::ca_segmented_queue::traits
        {
            typedef Disposer disposer;
            enum { padding = cds::opt::cache_line_padding | cds::opt::padding_tiny_data_only };
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cds::intrusive::CASegmentedQueue< gc_type, big_item, queue_traits > queue_type;

        std::vector<typename queue_type::value_type> arr;
        {
            queue_type q( c_QuasiFactor );
            test( q, arr );
        }
        queue_type::gc::force_dispose();
        check_array( arr );
    }

} // namespace

