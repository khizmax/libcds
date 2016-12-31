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

#ifndef CDSLIB_ALGO_ELIMINATION_OPT_H
#define CDSLIB_ALGO_ELIMINATION_OPT_H

#include <cds/details/defs.h>

namespace cds { namespace opt {

    /// Enable \ref cds_elimination_description "elimination back-off" for the container
    template <bool Enable>
    struct enable_elimination {
        //@cond
        template <class Base> struct pack: public Base
        {
            static CDS_CONSTEXPR const bool enable_elimination = Enable;
        };
        //@endcond
    };

    /// \ref cds_elimination_description "Elimination back-off strategy" option setter
    /**
        Back-off strategy for elimination.
        Usually, elimination back-off strategy is \p cds::backoff::delay.
    */
    template <typename Type>
    struct elimination_backoff {
        //@cond
        template <class Base> struct pack: public Base
        {
            typedef Type elimination_backoff;
        };
        //@endcond
    };
}} // namespace cds::opt

#endif // #ifndef CDSLIB_ALGO_ELIMINATION_OPT_H
