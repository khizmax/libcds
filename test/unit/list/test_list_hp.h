// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_LIST_TEST_LIST_HP_H
#define CDSUNIT_LIST_TEST_LIST_HP_H

#include "test_list.h"

namespace cds_test {

    class list_hp : public list_common
    {
    protected:
        template <typename List>
        void test_hp( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::value_type  value_type;
            typedef typename List::guarded_ptr guarded_ptr;
            value_type arr[nSize];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].nKey = static_cast<int>(i);
                arr[i].nVal = arr[i].nKey * 10;
            }
            shuffle( arr, arr + nSize );

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            guarded_ptr gp;
            size_t nCount = 0;

            // get() test
            for ( auto& i : arr ) {
                gp = l.get( i.nKey );
                EXPECT_TRUE( !gp );
                gp = l.get_with( other_item( i.nKey ), other_less());
                EXPECT_TRUE( !gp );

                EXPECT_TRUE( l.insert( i ));

                gp = l.get( i );
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->nKey, i.nKey );
                EXPECT_EQ( gp->nVal, gp->nKey * 10 );
                gp->nVal = gp->nKey * 5;

                gp = l.get_with( other_item( i.nKey ), other_less());
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->nKey, i.nKey );
                EXPECT_EQ( gp->nVal, gp->nKey * 5 );
                gp->nVal = gp->nKey * 10;

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

                gp = l.get( i );
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->nKey, i.nKey );

                switch ( i.nKey & 3 ) {
                    case 0:
                        gp = l.extract( i );
                        break;
                    case 1:
                        gp = l.extract_with( other_item( i.nKey ), other_less());
                        break;
                    default:
                        gp = l.extract( i.nKey );
                        break;
                }
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->nKey, i.nKey );
                EXPECT_EQ( gp->nVal, gp->nKey * 10 );

                gp = l.get( i.nKey );
                EXPECT_FALSE( gp );

                gp = l.extract( i );
                EXPECT_FALSE( gp );
                gp = l.extract_with( other_item( i.nKey ), other_less());
                EXPECT_FALSE( gp );
                gp = l.extract( i.nKey );
                EXPECT_FALSE( gp );
            }

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );
        }
    };

} // namespace cds_list

#endif // CDSUNIT_LIST_TEST_LIST_HP_H
