// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/algo/split_bitstring.h>
#include <cds_test/ext_gtest.h>

namespace {
    bool is_big_endian()
    {
        union {
            uint32_t ui;
            uint8_t  ch;
        } byte_order;
        byte_order.ui = 0xFF000001;

        return byte_order.ch != 0x01;
    }

    class Split_bitstrig : public ::testing::Test
    {
    protected:
        void cut_uint_le()
        {
            typedef cds::algo::split_bitstring< size_t, 0, size_t > split_bitstring;

            size_t src = sizeof(src) == 8 ? 0xFEDCBA9876543210 : 0x76543210;
            split_bitstring splitter( src );
            size_t res;

            // Trivial case
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut( sizeof( src ) * 8 );
            EXPECT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut( sizeof( src ) * 8 ), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            splitter.reset();
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut( sizeof( src ) * 8 );
            EXPECT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut( sizeof( src ) * 8 ), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                ASSERT_EQ( splitter.cut( 4 ), i );
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_FALSE( splitter );
            EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );

            // by one bit
            {
                splitter.reset();
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                EXPECT_EQ( splitter.bit_offset(), 0u );

                res = 0;
                for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    res |= splitter.cut( 1 ) << i;
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );

                EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), 0u );
                EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
            }

            // random cut
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                    EXPECT_EQ( splitter.bit_offset(), 0u );

                    res = 0;
                    size_t shift = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        int bits = std::rand() % 16;
                        res |= splitter.safe_cut( bits ) << shift;
                        shift += bits;
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );

                    EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), 0u );
                    EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
                }
            }
        }

        void cut_uint_be()
        {
            typedef cds::algo::split_bitstring< size_t, 0, size_t > split_bitstring;

            size_t src = sizeof(src) == 8 ? 0xFEDCBA9876543210 : 0x76543210;
            split_bitstring splitter( src );
            size_t res;

            // Trivial case
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut( sizeof( src ) * 8 );
            ASSERT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut( sizeof( src ) * 8 ), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            splitter.reset();
            ASSERT_FALSE( splitter.eos());
            ASSERT_FALSE( !splitter );
            res = splitter.cut( sizeof( src ) * 8 );
            EXPECT_EQ( res, src );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut( sizeof( src ) * 8 ), 0u );
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                if ( i % 2 == 0 ) {
                    // even - least half-byte
                    EXPECT_EQ( splitter.cut( 4 ), 0x0E - i ) << "i=" << i;
                }
                else {
                    // odd - most half-byte
                    EXPECT_EQ( splitter.cut( 4 ), 0x0F - i + 1 ) << "i=" << i;
                }
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );

            // by one bit
            {
                splitter.reset();
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                EXPECT_EQ( splitter.bit_offset(), 0u );

                res = 0;
                for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    if ( i % 8 == 0 )
                        res = res << 8;
                    res |= ( splitter.cut( 1 )) << ( i % 8 );
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );

                EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), 0u );
                EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
            }

            // random cut
            /*
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                    EXPECT_EQ( splitter.bit_offset(), 0u );

                    res = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        unsigned bits = std::rand() % 16;
                        size_t shift = splitter.rest_count();
                        if ( shift > bits )
                            shift = bits;
                        res = (res << shift) | splitter.safe_cut( bits );
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );

                    EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), 0u );
                    EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
                }
            }
            */
        }

        template <typename PartUInt>
        void cut_small_le()
        {
            typedef PartUInt part_uint;

            typedef cds::algo::split_bitstring< uint64_t, 0, part_uint > split_bitstring;

            uint64_t src = 0xFEDCBA9876543210;
            split_bitstring splitter(src);
            uint64_t res;

            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
            EXPECT_EQ( splitter.bit_offset(), 0u );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(src) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                EXPECT_EQ( static_cast<size_t>(splitter.cut( 4 )), i );
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );

            // by one bit
            {
                splitter.reset();
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                EXPECT_EQ( splitter.bit_offset(), 0u );

                res = 0;
                for ( size_t i = 0; i < sizeof(src) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    res += static_cast<uint64_t>(splitter.cut( 1 )) << i;
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );
                EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), 0u );
                EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
            }

            // random cut
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                    EXPECT_EQ( splitter.bit_offset(), 0u );

                    res = 0;
                    size_t shift = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        int bits = std::rand() % 16;
                        res += static_cast<uint64_t>(splitter.safe_cut( bits )) << shift;
                        shift += bits;
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );
                    EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), 0u );
                    EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
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

            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
            EXPECT_EQ( splitter.bit_offset(), 0u );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                if ( i % 2 == 0 ) {
                    EXPECT_EQ( splitter.cut( 4 ), 0x0E - i );
                }
                else {
                    EXPECT_EQ( splitter.cut( 4 ), 0x0F - i + 1 );
                }
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );

            // by one bit
            {
                splitter.reset();
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                EXPECT_EQ( splitter.bit_offset(), 0u );

                res = 0;
                for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
                    if ( i % 8 == 0 )
                        res = res << 8;
                    res |= ( splitter.cut( 1 )) << ( i % 8 );
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src );
                EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                EXPECT_EQ( *splitter.source(), src );
                EXPECT_EQ( splitter.rest_count(), 0u );
                EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
            }

            // random cut
            /*
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
                    EXPECT_EQ( splitter.bit_offset(), 0u );

                    res = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        unsigned bits = std::rand() % 16;
                        size_t shift = splitter.rest_count();
                        if ( shift > bits )
                            shift = bits;
                        res = ( res << shift ) | splitter.safe_cut( bits );
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src );
                    EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                    EXPECT_EQ( *splitter.source(), src );
                    EXPECT_EQ( splitter.rest_count(), 0u );
                    EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
                }
            }
            */
        }

        struct int48 {
            uint32_t    n32;
            uint16_t    n16;
#if 0
            friend bool operator ==( int48 lhs, int48 rhs )
            {
                return lhs.n32 == rhs.n32 && lhs.n16 == rhs.n16;
            }
#endif

            uint64_t to64() const
            {
#       ifdef CDS_ARCH_LITTLE_ENDIAN
                return ( static_cast<uint64_t>( n16 ) << 32 ) + n32;
#       else
                return ( static_cast<uint64_t>( n32 ) << 16 ) + n16;
#       endif
            }
        };
        static constexpr size_t int48_size = 6;

        void cut_int48_le()
        {
            int48 src;
            src.n32 = 0x76543210;
            src.n16 = 0xBA98;

            uint64_t res;

#if CDS_BUILD_BITS == 64
            {
                typedef cds::algo::split_bitstring< int48, int48_size, size_t > split_bitstring;
                split_bitstring splitter( src );

                // Trivial case
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                res = splitter.cut( int48_size * 8 );
                EXPECT_EQ( res, src.to64());
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( splitter.safe_cut( int48_size * 8 ), 0u );
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                splitter.reset();
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                res = splitter.cut( int48_size * 8 );
                EXPECT_EQ( res, src.to64());
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( splitter.safe_cut( int48_size * 8 ), 0u );
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
            }
#endif

            typedef cds::algo::split_bitstring< int48, int48_size, size_t > split_bitstring;
            split_bitstring splitter( src );

            EXPECT_EQ( splitter.source()->to64(), src.to64());
            EXPECT_EQ( splitter.rest_count(), int48_size * 8 );
            EXPECT_EQ( splitter.bit_offset(), 0u );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < int48_size * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                ASSERT_EQ( splitter.cut( 4 ), i );
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_FALSE( splitter );
            EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
            EXPECT_EQ( splitter.source()->to64(), src.to64());
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), int48_size * 8 );

            // by one bit
            {
                splitter.reset();
                EXPECT_EQ( splitter.source()->to64(), src.to64());
                EXPECT_EQ( splitter.rest_count(), int48_size * 8 );
                EXPECT_EQ( splitter.bit_offset(), 0u );

                res = 0;
                for ( size_t i = 0; i < int48_size * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
#if CDS_BUILD_BITS == 64
                    res |= splitter.cut( 1 ) << i;
#else
                    res |= static_cast<decltype(res)>( splitter.cut( 1 )) << i;
#endif
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src.to64());
                EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                EXPECT_EQ( splitter.source()->to64(), src.to64());
                EXPECT_EQ( splitter.rest_count(), 0u );
                EXPECT_EQ( splitter.bit_offset(), int48_size * 8 );
            }

            // random cut
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    EXPECT_EQ( splitter.source()->to64(), src.to64());
                    EXPECT_EQ( splitter.rest_count(), int48_size * 8 );
                    EXPECT_EQ( splitter.bit_offset(), 0u );

                    res = 0;
                    size_t shift = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        int bits = std::rand() % 16;
