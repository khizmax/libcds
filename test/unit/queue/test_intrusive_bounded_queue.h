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
