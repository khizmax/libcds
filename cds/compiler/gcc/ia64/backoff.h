// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_IA64_BACKOFF_H
#define CDSLIB_COMPILER_GCC_IA64_BACKOFF_H

//@cond none

namespace cds { namespace backoff {
    namespace gcc { namespace ia64 {

#       define CDS_backoff_hint_defined
        static inline void backoff_hint()
        {
            asm volatile ( "hint @pause;;" );
        }

#       define CDS_backoff_nop_defined
        static inline void backoff_nop()
        {
            asm volatile ( "nop;;" );
        }

    }} // namespace gcc::ia64

    namespace platform {
        using namespace gcc::ia64;
    }
}}  // namespace cds::backoff

//@endcond
#endif  // #ifndef CDSLIB_COMPILER_GCC_IA64_BACKOFF_H
