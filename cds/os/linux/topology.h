// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_LINUX_TOPOLOGY_H
#define CDSLIB_OS_LINUX_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <cds/details/defs.h>
#include <cds/threading/model.h>

#include <sys/syscall.h>
#include <sched.h>

namespace cds { namespace OS {
    /// Linux-specific wrappers
    inline namespace Linux {

        /// System topology
        /**
            The implementation assumes that processor IDs are in numerical order
            from 0 to N - 1, where N - count of processor in the system
        */
        struct topology {
        private:
            //@cond
            static unsigned int     s_nProcessorCount;
            //@endcond
        public:

            /// Logical processor count for the system
            static unsigned int processor_count()
            {
                return s_nProcessorCount;
            }

            /// Get current processor number
            /**
                Caveat: \p current_processor calls system \p sched_getcpu function
                that may not be defined for target system (\p sched_getcpu is available since glibc 2.6).
                If \p sched_getcpu is not defined the function emulates "current processor number" using
                thread-specific data. You may manually disable the \p sched_getcpu usage compiling with
                <tt>-DCDS_LINUX_NO_sched_getcpu</tt>.
            */
            static unsigned int current_processor()
            {
            // Compile libcds with -DCDS_LINUX_NO_sched_getcpu if your linux does not have sched_getcpu (glibc version less than 2.6)
#           if !defined(CDS_LINUX_NO_sched_getcpu) && defined(SYS_getcpu)
                int nProcessor = ::sched_getcpu();
                return nProcessor == -1 ? 0 : (unsigned int) nProcessor;
#           else
                // Use fake "current processor number" assigned for current thread
                return (unsigned int) threading::Manager::fake_current_processor();

                /*
                Another way (for x86/amd64 only)

                Using cpuid and APIC ID (http://www.scss.tcd.ie/Jeremy.Jones/GetCurrentProcessorNumberXP.htm)
                for x86 and amd64
                {
                    _asm {mov eax, 1}
                    _asm {cpuid}
                    _asm {shr ebx, 24}
                    _asm {mov eax, ebx}
                }
                However:
                    - cpuid is full sync barrier
                    - it is only for x86/amd64 architecture
                */
#           endif
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
    }   // namespace Linux

}}  // namespace cds::OS

#endif  // #ifndef CDSLIB_OS_LINUX_TOPOLOGY_H
