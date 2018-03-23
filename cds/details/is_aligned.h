// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_DETAILS_IS_ALIGNED_H
#define CDSLIB_DETAILS_IS_ALIGNED_H

#include <cds/details/defs.h>

namespace cds { namespace details {

    /// Checks if the pointer \p p has \p ALIGN byte alignment
    /**
        \p ALIGN must be power of 2.

        The function is mostly intended for run-time assertion
    */
    template <int ALIGN, typename T>
    static inline bool is_aligned(T const * p)
    {
        return (((uintptr_t)p) & uintptr_t(ALIGN - 1)) == 0;
    }

    /// Checks if the pointer \p p has \p nAlign byte alignment
    /**
        \p nAlign must be power of 2.

        The function is mostly intended for run-time assertion
    */
    template <typename T>
    static inline bool is_aligned(T const * p, size_t nAlign)
    {
        return (((uintptr_t)p) & uintptr_t(nAlign - 1)) == 0;
    }

}} // namespace cds::details

#endif // #ifndef CDSLIB_DETAILS_IS_ALIGNED_H
