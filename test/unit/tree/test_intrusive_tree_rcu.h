// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_TREE_TEST_INTRUSIVE_TREE_RCU_H
#define CDSUNIT_TREE_TEST_INTRUSIVE_TREE_RCU_H

#include "test_intrusive_tree.h"


// forward declaration
namespace cds { namespace intrusive {}}

namespace cds_test {

    namespace ci = cds::intrusive;

    class intrusive_tree_rcu: public intrusive_tree
    {
        typedef intrusive_tree base_class;

    protected:


        template <class Tree>
        void test( Tree& t )
        {
            // Precondition: tree is empty
            // Postcondition: tree is empty

            base_class::test( t );

            ASSERT_TRUE( t.empty());
            ASSERT_CONTAINER_SIZE( t, 0 );

            typedef typename Tree::value_type value_type;
            typedef typename Tree::rcu_lock   rcu_lock;

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key)));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end());

            typename Tree::exempt_ptr xp;

            // get/extract from empty tree
            for ( auto idx : indices ) {
                auto& i = data[idx];

                {
                    rcu_lock l;
                    value_type * p = t.get( i );
                    ASSERT_TRUE( p == nullptr );
                    p = t.get( i.key());
                    ASSERT_TRUE( !p );
                    p = t.get_with( other_item( i.key()), other_less());
                    ASSERT_TRUE( p == nullptr );
                }

                xp = t.extract( i );
                ASSERT_TRUE( !xp );
                xp = t.extract( i.key());
                ASSERT_TRUE( !xp );
                xp = t.extract_with( other_item( i.key()), other_less());
                ASSERT_TRUE( !xp );

                xp = t.extract_min();
                ASSERT_TRUE( !xp );
                xp = t.extract_max();
                ASSERT_TRUE( !xp );
            }

            // fill tree
            for ( auto idx : indices ) {
                auto& i = data[idx];
                i.nDisposeCount = 0;
                ASSERT_TRUE( t.insert( i ));
            }

            // get/extract
            for ( auto idx : indices ) {
                auto& i = data[idx];

                {
                    rcu_lock l;

                    value_type * p;
                    EXPECT_EQ( i.nFindCount, 0u );
                    p = t.get( i );
                    ASSERT_FALSE( !p );
                    ++p->nFindCount;
                    EXPECT_EQ( i.nFindCount, 1u );

                    p = t.get( i.key());
                    ASSERT_FALSE( !p );
                    ++p->nFindCount;
                    EXPECT_EQ( i.nFindCount, 2u );

                    p = t.get_with( other_item( i.key()), other_less());
                    ASSERT_FALSE( !p );
                    ++p->nFindCount;
                    EXPECT_EQ( i.nFindCount, 3u );
                }

                EXPECT_EQ( i.nEraseCount, 0u );
                switch ( i.key() % 3 ) {
                case 0:
                    xp = t.extract( i.key());
                    break;
                case 1:
                    xp = t.extract( i );
                    break;
                case 2:
                    xp = t.extract_with( other_item( i.key()), other_less());
                    break;
                }
                ASSERT_FALSE( !xp );
                ++xp->nEraseCount;
                EXPECT_EQ( i.nEraseCount, 1u );

                xp = t.extract( i );
                ASSERT_TRUE( !xp );
                xp = t.extract( i.key());
                ASSERT_TRUE( !xp );
                xp = t.extract_with( other_item( i.key()), other_less());
                ASSERT_TRUE( !xp );
            }

            ASSERT_TRUE( t.empty());
            ASSERT_CONTAINER_SIZE( t, 0u );

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }

            // extract_min
            for ( auto idx : indices ) {
                auto& i = data[idx];
                i.nDisposeCount = 0;
                ASSERT_TRUE( t.insert( i ));
            }

            size_t nCount = 0;
            int nKey = -1;
            while ( !t.empty()) {
                xp = t.extract_min();
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->key(), nKey + 1 );
                ++nCount;
                nKey = xp->key();
            }
            xp.release();
            ASSERT_TRUE( t.empty());
            ASSERT_CONTAINER_SIZE( t, 0u );
            EXPECT_EQ( nCount, data.size());

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }

            // extract_max
            for ( auto idx : indices ) {
                auto& i = data[idx];
                i.nDisposeCount = 0;
                ASSERT_TRUE( t.insert( i ));
            }

            nCount = 0;
            nKey = static_cast<int>( data.size());
            while ( !t.empty()) {
                xp = t.extract_max();
                ASSERT_FALSE( !xp );
                EXPECT_EQ( xp->key(), nKey - 1 );
                ++nCount;
                nKey = xp->key();
            }
            xp.release();
            ASSERT_TRUE( t.empty());
            ASSERT_CONTAINER_SIZE( t, 0u );
            EXPECT_EQ( nCount, data.size());

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1u );
            }
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_TREE_TEST_INTRUSIVE_TREE_RCU_H
