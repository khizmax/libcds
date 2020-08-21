// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_STACK_TEST_SEGMENTED_STACK_H
#define CDSUNIT_STACK_TEST_SEGMENTED_STACK_H

#include <cds_test/check_size.h>

namespace cds_test {

    class segmented_stack : public ::testing::Test
    {
    protected:
        template <typename Stack>
        void test( Stack& s )
        {
            typedef typename Stack::value_type value_type;
            value_type it;

            const size_t nSize = 100;

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );
            // push/pop
            for ( size_t i = 0; i < nSize; ++i ) {
                it = static_cast<value_type>(i);
                ASSERT_TRUE( s.push( it ));
                ASSERT_CONTAINER_SIZE( s, i + 1 );
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSize );

            for ( size_t i = 0; i < nSize; ++i ) {
                it = -1;
                ASSERT_TRUE( s.pop(it));
                ASSERT_CONTAINER_SIZE( s, nSize - i - 1 );
            }
            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            // clear
            for ( size_t i = 0; i < nSize; ++i ) {
                ASSERT_TRUE( s.push( static_cast<value_type>(i)));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSize );
            s.clear();
            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            // pop from empty stack
            it = nSize * 2;
            ASSERT_FALSE( s.pop( it ));
            ASSERT_EQ( it, static_cast<value_type>( nSize * 2 ));
            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );
        }
    };

} // namespace cds_test

#endif // CDSUNIT_STACK_TEST_SEGMENTED_STACK_H
