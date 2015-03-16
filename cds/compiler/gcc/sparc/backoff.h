//$$CDS-header$$

#ifndef CDSLIB_COMPILER_GCC_SPARC_BACKOFF_H
#define CDSLIB_COMPILER_GCC_SPARC_BACKOFF_H

//@cond none

namespace cds { namespace backoff {
    namespace gcc { namespace Sparc {

#       define CDS_backoff_pause_defined
        static inline void backoff_pause( unsigned int nLoop = 0x000003FF )
        {}

#       define CDS_backoff_nop_defined
        static inline void backoff_nop()
        {
            asm volatile ( "nop;" );
        }


    }} // namespace gcc::Sparc

    namespace platform {
        using namespace gcc::Sparc;
    }
}}  // namespace cds::backoff

//@endcond
#endif  // #ifndef CDSLIB_COMPILER_GCC_SPARC_BACKOFF_H
