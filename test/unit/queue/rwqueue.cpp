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

#include <cds/container/rwqueue.h>

namespace {
    namespace cc = cds::container;

    class RWQueue: public cds_test::generic_queue
    {};

    TEST_F( RWQueue, defaulted )
    {
        typedef cds::container::RWQueue< int > test_queue;

        test_queue q;
        test(q);
    }

    TEST_F( RWQueue, item_counting )
    {
        typedef cds::container::RWQueue< int,
            typename cds::container::rwqueue::make_traits <
                cds::opt::item_counter< cds::atomicity::item_counter >
            > ::type
        > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( RWQueue, mutex )
    {
        struct traits : public cds::container::rwqueue::traits
        {
            typedef std::mutex lock_type;
        };
        typedef cds::container::RWQueue< int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( RWQueue, padding )
    {
        struct traits : public cds::container::rwqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
            enum { padding = 64 };
        };
        typedef cds::container::RWQueue< int, traits > test_queue;

        test_queue q;
        test( q );
    }

    TEST_F( RWQueue, move )
    {
        typedef cds::container::RWQueue< std::string > test_queue;

        test_queue q;
        test_string( q );
    }

    TEST_F( RWQueue, move_item_counting )
    {
        struct traits : public cc::rwqueue::traits
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cds::container::RWQueue< std::string, traits > test_queue;

        test_queue q;
        test_string( q );
    }

} // namespace

