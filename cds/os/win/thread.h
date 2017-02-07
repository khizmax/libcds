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

#ifndef CDSLIB_OS_WIN_THREAD_H
#define CDSLIB_OS_WIN_THREAD_H

#ifndef NOMINMAX
#   define NOMINMAX
#endif
#include <windows.h>

namespace cds { namespace OS {
    /// Windows-specific functions
    CDS_CXX11_INLINE_NAMESPACE namespace Win32 {

        /// OS-specific type of thread identifier
        typedef DWORD           ThreadId;

        /// Get current thread id
        static inline ThreadId get_current_thread_id()
        {
            return ::GetCurrentThreadId();
        }
    }    // namespace Win32

    //@cond
    CDS_CONSTEXPR const Win32::ThreadId c_NullThreadId = 0;

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using Win32::ThreadId;
    using Win32::get_current_thread_id;
#endif
    //@endcond

}} // namespace cds::OS

#endif // #ifndef CDSLIB_OS_WIN_THREAD_H
