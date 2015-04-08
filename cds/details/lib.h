//$$CDS-header$$

#ifndef CDSLIB_LIB_H
#define CDSLIB_LIB_H
//@cond

#include <cds/details/defs.h>

#ifndef CDS_BUILD_LIB

#ifdef _DEBUG
#   define  CDS_LIB_DEBUG_SUFFIX "_debug"
#else
#   define  CDS_LIB_DEBUG_SUFFIX ""
#endif

#if CDS_COMPILER == CDS_COMPILER_MSVC
#   pragma comment( lib, "libcds-" CDS_PROCESSOR__NICK "-" CDS_COMPILER__NICK CDS_LIB_DEBUG_SUFFIX )
#elif CDS_COMPILER == CDS_COMPILER_INTEL
#   pragma comment( lib, "libcds-" CDS_PROCESSOR__NICK "-" CDS_COMPILER__NICK CDS_LIB_DEBUG_SUFFIX )
#endif

#undef CDS_LIB_DEBUG_SUFFIX

#endif // #ifndef CDS_BUILD_LIB

//@endcond
#endif // #ifndef CDSLIB_LIB_H
