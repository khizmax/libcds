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

#ifndef CDSUNIT_QUEUE_TEST_INTRUSIVE_SPECULATIVE_PAIRING_QUEUE_H
#define CDSUNIT_QUEUE_TEST_INTRUSIVE_SPECULATIVE_PAIRING_QUEUE_H

#include <cds_test/check_size.h>

namespace cds_test {

    class intrusive_speculative_pairing_queue : public ::testing::Test
    {
    protected:
        template <typename Base>
        struct base_hook_item : public Base
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount( 0 )
            {}

            base_hook_item( base_hook_item const& s)
                : nVal( s.nVal )
                , nDisposeCount( s.nDisposeCount )
            {}
        };

        template <typename Member>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            Member hMember;

            member_hook_item()
                : nDisposeCount( 0 )
            {}

            member_hook_item( member_hook_item const& s )
                : nVal( s.nVal )
                , nDisposeCount( s.nDisposeCount )
            {}
        };

        struct mock_disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount;
            }
        };

        template <typename Queue, typename Data>
        void test( Queue& q, Data& arr )
        {
            typedef typename Queue::value_type value_type;
            size_t nSize = arr.size();

            value_type * pv;
            for ( size_t i = 0; i < nSize; ++i )
                arr[i].nVal = static_cast<int>(i);

			q.changeProbStale(0);
			
            pv = q.pop();
            ASSERT_TRUE( pv == nullptr );
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );

            pv = q.dequeue();
            ASSERT_TRUE( pv == nullptr );
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );
			
            // push/pop test
			//------------------------------------
            for ( size_t i = 0; i < nSize; ++i ) {
                if ( i & 1 )
                    q.push( arr[i] );
                else
					q.enqueue( arr[i] );
                ASSERT_FALSE( q.empty());
                ASSERT_CONTAINER_SIZE( q, i + 1 );
            }
			
            for ( size_t i = 0; i <  nSize; ++i ) {
                ASSERT_FALSE( q.empty());
                ASSERT_CONTAINER_SIZE( q,  nSize - i );
                if ( i & 1 )
                    pv = q.pop();
                else
                    pv = q.dequeue();
                ASSERT_FALSE( pv == nullptr );
                ASSERT_EQ( pv->nVal, static_cast<int>(i));
            }
            ASSERT_TRUE( q.empty());
            ASSERT_CONTAINER_SIZE( q, 0 );
			
			//make invalid and dispose queue
			q.pop();
            Queue::gc::scan();
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 1 ) << "i=" << i;
            }
            //------------------------------------
			//end push/pop test
            
            //clear test
			//------------------------------------
			for ( size_t i = 0; i < nSize; ++i )
            {
                q.push( arr[i] );
				ASSERT_CONTAINER_SIZE( q, i+1);
			}
			
            ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize);		
            q.clear();
            ASSERT_CONTAINER_SIZE( q, 0 );
            ASSERT_TRUE( q.empty());
			
			Queue::gc::scan();
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 2 ) << "i=" << i;
            }
			//------------------------------------
			//end clear test
			
			//clear stale nodes test
			//------------------------------------
			//q.changeProbStale(100);
			for ( size_t i = 0; i < nSize; ++i )
            {
                q.push( arr[i] );
				ASSERT_CONTAINER_SIZE( q, i+1);
			}
			ASSERT_FALSE( q.empty());
            ASSERT_CONTAINER_SIZE( q, nSize);
			
			
			for ( size_t i = 0; i <  nSize; ++i ) {
                ASSERT_FALSE( q.empty());
                ASSERT_CONTAINER_SIZE( q,  nSize - i );
                if ( i & 1 )
                    pv = q.pop();
                else
                    pv = q.dequeue();
                ASSERT_FALSE( pv == nullptr );
                ASSERT_EQ( pv->nVal, static_cast<int>(i));
            }

			/*
			//Queue::gc::scan();
            for ( size_t i = 0; i < nSize-20; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 3 ) << "i=" << i;
            }
			for ( size_t i = nSize-20; i < nSize; ++i )
			{
				ASSERT_EQ( arr[i].nDisposeCount, 2 ) << "i=" << i;
			}
			//------------------------------------
			//end clear stale nodes test
			*/
			
			//clear queue
			q.clear();
			Queue::gc::scan();
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_EQ( arr[i].nDisposeCount, 3 ) << "i=" << i;
            }
			
        }
    };

} // namespace cds_test

#endif // CDSUNIT_QUEUE_TEST_INTRUSIVE_MSQUEUE_H