// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_HP_H
#define CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_HP_H

#include "test_intrusive_feldman_hashset.h"

namespace cds_test {

    class intrusive_feldman_hashset_hp: public intrusive_feldman_hashset
    {
        typedef intrusive_feldman_hashset base_class;

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
            size_t const nSetSize = std::max( s.head_size() * 2, static_cast<size_t>( 100 ));

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( nSetSize );
            indices.reserve( nSetSize );
            for ( size_t key = 0; key < nSetSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key)));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end());

            typename Set::guarded_ptr gp;

            // get/extract from empty set
            for ( auto idx : indices ) {
                auto& i = data[idx];

                gp = s.get( i.key());
                ASSERT_TRUE( !gp );

                gp = s.extract( i.key());
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
                gp = s.get( i.key());
                ASSERT_FALSE( !gp );
                ++gp->nFindCount;
                EXPECT_EQ( i.nFindCount, 1u );

                gp = s.extract( i.key());
                ASSERT_FALSE( !gp );
                ++gp->nEraseCount;
                EXPECT_EQ( i.nEraseCount, 1u );

                gp = s.extract( i.key());
                ASSERT_TRUE( !gp );

                gp = s.get( i.key());
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

            // erase_at( iterator )
            for ( auto& i : data ) {
                i.clear_stat();
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            for ( auto it = s.begin(); it != s.end(); ++it ) {
                ASSERT_TRUE( s.erase_at( it ));
                ASSERT_FALSE( s.erase_at( it ));
            }
            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            // Force retiring cycle
            Set::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }

            // erase_at( reverse_iterator )
            for ( auto& i : data ) {
                i.clear_stat();
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            for ( auto it = s.rbegin(); it != s.rend(); ++it ) {
                ASSERT_TRUE( s.erase_at( it ));
                ASSERT_FALSE( s.erase_at( it ));
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

#endif // #ifndef CDSUNIT_SET_TEST_INTRUSIVE_FELDMAN_HASHSET_HP_H
