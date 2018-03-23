// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_LIST_TEST_KV_LIST_HP_H
#define CDSUNIT_LIST_TEST_KV_LIST_HP_H

#include "test_kv_list.h"

namespace cds_test {

    class kv_list_hp : public kv_list_common
    {
    protected:
        template <typename List>
        void test_hp( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::guarded_ptr guarded_ptr;

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

            guarded_ptr gp;
            size_t nCount = 0;

            // get() test
            for ( auto& i : arr ) {
                gp = l.get( i.key );
                EXPECT_TRUE( !gp );
                gp = l.get_with( other_key( i.key ), other_less());
                EXPECT_TRUE( !gp );

                EXPECT_TRUE( l.insert( i.key, i.val ));

                gp = l.get( i.key );
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, i.key );
                EXPECT_EQ( gp->second.val, gp->first.nKey * 10 );
                gp->second.val = gp->first.nKey * 5;

                gp = l.get_with( other_key( i.key ), other_less());
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, i.key );
                EXPECT_EQ( gp->second.val, gp->first.nKey * 5 );
                gp->second.val = gp->first.nKey * 10;

                ++nCount;
                ASSERT_FALSE( l.empty());
                ASSERT_CONTAINER_SIZE( l, nCount );
            }

            ASSERT_FALSE( l.empty());
            ASSERT_CONTAINER_SIZE( l, nSize );

            // extract() test
            for ( auto const& i : arr ) {
                ASSERT_FALSE( l.empty());
                ASSERT_CONTAINER_SIZE( l, nCount );
                --nCount;

                gp = l.get( i.key );
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, i.key );

                switch ( i.key & 1 ) {
                    case 0:
                        gp = l.extract( i.key );
                        break;
                    case 1:
                        gp = l.extract_with( other_key( i.key ), other_less());
                        break;
                }
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, i.key );
                EXPECT_EQ( gp->second.val, gp->first.nKey * 10 );

                gp = l.get( i.key );
                EXPECT_FALSE( gp );

                gp = l.extract( i.key );
                EXPECT_FALSE( gp );
                gp = l.extract_with( other_key( i.key ), other_less());
                EXPECT_FALSE( gp );
            }

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );
        }
    };

} // namespace cds_list

#endif // CDSUNIT_LIST_TEST_KV_LIST_HP_H
