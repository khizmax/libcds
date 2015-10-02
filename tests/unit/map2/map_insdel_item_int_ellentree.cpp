//$$CDS-header$$

#include "map2/map_insdel_item_int.h"
#include "map2/map_type_ellen_bintree.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Map_InsDel_Item_int::X() { run_test<typename map_type< TAG, key_type, value_type>::X>(); }
#include "map2/map_defs.h"

namespace map2 {
    CDSUNIT_DECLARE_EllenBinTreeMap
} // namespace map2
