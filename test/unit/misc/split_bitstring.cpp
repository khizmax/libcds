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

#include <cds/algo/split_bitstring.h>
#include <gtest/gtest.h>

namespace {
    class Split_bitstrig : public ::testing::Test
    {
    protected:
        bool is_big_endian()
        {
            union {
                uint32_t ui;
                uint8_t  ch;
            } byte_order;
            byte_order.ui = 0xFF000001;

            return byte_order.ch != 0x01;
        }

        void cut_uint_le()
        {
            typedef cds::algo::split_bitstring< size_t > split_bitstring;

            size_t src = sizeof(src) == 8 ? 0xFEDCBA9876543210 : 0x76543210;
            split_bitstring splitter(src);
            size_t res;

            // Trivial case
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut(sizeof(src) * 8);
            EXPECT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ(splitter.safe_cut(sizeof(src) * 8), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            splitter.reset();
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut(sizeof(src) * 8);
            EXPECT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut(sizeof(src) * 8), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                ASSERT_EQ( splitter.cut( 4 ), i );
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_FALSE( splitter );

            // by one bit
            {
                splitter.reset();
                res = 0;
                for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    res = res + (splitter.cut( 1 ) << i);
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );
            }

            // random cut
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    res = 0;
                    size_t shift = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        int bits = std::rand() % 16;
                        res = res + ( splitter.safe_cut( bits ) << shift );
                        shift += bits;
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );
                }
            }
        }

        void cut_uint_be()
        {
            typedef cds::algo::split_bitstring< size_t > split_bitstring;

            size_t src = sizeof(src) == 8 ? 0xFEDCBA9876543210 : 0x76543210;
            split_bitstring splitter(src);
            size_t res;

            // Trivial case
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut(sizeof(src) * 8);
            ASSERT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ(splitter.safe_cut(sizeof(src) * 8), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            splitter.reset();
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut(sizeof(src) * 8);
            EXPECT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ(splitter.safe_cut(sizeof(src) * 8), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                EXPECT_EQ( splitter.cut( 4 ), 0x0F - i );
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            // by one bit
            {
                splitter.reset();
                res = 0;
                for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    res = (res << 1) + splitter.cut( 1 );
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );
            }

            // random cut
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    res = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        int bits = std::rand() % 16;
                        res = (res << bits) + splitter.safe_cut( bits );
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );
                }
            }
        }

        template <typename PartUInt>
        void cut_small_le()
        {
            typedef PartUInt part_uint;

            typedef cds::algo::split_bitstring< uint64_t, 0, part_uint > split_bitstring;

            uint64_t src = 0xFEDCBA9876543210;
            split_bitstring splitter(src);
            uint64_t res;

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(src) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                EXPECT_EQ( static_cast<size_t>(splitter.cut( 4 )), i );
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            // by one bit
            {
                splitter.reset();
                res = 0;
                for ( size_t i = 0; i < sizeof(src) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    res = res + ( static_cast<uint64_t>(splitter.cut( 1 )) << i);
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );
            }

            // random cut
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    res = 0;
                    size_t shift = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        int bits = std::rand() % 16;
                        res = res + ( static_cast<uint64_t>(splitter.safe_cut( bits )) << shift );
                        shift += bits;
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );
                }
            }
        }

        template <typename PartUInt>
        void cut_small_be()
        {
            typedef PartUInt part_uint;

            typedef cds::algo::split_bitstring< uint64_t, 0, part_uint > split_bitstring;

            uint64_t src = 0xFEDCBA9876543210;
            split_bitstring splitter(src);
            uint64_t res;

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                EXPECT_EQ( splitter.cut( 4 ), 0x0F - i );
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            // by one bit
            {
                splitter.reset();
                res = 0;
                for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    res = (res << 1) + splitter.cut( 1 );
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );
            }

            // random cut
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    res = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        int bits = std::rand() % 16;
                        res = (res << bits) + splitter.safe_cut( bits );
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );
                }
            }
        }
    };

    TEST_F( Split_bitstrig, cut_uint )
    {
        if ( is_big_endian())
            cut_uint_be();
        else
            cut_uint_le();
    }

    TEST_F( Split_bitstrig, cut_uint16 )
    {
        if ( is_big_endian())
            cut_small_be<uint16_t>();
        else
            cut_small_le<uint16_t>();
    }

} // namespace
