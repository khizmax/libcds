// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TEST_INTRUSIVE_MICHAEL_ITERABLE_HP_H
#define CDSUNIT_SET_TEST_INTRUSIVE_MICHAEL_ITERABLE_HP_H

#include "test_intrusive_michael_iterable.h"

// forward declaration
namespace cds { namespace intrusive {}}

namespace cds_test {

    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class intrusive_set_hp: public intrusive_set
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

            typename Set::guarded_ptr gp;

            // get/extract from empty set
            for ( auto idx : indices ) {
                auto& i = data[idx];

                gp = s.get( i );
                ASSERT_TRUE( !gp );
                gp = s.get( i.key());
                ASSERT_TRUE( !gp );
                gp = s.get_with( other_item( i.key()), other_less());
                ASSERT_TRUE( !gp );

                gp = s.extract( i );
                ASSERT_TRUE( !gp );
                gp = s.extract( i.key());
                ASSERT_TRUE( !gp );
                gp = s.extract_with( other_item( i.key()), other_less());
                ASSERT_TRUE( !gp );
            }

            // fill set
            for ( auto& i : data ) {
                i.nDisposeCount = 0;
                ASSERT_TRUE( s.insert( i ));
            }

            // get/extract
            for ( auto idx : indices ) {
                auto& i = data[idx];

                EXPECT_EQ( i.nFindCount, 0u );
                gp = s.get( i );
                ASSERT_FALSE( !gp );
                ++gp->nFindCount;
                EXPECT_EQ( i.nFindCount, 1u );

                gp = s.get( i.key());
                ASSERT_FALSE( !gp );
                ++gp->nFindCount;
                EXPECT_EQ( i.nFindCount, 2u );

                gp = s.get_with( other_item( i.key()), other_less());
                ASSERT_FALSE( !gp );
                ++gp->nFindCount;
                EXPECT_EQ( i.nFindCount, 3u );

                EXPECT_EQ( i.nEraseCount, 0u );
                switch ( i.key() % 3 ) {
                case 0:
                    gp = s.extract( i.key());
                    break;
                case 1:
                    gp = s.extract( i );
                    break;
                case 2:
                    gp = s.extract_with( other_item( i.key()), other_less());
                    break;
                }
                ASSERT_FALSE( !gp );
                ++gp->nEraseCount;
                EXPECT_EQ( i.nEraseCount, 1u );

                gp = s.extract( i );
                ASSERT_TRUE( !gp );
                gp = s.extract( i.key());
                ASSERT_TRUE( !gp );
                gp = s.extract_with( other_item( i.key()), other_less());
                ASSERT_TRUE( !gp );
            }

            gp.release();

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            // Force retiring cycle
            Set::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }

            // erase_at() test
            for ( auto& i : data ) {
                i.nDisposeCount = 0;
                ASSERT_TRUE( s.insert( i ));
            }

            for ( auto it = s.begin(); it != s.end(); ++it ) {
                EXPECT_TRUE( s.erase_at( it ));
                EXPECT_FALSE( s.erase_at( it ));
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

#endif // #ifndef CDSUNIT_SET_TEST_INTRUSIVE_MICHAEL_ITERABLE_HP_H
