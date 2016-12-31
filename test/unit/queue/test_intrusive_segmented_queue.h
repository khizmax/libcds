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

#ifndef CDSUNIT_QUEUE_TEST_INTRUSIVE_SEGMENTED_QUEUE_H
#define CDSUNIT_QUEUE_TEST_INTRUSIVE_SEGMENTED_QUEUE_H

#include <cds_test/check_size.h>

namespace cds_test {

    class intrusive_segmented_queue : public ::testing::Test
    {
    protected:
        struct item {
            int  nValue;

            size_t  nDisposeCount;
            size_t  nDispose2Count;

            item()
                : nValue( 0 )
                , nDisposeCount( 0 )
                , nDispose2Count( 0 )
            {}

            item( int nVal )
                : nValue( nVal )
                , nDisposeCount( 0 )
                , nDispose2Count( 0 )
            {}
        };

        struct big_item : public item
        {
            big_item()
            {}

            big_item( int nVal )
                : item( nVal )
            {}

            int arr[80];
        };

        struct Disposer
        {
            void operator()( item * p )
            {
                ++p->nDisposeCount;
            }
        };

        struct Disposer2
        {
            void operator()( item * p )
            {
                ++p->nDispose2Count;
            }
        };

        template <typename Queue, typename Data>
        void test( Queue& q, Data& val )
        {
            typedef typename Queue::value_type value_type;
            val.resize( 100 );
            for ( size_t i = 0; i < val.size(); ++i )
                val[i].nValue = static_cast<int>( i );

            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0u );

            // push/enqueue
            for ( size_t i = 0; i < val.size(); ++i ) {
                if ( i & 1 ) {
                    ASSERT_TRUE( q.push( val[i] ));
                }
                else {
                    ASSERT_TRUE( q.enqueue( val[i] ));
                }

                ASSERT_CONTAINER_SIZE( q, i + 1 );
            }
            EXPECT_TRUE( !q.empty());

            // pop/dequeue
            size_t nCount = 0;
            while ( !q.empty()) {
                value_type * pVal;
                if ( nCount & 1 )
                    pVal = q.pop();
                else
                    pVal = q.dequeue();

                ASSERT_TRUE( pVal != nullptr );

                int nSegment = int( nCount / q.quasi_factor());
                int nMin = nSegment * int( q.quasi_factor());
                int nMax = nMin + int( q.quasi_factor()) - 1;
                EXPECT_TRUE( nMin <= pVal->nValue && pVal->nValue <= nMax ) << nMin << " <= " << pVal->nValue << " <= " << nMax;

                ++nCount;
                EXPECT_CONTAINER_SIZE( q, val.size() - nCount );
            }
            EXPECT_EQ( nCount, val.size());
            EXPECT_TRUE( q.empty());
            EXPECT_CONTAINER_SIZE( q, 0u );

            // pop from empty queue
            ASSERT_TRUE( q.pop() == nullptr );
            EXPECT_TRUE( q.empty());
            EXPECT_CONTAINER_SIZE( q, 0u );

            // check that Disposer has not been called
            Queue::gc::force_dispose();
            for ( size_t i = 0; i < val.size(); ++i ) {
                EXPECT_EQ( val[i].nDisposeCount, 0u );
                EXPECT_EQ( val[i].nDispose2Count, 0u );
            }

            // clear
            for ( size_t i = 0; i < val.size(); ++i )
                EXPECT_TRUE( q.push( val[i] ));
            EXPECT_CONTAINER_SIZE( q, val.size());
            EXPECT_TRUE( !q.empty());

            q.clear();
            EXPECT_CONTAINER_SIZE( q, 0u );
            EXPECT_TRUE( q.empty());

            // check if Disposer has been called
            Queue::gc::force_dispose();
            for ( size_t i = 0; i < val.size(); ++i ) {
                EXPECT_EQ( val[i].nDisposeCount, 1u );
                EXPECT_EQ( val[i].nDispose2Count, 0u );
            }

            // clear_with
            for ( size_t i = 0; i < val.size(); ++i )
                EXPECT_TRUE( q.push( val[i] ));
            EXPECT_CONTAINER_SIZE( q, val.size());
            EXPECT_TRUE( !q.empty());

            q.clear_with( Disposer2());
            EXPECT_CONTAINER_SIZE( q, 0u );
            EXPECT_TRUE( q.empty());

            // check if Disposer has been called
            Queue::gc::force_dispose();
            for ( size_t i = 0; i < val.size(); ++i ) {
                EXPECT_EQ( val[i].nDisposeCount, 1u );
                EXPECT_EQ( val[i].nDispose2Count, 1u );
            }

            // check clear on destruct
            for ( size_t i = 0; i < val.size(); ++i )
                EXPECT_TRUE( q.push( val[i] ));
            EXPECT_CONTAINER_SIZE( q, val.size());
            EXPECT_TRUE( !q.empty());
        }
    };

} // namespace cds_test

#endif // CDSUNIT_QUEUE_TEST_INTRUSIVE_SEGMENTED_QUEUE_H
