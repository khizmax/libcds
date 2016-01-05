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

#ifndef CDSLIB_USER_SETUP_CACHE_LINE_H
#define CDSLIB_USER_SETUP_CACHE_LINE_H

/** \file
    \brief Cache-line size definition

    This file defines cache-line size constant. The constant is used many \p libcds algorithm
    to solve false-sharing problem.

    The value of cache-line size must be power of two.
    You may define your cache-line size by defining macro -DCDS_CACHE_LINE_SIZE=xx
    (where \p xx is 2**N: 32, 64, 128,...) at compile-time or you may define your constant
    value just editing \p cds/user_setup/cache_line.h file.

    The default value is 64.
*/

//@cond
namespace cds {
#ifndef CDS_CACHE_LINE_SIZE
    static const size_t c_nCacheLineSize = 64;
#else
    static const size_t c_nCacheLineSize = CDS_CACHE_LINE_SIZE;
#endif
}   // namespace cds
//@endcond

#endif // #ifndef CDSLIB_USER_SETUP_CACHE_LINE_H
