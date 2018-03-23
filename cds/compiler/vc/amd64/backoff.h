// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_VC_AMD64_BACKOFF_H
#define CDSLIB_COMPILER_VC_AMD64_BACKOFF_H

//@cond none
#include <intrin.h>

namespace cds { namespace backoff {
    namespace vc { namespace amd64 {

#       define CDS_backoff_hint_defined
        static inline void backoff_hint()
        {
            _mm_pause();
        }

#       define CDS_backoff_nop_defined
        static inline void backoff_nop()
        {
            __nop();
        }

    }} // namespace vc::amd64

    namespace platform {
        using namespace vc::amd64;
    }
}}  // namespace cds::backoff

//@endcond
#endif  // #ifndef CDSLIB_COMPILER_VC_AMD64_BACKOFF_H
