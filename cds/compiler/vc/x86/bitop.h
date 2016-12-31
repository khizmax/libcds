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

#ifndef CDSLIB_COMPILER_VC_X86_BITOP_H
#define CDSLIB_COMPILER_VC_X86_BITOP_H

#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(_BitScanForward)

//@cond none
namespace cds {
    namespace bitop { namespace platform { namespace vc { namespace x86 {
        // MSB - return index (1..32) of most significant bit in nArg. If nArg == 0 return 0
#        define cds_bitop_msb32_DEFINED
        static inline int msb32( uint32_t nArg )
        {
            unsigned long nIndex;
            if ( _BitScanReverse( &nIndex, nArg ))
                return (int) nIndex + 1;
            return 0;
        }

#        define cds_bitop_msb32nz_DEFINED
        static inline int msb32nz( uint32_t nArg )
        {
            assert( nArg != 0 );
            unsigned long nIndex;
            _BitScanReverse( &nIndex, nArg );
            return (int) nIndex;
        }

        // LSB - return index (1..32) of least significant bit in nArg. If nArg == 0 return -1U
#        define cds_bitop_lsb32_DEFINED
        static inline int lsb32( uint32_t nArg )
        {
            unsigned long nIndex;
            if ( _BitScanForward( &nIndex, nArg ))
                return (int) nIndex + 1;
            return 0;
        }

#        define cds_bitop_lsb32nz_DEFINED
        static inline int lsb32nz( uint32_t nArg )
        {
            assert( nArg != 0 );
            unsigned long nIndex;
            _BitScanForward( &nIndex, nArg );
            return (int) nIndex;
        }

        // bswap - Reverses the byte order of a 32-bit word
#        define cds_bitop_bswap32_DEFINED
        static inline uint32_t bswap32( uint32_t nArg )
        {
            __asm {
                mov    eax, nArg;
                bswap    eax;
            }
        }

#       define cds_bitop_complement32_DEFINED
        static inline bool complement32( uint32_t * pArg, unsigned int nBit )
        {
            return _bittestandcomplement( reinterpret_cast<long *>( pArg ), nBit ) != 0;
        }

#       define cds_bitop_complement64_DEFINED
        static inline bool complement64( uint64_t * pArg, unsigned int nBit )
        {
            if ( nBit < 32 )
                return _bittestandcomplement( reinterpret_cast<long *>( pArg ), nBit ) != 0;
            else
                return _bittestandcomplement( reinterpret_cast<long *>( pArg ) + 1, nBit - 32 ) != 0;
        }

    }} // namespace vc::x86

    using namespace vc::x86;

}}}    // namespace cds::bitop::platform
//@endcond

#endif    // #ifndef CDSLIB_COMPILER_VC_X86_BITOP_H
