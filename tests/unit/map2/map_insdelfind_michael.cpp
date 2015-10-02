//$$CDS-header$$

#include "map2/map_insdelfind.h"
#include "map2/map_type_michael.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Map_InsDelFind::X() { run_test<typename map_type< TAG, key_type, value_type>::X>(); }
#include "map2/map_defs.h"

namespace map2 {
    CDSUNIT_DECLARE_MichaelMap
} // namespace map2
