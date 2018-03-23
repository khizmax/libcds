// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
