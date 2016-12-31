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

#include <gtest/gtest.h>
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
