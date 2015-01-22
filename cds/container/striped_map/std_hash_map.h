//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_STRIPED_MAP_STD_HASH_MAP_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_MAP_STD_HASH_MAP_ADAPTER_H

#include <cds/container/striped_set/adapter.h>
#if (CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER < 1600    // MS VC 2008
#   include <cds/container/striped_map/std_hash_map_vc.h>
#else
#   include <cds/container/striped_map/std_hash_map_std.h>
#endif

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_MAP_STD_HASH_MAP_ADAPTER_H
