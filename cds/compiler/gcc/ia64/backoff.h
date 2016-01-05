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

#ifndef CDSLIB_COMPILER_GCC_IA64_BACKOFF_H
#define CDSLIB_COMPILER_GCC_IA64_BACKOFF_H

//@cond none

namespace cds { namespace backoff {
    namespace gcc { namespace ia64 {

#       define CDS_backoff_pause_defined
        static inline void backoff_pause( unsigned int nLoop = 0x000003FF )
        {
            asm volatile ( "hint @pause" );
        }

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
