//$$CDS-header$$

#ifndef CDSLIB_OS_SUNOS_TOPOLOGY_H
#define CDSLIB_OS_SUNOS_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <sys/processor.h>
#include <unistd.h>

namespace cds { namespace OS {

    /// Sun Solaris-specific wrappers
    CDS_CXX11_INLINE_NAMESPACE namespace Sun {

        /// System topology
        /**
            The implementation assumes that the processor IDs are in numerical order
            from 0 to N - 1, where N - count of processor in the system
        */
        struct topology {
            /// Logical processor count for the system
            static unsigned int processor_count()
            {
                // Maybe, _SC_NPROCESSORS_ONLN? But _SC_NPROCESSORS_ONLN may change dynamically...
                long nCount = ::sysconf(_SC_NPROCESSORS_CONF);
                if ( nCount == -1 )
                    return  1;
                return (unsigned int) nCount;
            }

            /// Get current processor number
            static unsigned int current_processor()
            {
                return  ::getcpuid();
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
    }   // namespace Sun

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    using Sun::topology;
#endif
}}  // namespace cds::OS

#endif  // #ifndef CDSLIB_OS_SUNOS_TOPOLOGY_H
