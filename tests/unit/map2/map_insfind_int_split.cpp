//$$CDS-header$$

#include "map2/map_insfind_int.h"
#include "map2/map_type_split_list.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Map_InsFind_int::X() { run_test<typename map_type< TAG, key_type, value_type>::X>(); }
#include "map2/map_defs.h"

namespace map2 {
    CDSUNIT_DECLARE_SplitList
    CDSUNIT_DECLARE_SplitList_nogc
} // namespace map2
