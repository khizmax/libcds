// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_POSIX_FAKE_TOPOLOGY_H
#define CDSLIB_OS_POSIX_FAKE_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <cds/os/details/fake_topology.h>
#include <unistd.h>

namespace cds { namespace OS {
    inline namespace posix {
        /// Fake system topology
        struct topology {
            /// Logical processor count for the system. Always returns 1
            static unsigned int processor_count()
            {
                return 1;
            }

            /// Get current processor number
            /**
                Always returns 0
            */
            static unsigned int current_processor()
            {
                return 0;
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
    }   // namespace posix

}}  // namespace cds::OS

#endif  // #ifndef CDSLIB_OS_POSIX_FAKE_TOPOLOGY_H
