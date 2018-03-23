// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/opt/hash.h>

namespace {
    typedef cds::opt::v::hash_selector< cds::opt::none >::type hashing;

#define HASHING(_n) \
        struct hash##_n: public hashing { \
            template <typename T> size_t operator()( T const& v ) const { return hashing::operator()(v) + _n ; } \
        };

        HASHING(2)
        HASHING(3)
        HASHING(4)
        HASHING(5)
        HASHING(6)
        HASHING(7)
        HASHING(8)
        HASHING(9)
        HASHING(10)
#undef HASHING

    TEST( HashTuple, test )
    {
        int nVal = 5;
        size_t nHash = hashing()(nVal);

        size_t val[16];

        cds::opt::hash< std::tuple< hashing, hash2 > >::pack<cds::opt::none>::hash  h2;
        h2( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3 > >::pack<cds::opt::none>::hash  h3;
        h3( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4 > >::pack<cds::opt::none>::hash  h4;
        h4( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );
        EXPECT_EQ( val[3], nHash + 4 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5 > >::pack<cds::opt::none>::hash  h5;
        h5( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );
        EXPECT_EQ( val[3], nHash + 4 );
        EXPECT_EQ( val[4], nHash + 5 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6 > >::pack<cds::opt::none>::hash  h6;
        h6( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );
        EXPECT_EQ( val[3], nHash + 4 );
        EXPECT_EQ( val[4], nHash + 5 );
        EXPECT_EQ( val[5], nHash + 6 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7 > >::pack<cds::opt::none>::hash  h7;
        h7( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );
        EXPECT_EQ( val[3], nHash + 4 );
        EXPECT_EQ( val[4], nHash + 5 );
        EXPECT_EQ( val[5], nHash + 6 );
        EXPECT_EQ( val[6], nHash + 7 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7, hash8 > >::pack<cds::opt::none>::hash  h8;
        h8( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );
        EXPECT_EQ( val[3], nHash + 4 );
        EXPECT_EQ( val[4], nHash + 5 );
        EXPECT_EQ( val[5], nHash + 6 );
        EXPECT_EQ( val[6], nHash + 7 );
        EXPECT_EQ( val[7], nHash + 8 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7, hash8, hash9 > >::pack<cds::opt::none>::hash  h9;
        h9( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );
        EXPECT_EQ( val[3], nHash + 4 );
        EXPECT_EQ( val[4], nHash + 5 );
        EXPECT_EQ( val[5], nHash + 6 );
        EXPECT_EQ( val[6], nHash + 7 );
        EXPECT_EQ( val[7], nHash + 8 );
        EXPECT_EQ( val[8], nHash + 9 );

        cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7, hash8, hash9, hash10 > >::pack<cds::opt::none>::hash  h10;
        h10( val, nVal );
        EXPECT_EQ( val[0], nHash );
        EXPECT_EQ( val[1], nHash + 2 );
        EXPECT_EQ( val[2], nHash + 3 );
        EXPECT_EQ( val[3], nHash + 4 );
        EXPECT_EQ( val[4], nHash + 5 );
        EXPECT_EQ( val[5], nHash + 6 );
        EXPECT_EQ( val[6], nHash + 7 );
        EXPECT_EQ( val[7], nHash + 8 );
        EXPECT_EQ( val[8], nHash + 9 );
        EXPECT_EQ( val[9], nHash + 10 );
    }

} // namespace
