// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/opt/permutation.h>
#include <cds_test/ext_gtest.h>

namespace {

    class Permutations: public ::testing::Test
    {
    protected:
        static const size_t c_nMax = 1024;

        template <typename Gen>
        void test_with( Gen& gen, size_t nLen )
        {
            unsigned int arr[c_nMax];
            for ( size_t nPass = 0; nPass < 10; ++nPass ) {
                for ( size_t i = 0; i < c_nMax; ++i )
                    arr[i] = 0;

                do {
                    typename Gen::integer_type i = gen;
                    ++arr[ i ];
                } while ( gen.next());

                for ( size_t i = 0; i < nLen; ++i )
                    EXPECT_EQ( arr[i], 1u ) << "i=" << i;
                for ( size_t i = nLen; i < c_nMax; ++i )
                    EXPECT_EQ( arr[i], 0u ) << "i=" << i;

                gen.reset();
            }
        }

        template <typename Gen>
        void test()
        {
            for ( size_t nLen = 2; nLen <= c_nMax; ++nLen ) {
                Gen gen( nLen );
                test_with( gen, nLen );
            }
        }

        template <typename Gen>
        void test2()
        {
            for ( size_t nLen = 2; nLen <= c_nMax; nLen *= 2 ) {
                Gen gen( nLen );
                test_with( gen, nLen );
            }
        }
    };

    TEST_F( Permutations, random_permutation )
    {
        test< cds::opt::v::random_permutation<> >();
    }
    TEST_F( Permutations, random2_permutation )
    {
        test2< cds::opt::v::random2_permutation<> >();
    }
    TEST_F( Permutations, random_shuffle_permutation )
    {
        test< cds::opt::v::random_shuffle_permutation<> >();
    }

} // namespace
