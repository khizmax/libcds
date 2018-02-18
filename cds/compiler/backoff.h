// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_BACKOFF_IMPL_H
#define CDSLIB_COMPILER_BACKOFF_IMPL_H

#include <cds/details/defs.h>

#if CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)
#   if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#       include <cds/compiler/vc/x86/backoff.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_AMD64
#       include <cds/compiler/vc/amd64/backoff.h>
#   else
#       error "MS VC++ compiler: unsupported processor architecture"
#   endif
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG || CDS_COMPILER == CDS_COMPILER_INTEL
#   if CDS_PROCESSOR_ARCH == CDS_PROCESSOR_X86
#       include <cds/compiler/gcc/x86/backoff.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_AMD64
#       include <cds/compiler/gcc/amd64/backoff.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_IA64
#       include <cds/compiler/gcc/ia64/backoff.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_SPARC
#       include <cds/compiler/gcc/sparc/backoff.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_PPC64
#       include <cds/compiler/gcc/ppc64/backoff.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_ARM7
#       include <cds/compiler/gcc/arm7/backoff.h>
#   elif CDS_PROCESSOR_ARCH == CDS_PROCESSOR_ARM8
#       include <cds/compiler/gcc/arm8/backoff.h>
#   endif
#else
#   error "Undefined compiler"
#endif

#endif  // #ifndef CDSLIB_COMPILER_BACKOFF_IMPL_H
