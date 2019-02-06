// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
			
			//dispose queue and create new
			q.clear();
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
