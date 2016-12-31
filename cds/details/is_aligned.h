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
