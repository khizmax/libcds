// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_BITOP_H
#define CDSLIB_BITOP_H

/*
    Different bit algorithms:
        LSB        get least significant bit number
        MSB        get most significant bit number
        bswap    swap byte order of word
        RBO        reverse bit order of word

    Editions:
        2007.10.08    Maxim.Khiszinsky    Created
*/

#include <cds/details/defs.h>
#include <cds/compiler/bitop.h>

namespace cds {
    /// Bit operations
    namespace bitop {

        ///@cond none
        namespace details {
            template <int> struct BitOps;

            // 32-bit bit ops
            template <> struct BitOps<4> {
                typedef uint32_t        TUInt;

                static int      MSB( TUInt x )      { return bitop::platform::msb32( x );   }
                static int      LSB( TUInt x )      { return bitop::platform::lsb32( x );   }
                static int      MSBnz( TUInt x )    { return bitop::platform::msb32nz( x ); }
                static int      LSBnz( TUInt x )    { return bitop::platform::lsb32nz( x ); }
                static int      SBC( TUInt x )      { return bitop::platform::sbc32( x ) ;  }
                static int      ZBC( TUInt x )      { return bitop::platform::zbc32( x ) ;  }

                static TUInt    RBO( TUInt x )      { return bitop::platform::rbo32( x );   }
                static bool     complement( TUInt& x, int nBit ) { return bitop::platform::complement32( &x, nBit ); }

                static TUInt    RandXorShift(TUInt x) { return bitop::platform::RandXorShift32(x); }
            };

            // 64-bit bit ops
            template <> struct BitOps<8> {
                typedef uint64_t TUInt;

                static int      MSB( TUInt x )        { return bitop::platform::msb64( x );     }
                static int      LSB( TUInt x )        { return bitop::platform::lsb64( x );     }
                static int      MSBnz( TUInt x )      { return bitop::platform::msb64nz( x );   }
                static int      LSBnz( TUInt x )      { return bitop::platform::lsb64nz( x );   }
                static  int     SBC( TUInt x )        { return bitop::platform::sbc64( x ) ;    }
                static  int     ZBC( TUInt x )        { return bitop::platform::zbc64( x ) ;    }

                static TUInt    RBO( TUInt x )        { return bitop::platform::rbo64( x );     }
                static bool     complement( TUInt& x, int nBit ) { return bitop::platform::complement64( &x, nBit ); }

                static TUInt    RandXorShift(TUInt x) { return bitop::platform::RandXorShift64(x); }
            };
        }    // namespace details
        //@endcond


        /// Get least significant bit (LSB) number (1..32/64), 0 if nArg == 0
        template <typename T>
        static inline int LSB( T nArg )
        {
            return details::BitOps< sizeof(T) >::LSB( (typename details::BitOps<sizeof(T)>::TUInt) nArg );
        }

        /// Get least significant bit (LSB) number (0..31/63)
        /**
            Precondition: nArg != 0
        */
        template <typename T>
        static inline int LSBnz( T nArg )
        {
            assert( nArg != 0 );
            return details::BitOps< sizeof(T) >::LSBnz( (typename details::BitOps<sizeof(T)>::TUInt) nArg );
        }

        /// Get most significant bit (MSB) number (1..32/64), 0 if nArg == 0
        template <typename T>
        static inline int MSB( T nArg )
        {
            return details::BitOps< sizeof(T) >::MSB( (typename details::BitOps<sizeof(T)>::TUInt) nArg );
        }

        /// Get most significant bit (MSB) number (0..31/63)
        /**
            Precondition: nArg != 0
        */
        template <typename T>
        static inline int MSBnz( T nArg )
        {
            assert( nArg != 0 );
            return details::BitOps< sizeof(T) >::MSBnz( (typename details::BitOps<sizeof(T)>::TUInt) nArg );
        }

        /// Get non-zero bit count of a word
        template <typename T>
        static inline int SBC( T nArg )
        {
            return details::BitOps< sizeof(T) >::SBC( (typename details::BitOps<sizeof(T)>::TUInt) nArg );
        }

        /// Get zero bit count of a word
        template <typename T>
        static inline int ZBC( T nArg )
        {
            return details::BitOps< sizeof(T) >::ZBC( (typename details::BitOps<sizeof(T)>::TUInt) nArg );
        }

        /// Reverse bit order of \p nArg
        template <typename T>
        static inline T RBO( T nArg )
        {
            return (T) details::BitOps< sizeof(T) >::RBO( (typename details::BitOps<sizeof(T)>::TUInt) nArg );
        }

        /// Complement bit \p nBit in \p nArg
        template <typename T>
        static inline bool complement( T& nArg, int nBit )
        {
            return details::BitOps< sizeof(T) >::complement( reinterpret_cast< typename details::BitOps<sizeof(T)>::TUInt& >( nArg ), nBit );
        }

        /// Simple random number generator
        template <typename T>
        static inline T RandXorShift( T x)
        {
            return (T) details::BitOps< sizeof(T) >::RandXorShift(x);
        }

    } // namespace bitop
} //namespace cds

#endif    // #ifndef CDSLIB_BITOP_H

