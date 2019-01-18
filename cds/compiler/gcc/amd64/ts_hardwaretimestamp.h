#ifndef CDSLIB_COMPILER_GCC_AMD64_TS_HARDWARETIMESTAMP_H
#define CDSLIB_COMPILER_GCC_AMD64_TS_HARDWARETIMESTAMP_H

#include <cstdint>
#include <cpuid.h>

namespace cds { namespace tshardwaretimestamp {
    namespace gcc { namespace amd64 {

#       define CDS_ts_hardwaretimestamp_hwptime_defined
        static inline uint64_t get_hwptime()
        {
            uint64_t aux;
            uint64_t rax, rdx;
            __asm__ volatile ("rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (aux) : : );
            return (rdx << 32) + rax;
        }

#       define CDS_ts_hardwaretimestamp_hwtime_defined
        static inline uint64_t get_hwtime()
        {
            uint64_t high, low;
            __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
            return ((uint64_t)low) | (((uint64_t)high) << 32);
        }

        static inline int has_rdtscp()
        {
            unsigned int eax, ebx, ecx, edx;
            if (__get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx))
                return (edx >> 27) & 0x1;
            else
                return 0;
        }

    }} // namespace gcc::amd64

    namespace platform {
        using namespace gcc::amd64;
    }
}}  // namespace cds::tshardwaretimestamp

#endif // #ifndef CDSLIB_COMPILER_GCC_AMD64_TS_HARDWARETIMESTAMP_H
