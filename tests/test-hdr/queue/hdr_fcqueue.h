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

#ifndef CDSTEST_HDR_FCQUEUE_H
#define CDSTEST_HDR_FCQUEUE_H

#include "cppunit/cppunit_proxy.h"
#include <cds/details/defs.h>

namespace queue {

    //
    // Test queue operation in single thread mode
    //
    class HdrFCQueue: public CppUnitMini::TestCase
    {
    protected:
        template <class Queue>
        void testNoItemCounter()
        {
            Queue   q;
            test_with( q );
            test_emplace( q );
        }

        template <class Queue>
        void test_with( Queue& q )
        {
            int     it;
            int     nPrev;

            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.empty() );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 );
#endif
                CPPUNIT_ASSERT( q.enqueue( 1 ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push( 10 ) );
                CPPUNIT_ASSERT( !q.empty() );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 )     ;   // no queue's item counter!
#endif

                it = -1;
                CPPUNIT_ASSERT( q.pop( it ) );
                CPPUNIT_ASSERT( it == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.dequeue( it ) );
                CPPUNIT_ASSERT( it == 10 );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 );
#endif
                CPPUNIT_ASSERT( q.empty() );
                it += 2009;
                nPrev = it;
                CPPUNIT_ASSERT( !q.dequeue( it ) );
                CPPUNIT_ASSERT( it == nPrev )       ;   // it must not be changed!
            }
        }

        template <class Queue>
        void test_emplace( Queue& q )
        {
            int     it;
            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.emplace( static_cast<int>( i * 42 )) );
                CPPUNIT_ASSERT( !q.empty() );
                it = -1;
                CPPUNIT_ASSERT( q.pop( it ));
                CPPUNIT_ASSERT( it == static_cast<int>( i * 42 ));
                CPPUNIT_ASSERT( q.empty() );
            }
        }

        template <class Queue>
        void testWithItemCounter()
        {
            Queue   q;
            test_ic_with( q );
            test_emplace_ic( q );
        }

        template <class Queue>
        void testFCQueue()
        {
            Queue   q;
            test_ic_with( q );
        }

        template <class Queue>
        void test_ic_with( Queue& q )
        {
            int     it;
            int     nPrev;

            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
                CPPUNIT_ASSERT( q.enqueue( 1 ) );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push( 10 ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 2 );

                it = -1;
                CPPUNIT_ASSERT( q.pop( it ) );
                CPPUNIT_ASSERT( it == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( q.dequeue( it ) );
                CPPUNIT_ASSERT( it == 10 );
                CPPUNIT_ASSERT( q.size() == 0 );
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
                it += 2009;
                nPrev = it;
                CPPUNIT_ASSERT( !q.dequeue( it ) );
                CPPUNIT_ASSERT( it == nPrev )       ;   // it must not be changed!

                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
            }
        }

        template <class Queue>
        void test_emplace_ic( Queue& q )
        {
            int     it = 0;
            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.emplace( (int) i * 10 ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( q.pop( it ));
                CPPUNIT_ASSERT( it == (int) i * 10 );
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
            }
        }

    public:
        void FCQueue_deque();
        void FCQueue_deque_elimination();
        void FCQueue_deque_mutex();
        void FCQueue_deque_stat();
        void FCQueue_list();
        void FCQueue_list_elimination();
        void FCQueue_list_mutex();
        void FCQueue_list_stat();

        CPPUNIT_TEST_SUITE(HdrFCQueue)
            CPPUNIT_TEST(FCQueue_deque)
            CPPUNIT_TEST(FCQueue_deque_elimination)
            CPPUNIT_TEST(FCQueue_deque_mutex)
            CPPUNIT_TEST(FCQueue_deque_stat)
            CPPUNIT_TEST(FCQueue_list)
            CPPUNIT_TEST(FCQueue_list_elimination)
            CPPUNIT_TEST(FCQueue_list_mutex)
            CPPUNIT_TEST(FCQueue_list_stat)
        CPPUNIT_TEST_SUITE_END();

    };
} // namespace queue

#endif // #ifndef CDSTEST_HDR_FCQUEUE_H
