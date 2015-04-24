//$$CDS-header$$

#ifndef CDSLIB_OS_WIN_TOPOLOGY_H
#define CDSLIB_OS_WIN_TOPOLOGY_H

#ifndef CDSLIB_OS_TOPOLOGY_H
#   error "<cds/os/topology.h> must be included instead"
#endif

#include <cds/details/defs.h>
#define NOMINMAX
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
