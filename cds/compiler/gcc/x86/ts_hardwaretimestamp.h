#ifndef CDSLIB_COMPILER_GCC_X86_TS_HARDWARETIMESTAMP_H
#define CDSLIB_COMPILER_GCC_X86_TS_HARDWARETIMESTAMP_H

#include <cstdint>
#include <cpuid.h>

namespace cds { namespace tshardwaretimestamp {
    namespace gcc { namespace x86 {

#       define CDS_ts_hardwaretimestamp_hwptime_defined
        static inline uint64_t get_hwptime()
        {
            uint64_t ret;
            __asm__ volatile ("rdtscp\n" : "=A" (ret) : : "ecx");
            return ret;
        }

#       define CDS_ts_hardwaretimestamp_hwtime_defined
        static inline uint64_t get_hwtime()
        {
            uint64_t ret;
            __asm__ volatile("rdtsc" : "=A"(ret));
            return ret;
        }

        static inline int has_rdtscp()
        {
            unsigned int eax, ebx, ecx, edx;
            if (__get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx))
                return (edx >> 27) & 0x1;
            else
                return 0;
        }

    }} // namespace gcc::x86

    namespace platform {
        using namespace gcc::x86;
    }
}}  // namespace cds::tshardwaretimestamp

#endif // #ifndef CDSLIB_COMPILER_GCC_X86_TS_HARDWARETIMESTAMP_H
