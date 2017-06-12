#ifndef CDSUNIT_SET_TEST_NC_FELDMAN_HASHSET_HP_H
#define CDSUNIT_SET_TEST_NC_FELDMAN_HASHSET_HP_H

#include "test_nc_feldman_hashset.h"

namespace cds_test {

    class nc_feldman_hashset_hp: public nc_feldman_hashset
    {
        typedef nc_feldman_hashset base_class;

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
            std::random_shuffle( indices.begin(), indices.end());

            for ( auto& i : data ) {
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            typedef typename Set::guarded_ptr guarded_ptr;
            guarded_ptr gp;

            // get()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !gp );

                gp = s.get( i );
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->key(), i.key());
                gp->nFindCount *= 2;

                gp.release();
            }

            // extract()
            for ( auto idx : indices ) {
                auto& i = data[idx];

                ASSERT_TRUE( !gp );
                gp = s.extract( i );
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->key(), i.key());

                gp = s.extract( i );
                ASSERT_TRUE( !gp );
            }

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );
        }
    };
} // namespace cds_test

#endif // CDSUNIT_SET_TEST_NC_FELDMAN_HASHSET_HP_H
