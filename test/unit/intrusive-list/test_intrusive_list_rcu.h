/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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
#ifndef CDSUNIT_LIST_TEST_INTRUSIVE_LIST_RCU_H
#define CDSUNIT_LIST_TEST_INTRUSIVE_LIST_RCU_H

#include "test_intrusive_list.h"

namespace cds_test {

    class intrusive_list_rcu : public intrusive_list_common
    {
    protected:
        template <typename List>
        void test_rcu( List& l )
        {
            // Precondition: list is empty
            // Postcondition: list is empty

            static const size_t nSize = 20;
            typedef typename List::value_type value_type;
            value_type arr[ nSize ];

            for ( size_t i = 0; i < nSize; ++i ) {
                arr[i].nKey = static_cast<int>( i );
                arr[i].nVal = arr[i].nKey * 10;
            }
            shuffle( arr, arr + nSize );

            typedef typename List::exempt_ptr exempt_ptr;
            typedef typename List::raw_ptr    raw_ptr;
            typedef typename List::rcu_lock   rcu_lock;

            ASSERT_TRUE( l.empty() );
            ASSERT_CONTAINER_SIZE( l, 0 );

            // get() test
            for ( auto& i : arr ) {
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
                    raw_ptr rp = l.get_with( other_item( i.nKey ), other_less() );
                    ASSERT_FALSE( !rp );
                    EXPECT_EQ( rp->nKey, i.nKey );
                    EXPECT_EQ( rp->nVal, i.nVal );
                }
            }

            exempt_ptr gp;

            // extract() test
            if ( List::c_bExtractLockExternal ) {
                for ( int i = 0; i < static_cast<int>(nSize); ++i ) {
                    {
                        rcu_lock lock;

                        if ( i & 1 )
                            gp = l.extract( i );
                        else
                            gp = l.extract_with( other_item(i), other_less());
                        ASSERT_FALSE( !gp );
                        EXPECT_EQ( gp->nKey, i );
                    }
                    gp.release();
                    {
                        rcu_lock lock;

                        gp = l.extract( i );
                        EXPECT_TRUE( !gp );
                        gp = l.extract_with( other_item( i ), other_less() );
                        EXPECT_TRUE( !gp );
                    }
                }
            }
            else {
                for ( int i = 0; i < static_cast<int>(nSize); ++i ) {
                    if ( i & 1 )
                        gp = l.extract( i );
                    else
                        gp = l.extract_with( other_item( i ), other_less() );
                    ASSERT_FALSE( !gp );
                    EXPECT_EQ( gp->nKey, i );

                    gp = l.extract( i );
                    EXPECT_TRUE( !gp );
                    gp = l.extract_with( other_item( i ), other_less() );
                    EXPECT_TRUE( !gp );
                }
            }

            ASSERT_TRUE( l.empty() );
            ASSERT_CONTAINER_SIZE( l, 0 );

            List::gc::force_dispose();
            for ( auto const& i : arr ) {
                EXPECT_EQ( i.s.nDisposeCount, 1 );
                EXPECT_FALSE( l.contains( i ) );
            }
        }
    };

} // namespace cds_test

#endif // CDSUNIT_LIST_TEST_INTRUSIVE_LIST_RCU_H 
