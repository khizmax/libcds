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

#ifndef CDSUNIT_SET_TEST_INTRUSIVE_SET_RCU_H
#define CDSUNIT_SET_TEST_INTRUSIVE_SET_RCU_H

#include "test_intrusive_set.h"

// forward declaration
namespace cds { namespace intrusive {}}

namespace cds_test {

    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class intrusive_set_rcu: public intrusive_set
    {
        typedef intrusive_set base_class;

    protected:

        template <class Set>
        void test( Set& s )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            base_class::test( s );

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            typedef typename Set::value_type value_type;

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key)));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end());

            typename Set::exempt_ptr xp;
            typename Set::raw_ptr rp;
            typedef typename Set::rcu_lock rcu_lock;

            // get/extract from empty set
            for ( auto idx : indices ) {
                auto& i = data[idx];

                {
                    rcu_lock l;
                    rp = s.get( i );
                    ASSERT_TRUE( !rp );
                    rp = s.get( i.key());
                    ASSERT_TRUE( !rp );
                    rp = s.get_with( other_item( i.key()), other_less());
                    ASSERT_TRUE( !rp );
                }

                if ( Set::c_bExtractLockExternal ) {
                    rcu_lock l;

                    xp = s.extract( i );
                    ASSERT_TRUE( !xp );
                    xp = s.extract( i.key());
                    ASSERT_TRUE( !xp );
                    xp = s.extract_with( other_item( i.key()), other_less());
                    ASSERT_TRUE( !xp );
                }
                else {
                    xp = s.extract( i );
                    ASSERT_TRUE( !xp );
                    xp = s.extract( i.key());
                    ASSERT_TRUE( !xp );
                    xp = s.extract_with( other_item( i.key()), other_less());
                    ASSERT_TRUE( !xp );
                }
            }

            // fill set
            for ( auto& i : data ) {
                i.nDisposeCount = 0;
                ASSERT_TRUE( s.insert( i ));
            }

            // get/extract
            for ( auto idx : indices ) {
                auto& i = data[idx];

                {
                    rcu_lock l;
                    EXPECT_EQ( i.nFindCount, 0u );
                    rp = s.get( i );
                    ASSERT_FALSE( !rp );
                    ++rp->nFindCount;
                    EXPECT_EQ( i.nFindCount, 1u );

                    rp = s.get( i.key());
                    ASSERT_FALSE( !rp );
                    ++rp->nFindCount;
                    EXPECT_EQ( i.nFindCount, 2u );

                    rp = s.get_with( other_item( i.key()), other_less());
                    ASSERT_FALSE( !rp );
                    ++rp->nFindCount;
                    EXPECT_EQ( i.nFindCount, 3u );
                }

                if ( Set::c_bExtractLockExternal ) {
                    {
                        rcu_lock l;

                        EXPECT_EQ( i.nEraseCount, 0u );
                        switch ( i.key() % 3 ) {
                        case 0:
                            xp = s.extract( i.key());
                            break;
                        case 1:
                            xp = s.extract( i );
                            break;
                        case 2:
                            xp = s.extract_with( other_item( i.key()), other_less());
                            break;
                        }
                        ASSERT_FALSE( !xp );
                        ++xp->nEraseCount;
                    }
                    EXPECT_EQ( i.nEraseCount, 1u );
                    xp.release();

                    {
                        rcu_lock l;

                        xp = s.extract( i );
                        ASSERT_TRUE( !xp );
                        xp = s.extract( i.key());
                        ASSERT_TRUE( !xp );
                        xp = s.extract_with( other_item( i.key()), other_less());
                        ASSERT_TRUE( !xp );
                    }
                }
                else {
                    EXPECT_EQ( i.nEraseCount, 0u );
                    switch ( i.key() % 3 ) {
                    case 0:
                        xp = s.extract( i.key());
                        break;
                    case 1:
                        xp = s.extract( i );
                        break;
                    case 2:
                        xp = s.extract_with( other_item( i.key()), other_less());
                        break;
                    }
                    ASSERT_FALSE( !xp );
                    ++xp->nEraseCount;
                    EXPECT_EQ( i.nEraseCount, 1u );

                    xp = s.extract( i );
                    ASSERT_TRUE( !xp );
                    xp = s.extract( i.key());
                    ASSERT_TRUE( !xp );
                    xp = s.extract_with( other_item( i.key()), other_less());
                    ASSERT_TRUE( !xp );
                }
            }

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            // Force retiring cycle
            Set::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_SET_TEST_INTRUSIVE_SET_RCU_H
