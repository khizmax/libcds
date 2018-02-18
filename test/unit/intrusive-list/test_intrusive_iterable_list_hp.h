// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CDSUNIT_LIST_TEST_INTRUSIVE_ITERABLE_LIST_HP_H
#define CDSUNIT_LIST_TEST_INTRUSIVE_ITERABLE_LIST_HP_H

#include "test_intrusive_iterable_list.h"

namespace cds_test {

    class intrusive_iterable_list_hp : public intrusive_iterable_list
    {
    protected:
        template <typename List>
        void test_hp( List& l )
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

            typedef typename List::guarded_ptr guarded_ptr;

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            guarded_ptr gp;

            // get() test
            for ( auto& i : arr ) {
                gp = l.get( i.nKey );
                EXPECT_TRUE( !gp );
                gp = l.get_with( other_item( i.nKey ), other_less());
                EXPECT_TRUE( !gp );

                EXPECT_TRUE( l.insert( i ));

                gp = l.get( i.nKey );
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->nKey, i.nKey );
                EXPECT_EQ( gp->nVal, i.nVal );
                gp = l.get_with( other_item( i.nKey ), other_less());
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->nKey, i.nKey );
                EXPECT_EQ( gp->nVal, i.nVal );
            }

            // extract() test
            for ( int i = 0; i < static_cast<int>(nSize); ++i ) {
                if ( i & 1 )
                    gp = l.extract( i );
                else
                    gp = l.extract_with( other_item( i ), other_less());
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->nKey, i );

                gp = l.extract( i );
                EXPECT_TRUE( !gp );
                gp = l.extract_with( other_item( i ), other_less());
                EXPECT_TRUE( !gp );
            }

            ASSERT_TRUE( l.empty());
            ASSERT_CONTAINER_SIZE( l, 0 );

            List::gc::force_dispose();
            for ( auto const& i : arr ) {
                EXPECT_EQ( i.s.nDisposeCount, 1 );
                EXPECT_FALSE( l.contains( i ));
            }
        }
    };

} // namespace cds_test

#endif // CDSUNIT_LIST_TEST_INTRUSIVE_ITERABLE_LIST_HP_H
