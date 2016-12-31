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

#ifndef CDSUNIT_PQUEUE_FCPQUEUE_H
#define CDSUNIT_PQUEUE_FCPQUEUE_H

#include "test_data.h"

namespace cds_test {

    class FCPQueue : public PQueueTest
    {
    protected:
        template <class PQueue>
        void test( PQueue& pq )
        {
            data_array<value_type> arr( PQueueTest::c_nCapacity );
            value_type * pFirst = arr.begin();
            value_type * pLast = arr.end();

            ASSERT_TRUE( pq.empty());
            ASSERT_TRUE( pq.size() == 0 );

            size_t nSize = 0;

            // Push test
            for ( value_type * p = pFirst; p < pLast; ++p ) {
                ASSERT_TRUE( pq.push( *p ));
                ASSERT_TRUE( !pq.empty());
                ASSERT_EQ( pq.size(), ++nSize );
            }

            ASSERT_EQ( pq.size(), static_cast<size_t>(PQueueTest::c_nCapacity ));

            // Pop test
            key_type nPrev = PQueueTest::c_nMinValue + key_type( PQueueTest::c_nCapacity ) - 1;
            value_type kv( 0 );

            ASSERT_TRUE( pq.pop( kv ));
            EXPECT_EQ( kv.k, nPrev );

            ASSERT_EQ( pq.size(), static_cast<size_t>( PQueueTest::c_nCapacity - 1 ));
            ASSERT_TRUE( !pq.empty());

            nSize = pq.size();
            while ( pq.size() > 1 ) {
                ASSERT_TRUE( pq.pop( kv ));
                EXPECT_EQ( kv.k, nPrev - 1 );
                nPrev = kv.k;

                --nSize;
                ASSERT_EQ( pq.size(), nSize );
            }

            ASSERT_TRUE( !pq.empty());
            ASSERT_EQ( pq.size(), 1u );

            ASSERT_TRUE( pq.pop( kv ));
            EXPECT_EQ( kv.k, PQueueTest::c_nMinValue );

            ASSERT_TRUE( pq.empty());
            ASSERT_EQ( pq.size(), 0u );

            // pop from empty pqueue
            kv.k = PQueueTest::c_nCapacity * 2;
            ASSERT_FALSE( pq.pop( kv ));
            EXPECT_EQ( kv.k, PQueueTest::c_nCapacity * 2 );

            // Clear test
            for ( value_type * p = pFirst; p < pLast; ++p )
                ASSERT_TRUE( pq.push( *p ));

            ASSERT_TRUE( !pq.empty());
            ASSERT_EQ( pq.size(), static_cast<size_t>( PQueueTest::c_nCapacity ));

            pq.clear();
            ASSERT_TRUE( pq.empty());
            ASSERT_EQ( pq.size(), 0u );
        }
    };
} // namespace cds_test

#endif // CDSUNIT_PQUEUE_FCPQUEUE_H
