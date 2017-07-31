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

#include <cds_test/ext_gtest.h>
#include <cds/algo/bit_reversal.h>

namespace {

    template <typename UInt>
    class bit_reversal: public ::testing::Test
    {
        typedef UInt uint_type;
        static std::vector<uint_type> arr_;
        static size_t const c_size = 100'000'000;

    public:
        static void SetUpTestCase()
        {
            arr_.resize( c_size );
            for ( size_t i = 0; i < c_size; ++i )
                arr_[i] = static_cast<uint_type>((i << 32) + (~i));
        }

        static void TearDownTestCase()
        {
            arr_.resize( 0 );
        }

        template <typename Algo>
        void test()
        {
            Algo f;
            for ( auto i : arr_ ) {
                EXPECT_EQ( i, f( f( i )));
            }
        }

        template <typename Algo>
        void test_eq()
        {
            Algo f;
            for ( auto i : arr_ ) {
                EXPECT_EQ( cds::algo::bit_reversal::swar()( i ), f( i )) << "i=" << i;
            }
        }
    };

    template <typename UInt> std::vector<UInt> bit_reversal<UInt>::arr_;

    typedef bit_reversal<uint32_t> bit_reversal32;
    typedef bit_reversal<uint64_t> bit_reversal64;

#define TEST_32BIT( x ) \
    TEST_F( bit_reversal32, x )      { test<cds::algo::bit_reversal::x>(); } \
    TEST_F( bit_reversal32, x##_eq ) { test_eq<cds::algo::bit_reversal::x>(); }

#define TEST_64BIT( x ) \
    TEST_F( bit_reversal64, x )      { test<cds::algo::bit_reversal::x>(); } \
    TEST_F( bit_reversal64, x##_eq ) { test_eq<cds::algo::bit_reversal::x>(); }

    TEST_32BIT( swar )
    TEST_32BIT( lookup )
    TEST_32BIT( muldiv )

    TEST_64BIT( swar )
    TEST_64BIT( lookup )
    TEST_64BIT( muldiv )

} // namespace
