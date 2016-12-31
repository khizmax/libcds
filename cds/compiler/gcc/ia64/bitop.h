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

#ifndef CDSLIB_COMPILER_GCC_IA64_BITOP_H
#define CDSLIB_COMPILER_GCC_IA64_BITOP_H

//@cond none
namespace cds {
    namespace bitop { namespace platform { namespace gcc { namespace ia64 {

        // MSB - return index (1..32) of most significant bit in x. If x == 0 return 0
#        define cds_bitop_msb32_DEFINED
        static inline int msb32( uint32_t nArg )
        {
            if ( !nArg )
                return 0;
            uint64_t x = nArg;
            x |= x >> 1;
            x |= x >> 2;
            x |= x >> 4;
            x |= x >> 8;
            x |= x >> 16;

            uint64_t    nRes;
            asm __volatile__( "popcnt %0=%1\n\t" : "=r" (nRes) : "r" (x));
            return (int) nRes;
        }

        // It is not compiled on HP-UX. Why?..
#if CDS_OS_TYPE != CDS_OS_HPUX
        // MSB - return index (0..31) of most significant bit in nArg.
        // !!! nArg != 0
#        define cds_bitop_msb32nz_DEFINED
        static inline int msb32nz( uint32_t nArg )
        {
            assert( nArg != 0 );
            long double d = nArg;
            long nExp;
            asm __volatile__("getf.exp %0=%1\n\t" : "=r"(nExp) : "f"(d));
            return (int) (nExp - 0xffff);
        }

        // MSB - return index (0..63) of most significant bit in nArg.
        // !!! nArg != 0
#        define cds_bitop_msb64nz_DEFINED
        static inline int msb64nz( uint64_t nArg )
        {
            assert( nArg != 0 );
            long double d = nArg;
            long nExp;
            asm __volatile__("getf.exp %0=%1\n\t" : "=r" (nExp) : "f" (d));
            return (int) (nExp - 0xffff);
        }
#endif    // #if CDS_OS_TYPE != CDS_OS_HPUX

    }} // namespace gcc::ia64

    using namespace gcc::ia64;

}}}    // namespace cds::bitop::platform
//@endcond

#endif // #ifndef CDSLIB_COMPILER_GCC_IA64_BITOP_H
