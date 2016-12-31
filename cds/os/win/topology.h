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

#ifndef CDSLIB_OS_WIN_TOPOLOGY_H
#define CDSLIB_OS_WIN_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <cds/details/defs.h>
#ifndef NOMINMAX
#   define NOMINMAX
#endif
#include <windows.h>

namespace cds { namespace OS {

    /// Windows-specific wrappers
    CDS_CXX11_INLINE_NAMESPACE namespace Win32 {

        /// System topology
        /**
            The implementation assumes that:
                \li the system has no more than 64 logical processors;
                \li processor IDs are in numerical order from 0 to N - 1, where N - count of processor in the system
        */
        struct CDS_EXPORT_API topology
        {
#   if _WIN32_WINNT >= 0x0601       // >= Windows 7
            static unsigned int    processor_count()
            {
                return ::GetActiveProcessorCount( ALL_PROCESSOR_GROUPS );
            }
#   else    // < Windows 7
            /// Logical processor count for the system
            static unsigned int processor_count();
#   endif

#   if _WIN32_WINNT >= 0x0600   // >= Windows Vista
            static unsigned int current_processor()
            {
                return ::GetCurrentProcessorNumber();
            }
#   else    // < Windows Vista
            /// Get current processor number
            static unsigned int current_processor();
#   endif

            /// Synonym for \ref current_processor
            static unsigned int native_current_processor()
            {
                return current_processor();
            }

            //@cond
            static void init()
            {}
            static void fini()
            {}
            //@endcond
        };

    }   // namespace Win32

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using Win32::topology;
#endif
}}  // namespace cds::OS



#endif  // #ifndef CDSLIB_OS_WIN_TOPOLOGY_H
