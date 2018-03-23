// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
    inline namespace Aix {

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

}}  // namespace cds::OS

#endif  // #ifndef CDSLIB_OS_AIX_TOPOLOGY_H
