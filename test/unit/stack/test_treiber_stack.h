// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_STACK_TREIBER_STACK_H
#define CDSUNIT_STACK_TREIBER_STACK_H

#include <cds_test/ext_gtest.h>

namespace cds_test {

    class TreiberStack : public ::testing::Test
    {
    protected:
        template <class Stack>
        void test( Stack& stack )
        {
            typedef typename Stack::value_type  value_type;
            value_type v;

            ASSERT_TRUE( stack.empty());

            ASSERT_TRUE( stack.push( 1 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.push( 2 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.push( 3 ));
            ASSERT_TRUE( !stack.empty());

            ASSERT_TRUE( stack.pop( v ));
            ASSERT_EQ( v, 3 );
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.pop_with( [&v]( value_type& src ) { v = src; } ));
            ASSERT_EQ( v, 2 );
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.pop( v ));
            ASSERT_EQ( v, 1 );
            ASSERT_TRUE( stack.empty());
            v = 1000;
            ASSERT_TRUE( !stack.pop( v ));
            ASSERT_EQ( v, 1000 );
            ASSERT_TRUE( stack.empty());

            ASSERT_TRUE( stack.push( 10 ));
            ASSERT_TRUE( stack.push( 20 ));
            ASSERT_TRUE( stack.push( 30 ));
            ASSERT_TRUE( !stack.empty());

            ASSERT_TRUE( stack.emplace( 100 ));
            ASSERT_TRUE( stack.pop( v ));
            ASSERT_EQ( v, 100 );

            stack.clear();
            ASSERT_TRUE( stack.empty());

            Stack::gc::scan();
        }
    };
} // namespace cds_test

#endif // CDSUNIT_STACK_TREIBER_STACK_H
