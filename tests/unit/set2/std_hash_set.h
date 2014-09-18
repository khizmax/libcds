//$$CDS-header$$

#ifndef __CDSUNIT_STD_HASH_SET_H
#define __CDSUNIT_STD_HASH_SET_H

#if (CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL) && _MSC_VER == 1500
#   include "set2/std_hash_set_vc9.h"
#else
#   include "set2/std_hash_set_std.h"
#endif

#endif // #ifndef __CDSUNIT_STD_HASH_SET_H
