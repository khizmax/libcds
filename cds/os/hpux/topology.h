// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_HPUX_TOPOLOGY_H
#define CDSLIB_OS_HPUX_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <sys/mpctl.h>

namespace cds { namespace OS {
    /// HP-UX-specific wrappers
    inline namespace Hpux {

        /// System topology
        struct topology {
        public:
            /// Logical processor count for the system
            static unsigned int processor_count()
            {
                return ::mpctl( MPC_GETNUMSPUS_SYS, 0, 0 );
            }

            /// Get current processor number
            /**
                In HP-UX, processor and locality domain IDs are not guaranteed to exist
                in numerical order. There may be holes in a sequential list of IDs.

                This function guarantees sequence of processor numbers using internal processor ID mapping.
                It returns sequential processor number, ie a number from 0 to N - 1, where N - processor count.
            */
            static unsigned int current_processor()
            {
                unsigned int nProc = native_current_processor();
                if ( nProc < s_nProcMapSize )
                    return s_procMap[ nProc ].nProcNo;
                assert( false );
                return 0;
            }

            /// Get OS-specific current processor number
            /**
                The function returns OS-provided processor number.
                Processor and locality domain IDs are not guaranteed to exist
                in numerical order.  There may be holes in a sequential list of IDs.
            */
            static unsigned int native_current_processor()
            {
                return ::mpctl( MPC_GETCURRENTSPU, 0, 0 );
            }

            //@cond
            static void init();
            static void fini();
            //@endcond

        private:
            //@cond
            struct processor_map {
                unsigned int    nProcNo;
                spu_t           nNativeProcNo;
                unsigned int    nCell;

            };
            static processor_map *  s_procMap;
            static size_t           s_nProcMapSize;

            static void make_processor_map();

            //@endcond
        };
    }   // namespace Hpux

}}  // namespace cds::OS

#endif  // #ifndef CDSLIB_OS_HPUX_TOPOLOGY_H
