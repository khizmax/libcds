//$$CDS-header$$

#ifndef __CDS_COMPILER_CXX11_ATOMIC_PATCHES_H
#define __CDS_COMPILER_CXX11_ATOMIC_PATCHES_H

#if CDS_COMPILER == CDS_COMPILER_GCC
#   include <cds/compiler/gcc/cxx11_atomic_patches.h>
#elif CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS
#   include <cds/compiler/icl/cxx11_atomic_patches_win.h>
#endif


#endif // #ifndef __CDS_COMPILER_CXX11_ATOMIC_PATCHES_H
