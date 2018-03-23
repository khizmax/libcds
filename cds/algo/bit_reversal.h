// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_ALGO_BIT_REVERSAL_H
#define CDSLIB_ALGO_BIT_REVERSAL_H

#include <cds/algo/base.h>

    // Source: http://stackoverflow.com/questions/746171/best-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c
namespace cds { namespace algo {

    /// Bit reversal algorithms
    namespace bit_reversal {

        /// SWAR algorithm (source: http://aggregate.org/MAGIC/#Bit%20Reversal)
        struct swar {
            /// 32bit
            uint32_t operator()( uint32_t x ) const
            {
                x = ( ( ( x & 0xaaaaaaaa ) >> 1 ) | ( ( x & 0x55555555 ) << 1 ));
                x = ( ( ( x & 0xcccccccc ) >> 2 ) | ( ( x & 0x33333333 ) << 2 ));
                x = ( ( ( x & 0xf0f0f0f0 ) >> 4 ) | ( ( x & 0x0f0f0f0f ) << 4 ));
                x = ( ( ( x & 0xff00ff00 ) >> 8 ) | ( ( x & 0x00ff00ff ) << 8 ));
                return( ( x >> 16 ) | ( x << 16 ));
            }

            /// 64bit
            uint64_t operator()( uint64_t x ) const
            {
                return ( static_cast<uint64_t>( operator()( static_cast<uint32_t>( x ))) << 32 )  // low 32bit
                    | ( static_cast<uint64_t>( operator()( static_cast<uint32_t>( x >> 32 ))));  // high 32bit
            }
        };

        /// Lookup table algorithm
        struct lookup {
            /// 32bit
            uint32_t operator()( uint32_t x ) const
            {
                static uint8_t const table[] = {
                    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
                    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
                    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
                    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
                    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
                    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
                    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
                    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
                    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
                    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
                    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
                    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
                    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
                    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
                    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
                    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
                };
                static_assert( sizeof( table ) / sizeof( table[0] ) == 256, "Table size mismatch" );

                return ( static_cast<uint32_t>( table[x & 0xff] ) << 24 ) |
                    ( static_cast<uint32_t>( table[( x >> 8 ) & 0xff] ) << 16 ) |
                    ( static_cast<uint32_t>( table[( x >> 16 ) & 0xff] ) << 8 ) |
                    ( static_cast<uint32_t>( table[( x >> 24 ) & 0xff] ));
            }

            /// 64bit
            uint64_t operator()( uint64_t x ) const
            {
                return ( static_cast<uint64_t>( operator()( static_cast<uint32_t>( x ))) << 32 ) |
                    static_cast<uint64_t>( operator()( static_cast<uint32_t>( x >> 32 )));
            }
        };


        /// Mul-Div algorithm for 32bit architectire

        /// Mul-Div algorithm
        struct muldiv {
            //@cond
            static uint8_t muldiv32_byte( uint8_t b )
            {
                return static_cast<uint8_t>( ( ( b * 0x0802LU & 0x22110LU ) | ( b * 0x8020LU & 0x88440LU )) * 0x10101LU >> 16 );
            }

            static uint8_t muldiv64_byte( uint8_t b )
            {
                return static_cast<uint8_t>( ( b * 0x0202020202ULL & 0x010884422010ULL ) % 1023 );
            }

            // for 32bit architecture
            static uint32_t muldiv32( uint32_t x )
            {
                return static_cast<uint32_t>( muldiv32_byte( static_cast<uint8_t>( x >> 24 )))
                    | ( static_cast<uint32_t>( muldiv32_byte( static_cast<uint8_t>( x >> 16 ))) << 8 )
                    | ( static_cast<uint32_t>( muldiv32_byte( static_cast<uint8_t>( x >> 8 ))) << 16 )
                    | ( static_cast<uint32_t>( muldiv32_byte( static_cast<uint8_t>( x ))) << 24 );
            }

            static uint64_t muldiv32( uint64_t x )
            {
                return static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x >> 56 )))
                    | ( static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x >> 48 ))) << 8 )
                    | ( static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x >> 40 ))) << 16 )
                    | ( static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x >> 32 ))) << 24 )
                    | ( static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x >> 24 ))) << 32 )
                    | ( static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x >> 16 ))) << 40 )
                    | ( static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x >> 8 ))) << 48 )
                    | ( static_cast<uint64_t>( muldiv32_byte( static_cast<uint8_t>( x ))) << 56 );
            }

            /// for 64bit architectire
            static uint32_t muldiv64( uint32_t x )
            {
                return static_cast<uint32_t>( muldiv64_byte( static_cast<uint8_t>( x >> 24 )))
                    | ( static_cast<uint32_t>( muldiv64_byte( static_cast<uint8_t>( x >> 16 ))) << 8 )
                    | ( static_cast<uint32_t>( muldiv64_byte( static_cast<uint8_t>( x >> 8 ))) << 16 )
                    | ( static_cast<uint32_t>( muldiv64_byte( static_cast<uint8_t>( x ))) << 24 );
            }

            static uint64_t muldiv64( uint64_t x )
            {
                return static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x >> 56 )))
                    | ( static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x >> 48 ))) << 8 )
                    | ( static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x >> 40 ))) << 16 )
                    | ( static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x >> 32 ))) << 24 )
                    | ( static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x >> 24 ))) << 32 )
                    | ( static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x >> 16 ))) << 40 )
                    | ( static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x >> 8 ))) << 48 )
                    | ( static_cast<uint64_t>( muldiv64_byte( static_cast<uint8_t>( x ))) << 56 );
            }
            //@endcond

            /// 32bit
            uint32_t operator()( uint32_t x ) const
            {
#           if CDS_BUILD_BITS == 32
                return muldiv32( x );
#           else
                return muldiv64( x );
#           endif
            }

            /// 64bit
            uint64_t operator()( uint64_t x ) const
            {
#           if CDS_BUILD_BITS == 32
                return muldiv32( x );
#           else
                return muldiv64( x );
#           endif
            }
        };

    } // namespace bit_reversal
}} // namespace cds::algo

#endif // #ifndef CDSLIB_ALGO_BIT_REVERSAL_H
