//$$CDS-header$$

#include "map2/map_find_string.h"
#include "map2/map_type_michael.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Map_find_string::X() { run_test<typename map_type< TAG, key_type, value_type>::X>(); }
#include "map2/map_defs.h"

namespace map2 {
    CDSUNIT_DECLARE_MichaelMap
    CDSUNIT_DECLARE_MichaelMap_nogc
} // namespace map2
