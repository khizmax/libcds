/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_COMPILER_GCC_SPARC_BITOP_H
#define CDSLIB_COMPILER_GCC_SPARC_BITOP_H

//@cond none
namespace cds {
    namespace bitop { namespace platform { namespace gcc { namespace Sparc {

        // MSB - return index (1..64) of most significant bit in nArg. If nArg == 0 return 0
        // Source: UltraSPARC Architecture 2007
        //
        // Test result: this variant and its variation about 100 times slower then generic implementation :-(
        static inline int sparc_msb64( uint64_t nArg )
        {
            uint64_t result;
            asm volatile (
                "neg %[nArg], %[result] \n\t"
                "xnor %[nArg], %[result], %%g5 \n\t"
                "popc %%g5, %[result] \n\t"
                "movrz %[nArg], %%g0, %[result] \n\t"
                : [result] "=r" (result)
                : [nArg] "r" (nArg)
                : "g5"
            );
            return result;
        }

        // MSB - return index (1..32) of most significant bit in nArg. If nArg == 0 return 0
        static inline int sparc_msb32( uint32_t nArg )
        {
            return sparc_msb64( (uint64_t) nArg );
        }

    }} // namespace gcc::Sparc

    using namespace gcc::Sparc;

}}}    // namespace cds::bitop::platform
//@endcond

#endif // #ifndef CDSLIB_COMPILER_GCC_SPARC_BITOP_H
