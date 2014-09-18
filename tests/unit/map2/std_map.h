//$$CDS-header$$

#ifndef __CDSUNIT_STD_MAP_H
#define __CDSUNIT_STD_MAP_H

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   include "map2/std_map_vc.h"
#elif CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
#   include "map2/std_map_gcc.h"
#else
#   error "std::map is undefined for this compiler"
#endif

#endif // #ifndef __CDSUNIT_STD_MAP_H
