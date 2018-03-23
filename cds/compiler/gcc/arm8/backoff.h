// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_ARM8_BACKOFF_H
#define CDSLIB_COMPILER_GCC_ARM8_BACKOFF_H

//@cond none

namespace cds { namespace backoff {
    namespace gcc { namespace arm8 {

#       define CDS_backoff_hint_defined
        static inline void backoff_hint()
        {
            asm volatile( "yield" ::: "memory" );
        }
    }} // namespace gcc::arm8

    namespace platform {
        using namespace gcc::arm8;
    }
}}  // namespace cds::backoff

//@endcond
#endif  // #ifndef CDSLIB_COMPILER_GCC_ARM8_BACKOFF_H
