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

#ifndef CDSUNIT_QUEUE_TEST_SEGMENTED_QUEUE_H
#define CDSUNIT_QUEUE_TEST_SEGMENTED_QUEUE_H

#include <cds_test/check_size.h>

namespace cds_test {

    class segmented_queue : public ::testing::Test
    {
    protected:
        template <typename Queue>
        void test( Queue& q )
        {
            typedef typename Queue::value_type value_type;
            value_type it;

            const size_t nSize = 100;

            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            // enqueue/dequeue
            for ( size_t i = 0; i < nSize; ++i ) {
                it = static_cast<value_type>(i);
                ASSERT_TRUE( q.enqueue( it ));
                ASSERT_CONTAINER_SIZE( q, i + 1 );
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize );

            for ( size_t i = 0; i < nSize; ++i ) {
                it = -1;
                ASSERT_TRUE( q.dequeue( it ));
                ASSERT_CONTAINER_SIZE( q, nSize - i - 1 );

                int nSegment = int( i / q.quasi_factor());
                int nMin = nSegment * int( q.quasi_factor());
                int nMax = nMin + int( q.quasi_factor()) - 1;
                EXPECT_LE( nMin, it );
                EXPECT_LE( it, nMax );
            }
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            // push/pop
            for ( size_t i = 0; i < nSize; ++i ) {
                it = static_cast<value_type>(i);
                ASSERT_TRUE( q.push( it ));
                ASSERT_CONTAINER_SIZE( q, i + 1 );
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize );

            size_t nPushed = nSize;
            size_t nStartSegment = nPushed / q.quasi_factor();
            size_t nOffset = nPushed % q.quasi_factor();
            for ( size_t i = 0; i < nSize; ++i ) {
                it = -1;
                ASSERT_TRUE( q.pop( it ));
                ASSERT_CONTAINER_SIZE( q, nSize - i - 1 );

                int nSegment = static_cast<int>((i + nPushed) / q.quasi_factor() - nStartSegment );
                int nMin = nSegment * static_cast<int>( q.quasi_factor());
                if ( nSegment )
                    nMin -= static_cast<int>( nOffset );
                int nMax = nMin + static_cast<int>( q.quasi_factor()) - 1;
                EXPECT_LE( nMin, it );
                EXPECT_LE( it, nMax );
            }
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            // push/pop with lambda
            for ( size_t i = 0; i < nSize; ++i ) {
                it = static_cast<value_type>(i);
                EXPECT_NE( it, -1 );
                auto f = [&it]( value_type& dest ) { dest = it; it = -1; };
                if ( i & 1 )
                    ASSERT_TRUE( q.enqueue_with( f ));
                else
                    ASSERT_TRUE( q.push_with( f ));
                ASSERT_EQ( it, -1 );
                ASSERT_CONTAINER_SIZE( q, i + 1 );
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize );

            nPushed += nSize;
            nStartSegment = nPushed / q.quasi_factor();
            nOffset = nPushed % q.quasi_factor();
            for ( size_t i = 0; i < nSize; ++i ) {
                it = -1;
                auto f = [&it]( value_type& src ) { it = src; src = -1; };
                if ( i & 1 )
                    ASSERT_TRUE( q.pop_with( f ));
                else
                    ASSERT_TRUE( q.dequeue_with( f ));
                ASSERT_CONTAINER_SIZE( q, nSize - i - 1 );

                int nSegment = static_cast<int>((i + nPushed) / q.quasi_factor() - nStartSegment);
                int nMin = nSegment * static_cast<int>(q.quasi_factor());
                if ( nSegment )
                    nMin -= static_cast<int>(nOffset);
                int nMax = nMin + static_cast<int>(q.quasi_factor()) - 1;
                EXPECT_LE( nMin, it );
                EXPECT_LE( it, nMax );
            }
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            // clear
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.push( static_cast<value_type>(i)));
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize );

            q.clear();
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            // pop from empty queue
            it = nSize * 2;
            ASSERT_FALSE( q.pop( it ));
            ASSERT_EQ( it, static_cast<value_type>( nSize * 2 ));
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            ASSERT_FALSE( q.dequeue( it ));
            ASSERT_EQ( it, static_cast<value_type>( nSize * 2 ));
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );
        }

        template <class Queue>
        void test_string( Queue& q )
        {
            std::string str[3];
            str[0] = "one";
            str[1] = "two";
            str[2] = "three";
            const size_t nSize = sizeof( str ) / sizeof( str[0] );

            // emplace
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( q.emplace( str[i].c_str()));
                ASSERT_CONTAINER_SIZE( q, i + 1 );
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize );

            {
                std::string s;
                auto f = [&s]( std::string& src ) {
                    ASSERT_FALSE( src.empty());
                    s = std::move( src );
                    ASSERT_NE( s, src );
                };
                for ( size_t i = 0; i < nSize; ++i ) {
                    if ( i & 1 )
                        ASSERT_TRUE( q.pop_with( f ));
                    else
                        ASSERT_TRUE( q.dequeue_with( f ));

                    ASSERT_CONTAINER_SIZE( q, nSize - i - 1 );
                    ASSERT_TRUE( s == str[0] || s == str[1] || s == str[2] );
                }
            }
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );


            // move push
            for ( size_t i = 0; i < nSize; ++i ) {
                std::string s = str[i];
                ASSERT_FALSE( s.empty());
                if ( i & 1 )
                    ASSERT_TRUE( q.enqueue( std::move( s )));
                else
                    ASSERT_TRUE( q.push( std::move( s )));
                ASSERT_TRUE( s.empty());
                ASSERT_CONTAINER_SIZE( q, i + 1 );
            }
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize );

            for ( size_t i = 0; i < nSize; ++i ) {
                std::string s;
                ASSERT_TRUE( q.pop( s ));
                ASSERT_CONTAINER_SIZE( q, nSize - i - 1 );
                ASSERT_TRUE( s == str[0] || s == str[1] || s == str[2] );
            }
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );
        }

    };

} // namespace cds_test

#endif // CDSUNIT_QUEUE_TEST_SEGMENTED_QUEUE_H