#if CDS_BUILD_BITS == 64
                        res |= splitter.safe_cut( bits ) << shift;
#else
                        res |= static_cast<decltype(res)>( splitter.safe_cut( bits )) << shift;
#endif
                        shift += bits;
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src.to64());
                    EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                    EXPECT_EQ( splitter.source()->to64(), src.to64());
                    EXPECT_EQ( splitter.rest_count(), 0u );
                    EXPECT_EQ( splitter.bit_offset(), int48_size * 8 );
                }
            }
        }

        void cut_int48_be()
        {
            int48 src;
            src.n32 = 0xBA987654;
            src.n16 = 0x3210;

            uint64_t res;

#if CDS_BUILD_BITS == 64
            {
                typedef cds::algo::split_bitstring< int48, int48_size, size_t > split_bitstring;
                split_bitstring splitter( src );

                // Trivial case
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                res = splitter.cut( int48_size * 8 );
                ASSERT_EQ( res, src.to64());
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( splitter.safe_cut( int48_size * 8 ), 0u );
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                splitter.reset();
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                res = splitter.cut( int48_size * 8 );
                EXPECT_EQ( res, src.to64());
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( splitter.safe_cut( int48_size * 8 ), 0u );
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
            }
#endif

            typedef cds::algo::split_bitstring< int48, int48_size, size_t > split_bitstring;
            split_bitstring splitter( src );

            EXPECT_EQ( splitter.source()->to64(), src.to64());
            EXPECT_EQ( splitter.rest_count(), int48_size * 8 );
            EXPECT_EQ( splitter.bit_offset(), 0u );

            // Cut each hex digit
            splitter.reset();
            for ( size_t i = 0; i < int48_size * 2; ++i ) {
                ASSERT_FALSE( splitter.eos());
                ASSERT_FALSE( !splitter );
                if ( i % 2 == 0 ) {
                    EXPECT_EQ( splitter.cut( 4 ), 0x0A - i );
                }
                else {
                    EXPECT_EQ( splitter.cut( 4 ), 0x0B - i + 1 );
                }
            }
            ASSERT_TRUE( splitter.eos());
            ASSERT_TRUE( !splitter );
            EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
            EXPECT_EQ( splitter.source()->to64(), src.to64());
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), int48_size * 8 );

            // by one bit
            {
                splitter.reset();
                EXPECT_EQ( splitter.source()->to64(), src.to64());
                EXPECT_EQ( splitter.rest_count(), int48_size * 8 );
                EXPECT_EQ( splitter.bit_offset(), 0u );

                res = 0;
                for ( size_t i = 0; i < int48_size * 8; ++i ) {
                    ASSERT_FALSE( splitter.eos());
                    ASSERT_FALSE( !splitter );
#if CDS_BUILD_BITS == 64
                    if ( i % 8 == 0 )
                        res = res << 8;
                    res |= ( splitter.cut( 1 )) << ( i % 8 );
#else
                    res = ( res << 1 ) | static_cast<decltype(res)>( splitter.cut( 1 ));
#endif
                }
                ASSERT_TRUE( splitter.eos());
                ASSERT_TRUE( !splitter );
                EXPECT_EQ( res, src.to64());
                EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                EXPECT_EQ( splitter.source()->to64(), src.to64());
                EXPECT_EQ( splitter.rest_count(), 0u );
                EXPECT_EQ( splitter.bit_offset(), int48_size * 8 );
            }

            // random cut
            /*
            {
                for ( size_t k = 0; k < 100; ++k ) {
                    splitter.reset();
                    EXPECT_EQ( splitter.source()->to64(), src.to64());
                    EXPECT_EQ( splitter.rest_count(), int48_size * 8 );
                    EXPECT_EQ( splitter.bit_offset(), 0u );

                    res = 0;
                    while ( splitter ) {
                        ASSERT_FALSE( splitter.eos());
                        ASSERT_FALSE( !splitter );
                        unsigned bits = std::rand() % 16;
                        size_t shift = splitter.rest_count();
                        if ( shift > bits )
                            shift = bits;
#if CDS_BUILD_BITS == 64
                        res = ( res << shift ) | splitter.safe_cut( bits );
#else
                        res = ( res << shift ) | static_cast<decltype(res)>( splitter.safe_cut( bits ));
#endif
                    }
                    ASSERT_TRUE( splitter.eos());
                    ASSERT_TRUE( !splitter );
                    EXPECT_EQ( res, src.to64());
                    EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
                    EXPECT_EQ( splitter.source()->to64(), src.to64());
                    EXPECT_EQ( splitter.rest_count(), 0u );
                    EXPECT_EQ( splitter.bit_offset(), int48_size * 8 );
                }
            }
            */
        }

        void cut_byte_le()
        {
            size_t src = sizeof( src ) == 8 ? 0xFEDCBA9876543210 : 0x76543210;

            typedef cds::algo::byte_splitter< size_t > splitter_type;
            splitter_type splitter( src );

            ASSERT_TRUE( !splitter.eos());
            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
            EXPECT_EQ( splitter.bit_offset(), 0u );
            EXPECT_TRUE( splitter.is_correct( 8 ));
            EXPECT_FALSE( splitter.is_correct( 4 ));

            unsigned expected = 0x10;
            for ( unsigned i = 0; i < splitter_type::c_bitstring_size; ++i ) {
                auto part = splitter.cut( 8 );
                EXPECT_EQ( part, expected );
                expected += 0x22;
            }

            ASSERT_TRUE( splitter.eos());
            EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
        }

        void cut_byte_be()
        {
            size_t src = sizeof( src ) == 8 ? 0xFEDCBA9876543210 : 0x76543210;

            typedef cds::algo::byte_splitter< size_t > splitter_type;
            splitter_type splitter( src );

            ASSERT_TRUE( !splitter.eos());
            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), sizeof( src ) * 8 );
            EXPECT_EQ( splitter.bit_offset(), 0u );
            EXPECT_TRUE( splitter.is_correct( 8 ));
            EXPECT_FALSE( splitter.is_correct( 4 ));

            unsigned expected = 0xFE;
            for ( unsigned i = 0; i < splitter_type::c_bitstring_size; ++i ) {
                auto part = splitter.cut( 8 );
                EXPECT_EQ( part, expected );
                expected -= 0x22;
            }

            ASSERT_TRUE( splitter.eos());
            EXPECT_EQ( splitter.safe_cut( 8 ), 0u );
            EXPECT_EQ( *splitter.source(), src );
            EXPECT_EQ( splitter.rest_count(), 0u );
            EXPECT_EQ( splitter.bit_offset(), sizeof( src ) * 8 );
        }
    };

    class Split_number: public ::testing::Test
    {
    protected:
        template <typename Int>
        void split( Int const n )
        {
            cds::algo::number_splitter< Int > splitter( n );

            // split by hex digit
            for ( unsigned count = 4; count < sizeof( Int ) * 8; count += 4 ) {
                EXPECT_EQ( splitter.cut( 4 ), static_cast<Int>( count / 4 - 1 ));
            }

            // random cut
            for ( int i = 0; i < 100; ++i ) {
                splitter.reset();
                EXPECT_EQ( splitter.source(), n );
                EXPECT_EQ( splitter.bit_offset(), 0u );
                EXPECT_EQ( splitter.rest_count(), sizeof( Int ) * 8 );

                unsigned total = 0;
                Int result = 0;

                while ( total < sizeof( Int ) * 8 ) {
                    unsigned count = std::rand() % 16;

                    unsigned shift = count;
                    if ( total + count > sizeof( Int ) * 8 )
                        shift = sizeof( Int ) * 8 - total;

                    result += splitter.safe_cut( count ) << total;
                    total += shift;
                }

                EXPECT_EQ( result, n );

                EXPECT_EQ( splitter.bit_offset(), sizeof( Int ) * 8 );
                EXPECT_EQ( splitter.rest_count(), 0u );
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

    TEST_F( Split_bitstrig, cut_int48 )
    {
        if ( is_big_endian())
            cut_int48_be();
        else
            cut_int48_le();
    }

    TEST_F( Split_bitstrig, cut_byte )
    {
        if ( is_big_endian())
            cut_byte_be();
        else
            cut_byte_le();
    }

    TEST_F( Split_number, split_int )
    {
        split( (int)0x76543210 );
    }

    TEST_F( Split_number, split_uint )
    {
        split( (unsigned)0x76543210 );
    }

    TEST_F( Split_number, split_short )
    {
        split( (short int)0x3210 );
    }

    TEST_F( Split_number, split_ushort )
    {
        split( (unsigned short)0x3210 );
    }

    TEST_F( Split_number, split_long )
    {
        if ( sizeof( long ) == 8 )
            split( (long)0xFEDCBA9876543210 );
        else
            split( (long)0x76543210 );
    }

    TEST_F( Split_number, split_ulong )
    {
        if ( sizeof( long ) == 8 )
            split( (unsigned long)0xFEDCBA9876543210 );
        else
            split( (unsigned long)0x76543210 );
    }

    TEST_F( Split_number, split_int64 )
    {
        split( (int64_t)0xFEDCBA9876543210 );
    }

    TEST_F( Split_number, split_uint64 )
    {
        split( (uint64_t)0xFEDCBA9876543210 );
    }

} // namespace
