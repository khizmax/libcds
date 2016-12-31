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

#include "test_generic_queue.h"

#include <cds/gc/dhp.h>
#include <cds/container/msqueue.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;


    class MSQueue_DHP : public cds_test::generic_queue
    {
    protected:
        void SetUp()
        {
            typedef cc::MSQueue< gc_type, int > queue_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, queue_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( MSQueue_DHP, defaulted )
    {
        typedef cds::container::MSQueue< gc_type, int > test_queue;

        test_queue q;
        test(q);
    }

    TEST_F( MSQueue_DHP, item_counting )
    {
        typedef cds::container::MSQueue < gc_type, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter < cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( MSQueue_DHP, relaxed )
    {
        typedef cds::container::MSQueue < gc_type, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
                , cds::opt::memory_model < cds::opt::v::relaxed_ordering >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( MSQueue_DHP, aligned )
    {
        typedef cds::container::MSQueue < gc_type, int,
            typename cds::container::msqueue::make_traits <
                cds::opt::memory_model< cds::opt::v::relaxed_ordering>
                , cds::opt::padding < 32 >
            >::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( MSQueue_DHP, seq_cst )
    {
        struct traits : public cc::msqueue::traits
        {
            typedef cds::opt::v::sequential_consistent memory_model;
            typedef cds::atomicity::item_counter item_counter;
            enum { padding = 64 };
        };
        typedef cds::container::MSQueue < gc_type, int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( MSQueue_DHP, move )
    {
        typedef cds::container::MSQueue< gc_type, std::string > test_queue;

        test_queue q;
        test_string( q );
    }

    TEST_F( MSQueue_DHP, move_item_counting )
    {
        struct traits : public cc::msqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::MSQueue< gc_type, std::string, traits > test_queue;

        test_queue q;
        test_string( q );
    }

} // namespace

