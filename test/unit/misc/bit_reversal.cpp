// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
