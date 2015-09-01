//$$CDS-header$$

#include "map2/map_delodd.h"
#include "map2/map_type_cuckoo.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Map_DelOdd::X() { run_test_no_extract<typename map_type< TAG, key_type, value_type>::X>(); }
#include "map2/map_defs.h"

namespace map2 {
    CDSUNIT_DECLARE_CuckooMap
} // namespace map2
