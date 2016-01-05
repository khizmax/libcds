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

#ifndef CDSLIB_OS_DETAILS_FAKE_TOPOLOGY_H
#define CDSLIB_OS_DETAILS_FAKE_TOPOLOGY_H

#include <cds/details/defs.h>
#include <cds/threading/model.h>

//@cond
namespace cds { namespace OS { namespace details {

    /// Fake system topology
    struct fake_topology {
        /// Logical processor count for the system
        static unsigned int processor_count()
        {
            return 1;
        }

        /// Get current processor number
        /**
            The function emulates "current processor number" using thread-specific data.
        */
        static unsigned int current_processor()
        {
            // Use fake "current processor number" assigned for current thread
            return (unsigned int) threading::Manager::fake_current_processor();
        }

        /// Synonym for \ref current_processor
        static unsigned int native_current_processor()
        {
            return current_processor();
        }
    };
}}}  // namespace cds::OS::details
//@endcond

#endif  // #ifndef CDSLIB_OS_DETAILS_FAKE_TOPOLOGY_H
