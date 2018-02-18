// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_OSX_TOPOLOGY_H
#define CDSLIB_OS_OSX_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <cds/os/details/fake_topology.h>

namespace cds { namespace OS {
    /// OSX-specific wrappers
    inline namespace OS_X {

        /// System topology
        /**
            The implementation assumes that processor IDs are in numerical order
            from 0 to N - 1, where N - count of processor in the system
        */
        struct topology: public OS::details::fake_topology
        {
        private:
            //@cond
            typedef OS::details::fake_topology  base_class;
            static unsigned int     s_nProcessorCount;
            //@endcond
        public:

            /// Logical processor count for the system
            static unsigned int processor_count()
            {
                return s_nProcessorCount;
            }

            /// Get current processor number
            static unsigned int current_processor()
            {
                return base_class::current_processor();
            }

            /// Synonym for \ref current_processor
            static unsigned int native_current_processor()
            {
                return current_processor();
            }

            //@cond
            static void init();
            static void fini();
            //@endcond
        };
    }   // namespace OS_X

}}  // namespace cds::OS

#endif  // #ifndef CDSLIB_OS_OSX_TOPOLOGY_H
