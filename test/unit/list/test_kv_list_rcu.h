// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_LIST_TEST_KV_LIST_RCU_H
#define CDSUNIT_LIST_TEST_KV_LIST_RCU_H

#include "test_kv_list.h"

namespace cds_test {

    class kv_list_rcu : public kv_list_common
    {
    protected:
        template <typename List>
        void test_rcu( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            struct key_val {
                int key;
                int val;
            };
            key_val arr[nSize];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].key = static_cast<int>(i);
                arr[i].val = arr[i].key * 10;
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
                    raw_ptr rp = l.get( i.key );
                    EXPECT_TRUE( !rp );
                    rp = l.get_with( other_key( i.key ), other_less());
                    EXPECT_TRUE( !rp );
                }

                EXPECT_TRUE( l.insert( i.key, i.val ));

                {
                    rcu_lock lock;
                    raw_ptr rp = l.get( i.key );
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->first.nKey, i.key );
                    EXPECT_EQ( rp->second.val, i.val );
                }
                {
                    rcu_lock lock;
                    raw_ptr rp = l.get( key_type( i.key ));
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->first.nKey, i.key );
                    EXPECT_EQ( rp->second.val, i.val );
                }
                {
                    rcu_lock lock;
                    raw_ptr rp = l.get_with( other_key( i.key ), other_less());
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->first.key(), i.key );
                    EXPECT_EQ( rp->second.val, i.val );
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
                        if ( i.key & 1 )
                            gp = l.extract( i.key );
                        else
                            gp = l.extract_with( other_key( i.key ), other_less());

                        ASSERT_FALSE( !gp );
                        EXPECT_EQ( gp->first.key(), i.key );
                    }
                    gp.release();
                    {
                        rcu_lock lock;
                        gp = l.extract( i.key );
                        EXPECT_TRUE( !gp );
                        gp = l.extract_with( other_key( i.key ), other_less());
                        EXPECT_TRUE( !gp );
                    }
                }
            }
            else {
                for ( auto const& i : arr ) {
                    if ( i.key & 1 )
                        gp = l.extract( i.key );
                    else
                        gp = l.extract_with( other_key( i.key ), other_less());

                    ASSERT_FALSE( !gp );
                    EXPECT_EQ( gp->first.key(), i.key );

                    gp = l.extract( i.key );
                    EXPECT_TRUE( !gp );
                    gp = l.extract_with( other_key( i.key ), other_less());
                    EXPECT_TRUE( !gp );
                }
            }

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );
        }
    };
} // namespace cds_test

#endif // CDSUNIT_LIST_TEST_KV_LIST_RCU_H
