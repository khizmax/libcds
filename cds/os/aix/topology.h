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

#ifndef CDSLIB_OS_AIX_TOPOLOGY_H
#define CDSLIB_OS_AIX_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <cds/os/details/fake_topology.h>

#include <unistd.h>

#ifdef _AIX53
#   include <sys/processor.h>
#endif

namespace cds { namespace OS {

    ///  IBM AIX specific wrappers
    CDS_CXX11_INLINE_NAMESPACE namespace Aix {

        /// System topology
        /**
            The implementation assumes that the processor IDs are in numerical order
            from 0 to N - 1, where N - count of processor in the system
        */
        struct topology: public OS::details::fake_topology
        {
        private:
            //@cond
            typedef OS::details::fake_topology  base_class;
            //@endcond
        public:
            /// Logical processor count for the system
            static unsigned int processor_count()
            {
                long nCount = ::sysconf(_SC_NPROCESSORS_ONLN);
                if ( nCount == -1 )
                    return  1;
                return (unsigned int) nCount;
            }

            /// Get current processor number
            /**
                Caveat: AIX has no "get current processor number" system call.
                The function emulates "current processor number" using thread-specific data.
            */
            static unsigned int current_processor()
            {
#           if defined(_AIX53) && _AIX53 == 1
                // AIX 5.3 has undocumented function mycpu() in sys/processor.h
                return ::mycpu();
#           else
                return base_class::current_processor();
#           endif
            }

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
    }   // namespace Aix

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using Aix::topology;
#endif
}}  // namespace cds::OS

#endif  // #ifndef CDSLIB_OS_AIX_TOPOLOGY_H
