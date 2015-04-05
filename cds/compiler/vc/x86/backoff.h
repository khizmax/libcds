//$$CDS-header$$

#ifndef CDSLIB_COMPILER_VC_X86_BACKOFF_H
#define CDSLIB_COMPILER_VC_X86_BACKOFF_H

//@cond none

namespace cds { namespace backoff {
    namespace vc { namespace x86 {

#       define CDS_backoff_pause_defined
        static inline void backoff_pause( unsigned int nLoop = 0x000003FF )
        {
            __asm {
                and ecx, nLoop;
                cmovz ecx, nLoop;
                rep nop;
            }
        }

#       define CDS_backoff_nop_defined
        static inline void backoff_nop()
        {
            __nop();
        }

    }} // namespace vc::x86

    namespace platform {
        using namespace vc::x86;
    }
}}  // namespace cds::backoff

//@endcond
#endif  // #ifndef CDSLIB_COMPILER_VC_X86_BACKOFF_H
