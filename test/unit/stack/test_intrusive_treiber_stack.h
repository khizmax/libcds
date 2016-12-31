/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSUNIT_STACK_INTRUSIVE_TREIBER_STACK_H
#define CDSUNIT_STACK_INTRUSIVE_TREIBER_STACK_H

#include <gtest/gtest.h>
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
