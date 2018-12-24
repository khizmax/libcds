#ifndef CDSLIB_COMPILER_GCC_X86_TS_HARDWARETIMESTAMP_H
#define CDSLIB_COMPILER_GCC_X86_TS_HARDWARETIMESTAMP_H

#include <cstdint>

namespace cds { namespace tshardwaretimestamp {
    namespace gcc { namespace x86 {

#       define CDS_ts_hardwaretimestamp_hwptime_defined
        static inline uint64_t get_hwptime()
        {
            uint64_t aux;
            uint64_t rax, rdx;
            asm volatile ("rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (aux) : : );
            return (rdx << 32) + rax;
        }

#       define CDS_ts_hardwaretimestamp_hwtime_defined
        static inline uint64_t get_hwtime()
        {
            unsigned int hi, lo;
            __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
            return ((uint64_t)lo) | (((uint64_t)hi) << 32);
        }

    }} // namespace gcc::x86

    namespace platform {
        using namespace gcc::x86;
    }
}}  // namespace cds::tshardwaretimestamp

#endif // #ifndef CDSLIB_COMPILER_GCC_X86_TS_HARDWARETIMESTAMP_H
