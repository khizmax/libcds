// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TEST_SET_RCU_H
#define CDSUNIT_SET_TEST_SET_RCU_H

#include "test_set.h"

namespace cds_test {

    class container_set_rcu: public container_set
    {
        typedef container_set base_class;

    protected:
        template <typename Set>
        void test( Set& s )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            base_class::test( s );

            typedef typename Set::value_type value_type;

            size_t const nSetSize = kSize;
            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key)));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end());

            for ( auto& i : data ) {
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            // iterator test
            for ( auto it = s.begin(); it != s.end(); ++it ) {
                it->nFindCount = it->key() * 3;
            }

            for ( auto it = s.cbegin(); it != s.cend(); ++it ) {
                EXPECT_EQ( it->nFindCount, static_cast<unsigned>( it->key() * 3 ));
            }

            typedef typename Set::exempt_ptr exempt_ptr;
            typedef typename Set::raw_ptr    raw_ptr;
            typedef typename Set::rcu_lock   rcu_lock;

            // get()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                {
                    rcu_lock l;
                    raw_ptr  rp{};
                    ASSERT_TRUE( !rp );
                    switch ( idx % 3 ) {
                    case 0:
                        rp = s.get( i.key());
                        ASSERT_FALSE( !rp );
                        break;
                    case 1:
                        rp = s.get( i );
                        ASSERT_FALSE( !rp );
                        break;
                    case 2:
                        rp = s.get_with( other_item( i.key()), other_less());
                        ASSERT_FALSE( !rp );
                    }
                    EXPECT_EQ( rp->key(), i.key());
                    EXPECT_EQ( rp->nFindCount, static_cast<unsigned>( i.key() * 3 ));
                    rp->nFindCount *= 2;
                }
            }

            // extract()
            exempt_ptr xp;
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !xp );
                if ( Set::c_bExtractLockExternal ) {
                    {
                        rcu_lock l;

                        switch ( idx % 3 ) {
                        case 0:
                            xp = s.extract( i.key());
                            ASSERT_FALSE( !xp );
                            break;
                        case 1:
                            xp = s.extract( i );
                            ASSERT_FALSE( !xp );
                            break;
                        case 2:
                            xp = s.extract_with( other_item( i.key()), other_less());
                            ASSERT_FALSE( !xp );
                            break;
                        }
                        EXPECT_EQ( xp->key(), i.key());
                        EXPECT_EQ( xp->nFindCount, static_cast<unsigned>( i.key() * 6 ));
                    }
                    xp.release();

                    {
                        rcu_lock l;

                        switch ( idx % 3 ) {
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
                        ASSERT_TRUE( !xp );
                    }
                }
                else {
                    switch ( idx % 3 ) {
                    case 0:
                        xp = s.extract( i.key());
                        ASSERT_FALSE( !xp );
                        break;
                    case 1:
                        xp = s.extract( i );
                        ASSERT_FALSE( !xp );
                        break;
                    case 2:
                        xp = s.extract_with( other_item( i.key()), other_less());
                        ASSERT_FALSE( !xp );
                        break;
                    }
                    EXPECT_EQ( xp->key(), i.key());
                    EXPECT_EQ( xp->nFindCount, static_cast<unsigned>( i.key() * 6 ));

                    switch ( idx % 3 ) {
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
                    ASSERT_TRUE( !xp );
                }
            }

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );
        }

    };
} // namespace cds_test

#endif // CDSUNIT_SET_TEST_SET_RCU_H
