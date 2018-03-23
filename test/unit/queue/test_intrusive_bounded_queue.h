// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_QUEUE_TEST_INTRUSIVE_BOUNDED_QUEUE_H
#define CDSUNIT_QUEUE_TEST_INTRUSIVE_BOUNDED_QUEUE_H

#include <cds_test/check_size.h>
#include <vector>

namespace cds_test {

    class intrusive_bounded_queue : public ::testing::Test
    {
    protected:
        struct item {
            int nVal;
            int nDisposeCount;

            item()
                : nDisposeCount( 0 )
            {}
        };

    protected:
        template <typename Queue>
        void test( Queue& q )
        {
            typedef typename Queue::value_type value_type;
            value_type it;

            const size_t nSize = q.capacity();

            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            std::vector< value_type > arr;
            arr.resize( nSize );
            for ( size_t i = 0; i < nSize; ++i )
                arr[i].nVal = static_cast<int>(i);

            // push
            for ( auto& i : arr ) {
                if ( i.nVal & 1 ) {
                    ASSERT_TRUE( q.push( i ));
                }
                else {
                    ASSERT_TRUE( q.enqueue( i ));
                }
                ASSERT_CONTAINER_SIZE( q, i.nVal + 1 );
                ASSERT_FALSE( q.empty());
            }

            ASSERT_CONTAINER_SIZE( q, q.capacity());

            // pop
            int val = 0;
            while ( !q.empty()) {
                value_type * v;
                if ( val & 1 )
                    v = q.pop();
                else
                    v = q.dequeue();

                ASSERT_TRUE( v != nullptr );
                ASSERT_EQ( v->nVal, val );
                ++val;
                ASSERT_CONTAINER_SIZE( q, nSize - static_cast<size_t>( val ));
            }
            ASSERT_EQ( val, static_cast<int>( nSize ));

            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            // pop from empty queue
            {
                value_type * v = q.pop();
                ASSERT_TRUE( v == nullptr );
                ASSERT_TRUE( q.empty());
                ASSERT_CONTAINER_SIZE( q, 0 );
            }

            // clear
            for ( auto& i : arr ) {
                ASSERT_TRUE( q.push( i ));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, q.capacity());
            q.clear();
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            if ( std::is_same<typename Queue::disposer, cds::intrusive::opt::v::empty_disposer>::value ) {
                // no disposer
                for ( auto& i : arr ) {
                    ASSERT_EQ( i.nDisposeCount, 0 );
                }
            }
            else {
                // check the disposer has been called
                for ( auto& i : arr ) {
                    ASSERT_EQ( i.nDisposeCount, 1 );
                }
            }

            // clear with disposer
            for ( auto& i : arr ) {
                ASSERT_TRUE( q.push( i ));
                i.nDisposeCount = 0;
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, q.capacity());
            q.clear( []( value_type * p ) { p->nDisposeCount = p->nVal + 1; } );
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );
            // check the disposer has not been called
            for ( auto& i : arr ) {
                ASSERT_EQ( i.nDisposeCount, i.nVal + 1 );
            }
        }
    };

} // namespace cds_test

#endif // CDSUNIT_QUEUE_TEST_INTRUSIVE_BOUNDED_QUEUE_H
