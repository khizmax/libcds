// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_LIST_TEST_LIST_RCU_H
#define CDSUNIT_LIST_TEST_LIST_RCU_H

#include "test_list.h"

namespace cds_test {

    class list_rcu : public list_common
    {
    protected:
        template <typename List>
        void test_rcu( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::value_type value_type;
            value_type arr[nSize];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].nKey = static_cast<int>(i);
                arr[i].nVal = arr[i].nKey * 10;
            }
            shuffle( arr, arr + nSize );

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            typedef typename List::exempt_ptr exempt_ptr;
            typedef typename List::raw_ptr    raw_ptr;
            typedef typename List::rcu_lock   rcu_lock;

            // get() test
            for ( auto const& i : arr ) {
                {
                    rcu_lock lock;
                    raw_ptr rp = l.get( i.nKey );
                    EXPECT_TRUE( !rp );
                    rp = l.get_with( other_item( i.nKey ), other_less());
                    EXPECT_TRUE( !rp );
                }

                EXPECT_TRUE( l.insert( i ));

                {
                    rcu_lock lock;
                    raw_ptr rp = l.get( i.nKey );
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->nKey, i.nKey );
                    EXPECT_EQ( rp->nVal, i.nVal );
                }
                {
                    rcu_lock lock;
                    raw_ptr rp = l.get( i );
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->nKey, i.nKey );
                    EXPECT_EQ( rp->nVal, i.nVal );
                }
                {
                    rcu_lock lock;
                    raw_ptr rp = l.get_with( other_item( i.nKey ), other_less());
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->nKey, i.nKey );
                    EXPECT_EQ( rp->nVal, i.nVal );
                }
            }

            ASSERT_FALSE( l.empty());
            ASSERT_CONTAINER_SIZE( l, nSize );

            // extract()

            exempt_ptr gp;
            if ( List::c_bExtractLockExternal ) {
                for ( auto const& i : arr ) {
                    {
                        rcu_lock lock;
                        if ( i.nKey & 1 )
                            gp = l.extract( i.nKey );
                        else
                            gp = l.extract_with( other_item( i.nKey ), other_less());

                        ASSERT_FALSE( !gp );
                        EXPECT_EQ( gp->nKey, i.nKey );
                    }
                    gp.release();
                    {
                        rcu_lock lock;
                        gp = l.extract( i );
                        EXPECT_TRUE( !gp );
                        gp = l.extract_with( other_item( i.nKey ), other_less());
                        EXPECT_TRUE( !gp );
                    }
                }
            }
            else {
                for ( auto const& i : arr ) {
                    if ( i.nKey & 1 )
                        gp = l.extract( i.nKey );
                    else
                        gp = l.extract_with( other_item( i.nKey ), other_less());

                    ASSERT_FALSE( !gp );
                    EXPECT_EQ( gp->nKey, i.nKey );

                    gp = l.extract( i );
                    EXPECT_TRUE( !gp );
                    gp = l.extract_with( other_item( i.nKey ), other_less());
                    EXPECT_TRUE( !gp );
                }
            }

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );
        }
    };
} // namespace cds_test

#endif // CDSUNIT_LIST_TEST_LIST_RCU_H
