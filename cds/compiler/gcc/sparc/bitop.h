// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
