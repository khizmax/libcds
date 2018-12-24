#ifndef CDSLIB_COMPILER_TS_HARDWARETIMESTAMP_H
#define CDSLIB_COMPILER_TS_HARDWARETIMESTAMP_H

// Choose appropriate header for current architecture and compiler

#if CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG || CDS_COMPILER == CDS_COMPILER_INTEL
#   if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#       include <cds/compiler/gcc/x86/ts_hardwaretimestamp.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_AMD64
#       include <cds/compiler/gcc/amd64/ts_hardwaretimestamp.h>
#   endif
#else
#   error "Undefined compiler"
#endif

#endif // #ifndef CDSLIB_COMPILER_TS_HARDWARETIMESTAMP_H
