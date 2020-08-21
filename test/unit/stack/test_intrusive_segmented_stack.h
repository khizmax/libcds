// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_STACK_TEST_INTRUSIVE_SEGMENTED_STACK_H
#define CDSUNIT_STACK_TEST_INTRUSIVE_SEGMENTED_STACK_H

#include <cds_test/check_size.h>

namespace cds_test {

    class intrusive_segmented_stack : public ::testing::Test
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
                ++(p->nDisposeCount);
            }
        };

        struct Disposer2
        {
            void operator()( item * p )
            {
                ++(p->nDispose2Count);
            }
        };

        template <typename Stack, typename Data>
        void test( Stack& s, Data& val )
        {
            typedef typename Stack::value_type value_type;
            val.resize( 100 );
            for ( size_t i = 0; i < val.size(); ++i )
                val[i].nValue = static_cast<int>( i );

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0u );

            // push
            for ( size_t i = 0; i < val.size(); ++i ) {
				ASSERT_TRUE(s.push( val[i]) );

				ASSERT_CONTAINER_SIZE( s, i + 1 );
			}
            EXPECT_TRUE( !s.empty());

            // pop
            size_t nCount = 0;
            while ( !s.empty()) {
                value_type * pVal;
                pVal = s.pop();

                ASSERT_TRUE( pVal != nullptr );

                ++nCount;
                EXPECT_CONTAINER_SIZE( s, val.size() - nCount );
            }
            EXPECT_EQ( nCount, val.size());
            EXPECT_TRUE( s.empty());
            EXPECT_CONTAINER_SIZE( s, 0u );

            // pop from empty stack
            ASSERT_TRUE( s.pop() == nullptr );
            EXPECT_TRUE( s.empty());
            EXPECT_CONTAINER_SIZE( s, 0u );

            // check that Disposer has not been called
            Stack::gc::force_dispose();
            for ( size_t i = 0; i < val.size(); ++i ) {
                EXPECT_EQ( val[i].nDisposeCount, 0u );
                EXPECT_EQ( val[i].nDispose2Count, 0u );
            }

            // clear
            for ( size_t i = 0; i < val.size(); ++i )
                EXPECT_TRUE( s.push( val[i] ));
            EXPECT_CONTAINER_SIZE( s, val.size());
            EXPECT_TRUE( !s.empty());

            s.clear();
            EXPECT_CONTAINER_SIZE( s, 0u );
            EXPECT_TRUE( s.empty());

            // check if Disposer has been called
            Stack::gc::force_dispose();
            for ( size_t i = 0; i < val.size(); ++i ) {
                EXPECT_EQ( val[i].nDisposeCount, 1u );
                EXPECT_EQ( val[i].nDispose2Count, 0u );
            }

            // clear_with
            for ( size_t i = 0; i < val.size(); ++i )
                EXPECT_TRUE( s.push( val[i] ));
            EXPECT_CONTAINER_SIZE( s, val.size());
            EXPECT_TRUE( !s.empty());

            s.clear_with( Disposer2() );
            EXPECT_CONTAINER_SIZE( s, 0u );
            EXPECT_TRUE( s.empty());

            // check if Disposer has been called
            Stack::gc::force_dispose();
            for ( size_t i = 0; i < val.size(); ++i ) {
                EXPECT_EQ( val[i].nDisposeCount, 1u );
                EXPECT_EQ( val[i].nDispose2Count, 1u );
            }

            // check clear on destruct
            for ( size_t i = 0; i < val.size(); ++i )
                EXPECT_TRUE( s.push( val[i] ));
            EXPECT_CONTAINER_SIZE( s, val.size());
            EXPECT_TRUE( !s.empty());
        }
    };

} // namespace cds_test

#endif // CDSUNIT_STACK_TEST_INTRUSIVE_SEGMENTED_STACK_H
