// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_STACK_INTRUSIVE_TREIBER_STACK_H
#define CDSUNIT_STACK_INTRUSIVE_TREIBER_STACK_H

#include <cds_test/ext_gtest.h>
#include <cds/intrusive/details/single_link_struct.h>

namespace cds_test {

    class IntrusiveTreiberStack : public ::testing::Test
    {
    protected:
        template <typename GC>
        struct base_hook_item : public cds::intrusive::single_link::node< GC >
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount( 0 )
            {}
        };

        template <typename GC>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            cds::intrusive::single_link::node< GC > hMember;

            member_hook_item()
                : nDisposeCount( 0 )
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

        template <typename Stack>
        void test( Stack& stack )
        {
            typedef typename Stack::value_type  value_type;

            ASSERT_TRUE( stack.empty());

            value_type v1, v2, v3;
            v1.nVal = 1;
            v2.nVal = 2;
            v3.nVal = 3;
            ASSERT_TRUE( stack.push( v1 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.push( v2 ));
            ASSERT_TRUE( !stack.empty());
            ASSERT_TRUE( stack.push( v3 ));
            ASSERT_TRUE( !stack.empty());

            value_type * pv;
            pv = stack.pop();
            ASSERT_NE( pv, nullptr );
            ASSERT_EQ( pv, &v3 );
            ASSERT_EQ( pv->nVal, 3 );
            ASSERT_TRUE( !stack.empty());
            pv = stack.pop();
            ASSERT_NE( pv, nullptr );
            ASSERT_EQ( pv, &v2 );
            ASSERT_EQ( pv->nVal, 2 );
            ASSERT_TRUE( !stack.empty());
            pv = stack.pop();
            ASSERT_NE( pv, nullptr );
            ASSERT_EQ( pv, &v1 );
            ASSERT_EQ( pv->nVal, 1 );
            ASSERT_TRUE( stack.empty());
            pv = stack.pop();
            ASSERT_EQ( pv, nullptr );
            ASSERT_TRUE( stack.empty());

            ASSERT_EQ( v1.nDisposeCount, 0 );
            ASSERT_EQ( v2.nDisposeCount, 0 );
            ASSERT_EQ( v3.nDisposeCount, 0 );

            stack.push( v1 );
            stack.push( v2 );
            stack.push( v3 );

            stack.clear();
            ASSERT_TRUE( stack.empty());

            Stack::gc::scan();
            if ( !std::is_same<typename Stack::disposer, cds::intrusive::opt::v::empty_disposer>::value ) {
                ASSERT_EQ( v1.nDisposeCount, 1 );
                ASSERT_EQ( v2.nDisposeCount, 1 );
                ASSERT_EQ( v3.nDisposeCount, 1 );
            }
        }
    };

} // namespace cds_test

#endif // CDSUNIT_STACK_INTRUSIVE_TREIBER_STACK_H
