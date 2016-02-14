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

            ASSERT_TRUE( l.empty() );
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
                ASSERT_FALSE( l.empty() );
                ASSERT_CONTAINER_SIZE( l, nCount );
            }

            ASSERT_FALSE( l.empty() );
            ASSERT_CONTAINER_SIZE( l, nSize );

            // extract() test
            for ( auto const& i : arr ) {
                ASSERT_FALSE( l.empty() );
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

            ASSERT_TRUE( l.empty() );
            ASSERT_CONTAINER_SIZE( l, 0 );
        }
    };

} // namespace cds_list

#endif // CDSUNIT_LIST_TEST_LIST_HP_H
