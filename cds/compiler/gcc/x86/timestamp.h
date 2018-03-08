
#ifndef CDSLIB_COMPILER_GCC_X86_TIMESTAMP_H
#define CDSLIB_COMPILER_GCC_X86_TIMESTAMP_H
namespace cds { namespace timestamp {
    namespace gcc { namespace x86 {

#       define CDS_timestamp_defined
        inline unsigned long getTimestamp ()
		{
			uint32_t time_edx1, time_eax1;
			unsigned long time_last;
			asm volatile (  "rdtscp\n\t"
							"mov %%edx, %0\n\t"
							"mov %%eax, %1\n\t"
							"cpuid\n\t" : "=r"(time_edx1), "=r"(time_eax1) ::
							"%rax", "%rbx", "%rcx", "%rdx");

			time_last =
						(static_cast<unsigned long long>(time_edx1) << 32 | static_cast<unsigned long long>(time_eax1));
			return time_last;
		}

    }} // namespace gcc::x86

    namespace platform {
        using namespace gcc::x86;
    }
}}  // namespace cds::timestamp

#endif //CDSLIB_COMPILER_GCC_X86_TIMESTAMP_H
