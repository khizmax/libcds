//$$CDS-header$$

#ifndef CDSLIB_COMPILER_GCC_X86_BACKOFF_H
#define CDSLIB_COMPILER_GCC_X86_BACKOFF_H

//@cond none

namespace cds { namespace backoff {
    namespace gcc { namespace x86 {

#       define CDS_backoff_pause_defined
        static inline void backoff_pause( unsigned int nLoop = 0x000003FF )
        {
            asm volatile (
                "andl %[nLoop], %%ecx;      \n\t"
                "cmovzl %[nLoop], %%ecx;    \n\t"
                "rep; "
                "nop;   \n\t"
                : /*no output*/
                : [nLoop] "r" (nLoop)
                : "ecx", "cc"
                );
        }

#       define CDS_backoff_nop_defined
        static inline void backoff_nop()
        {
            asm volatile ( "nop;" );
        }

    }} // namespace gcc::x86

    namespace platform {
        using namespace gcc::x86;
    }
}}  // namespace cds::backoff

//@endcond
#endif  // #ifndef CDSLIB_COMPILER_GCC_X86_BACKOFF_H
