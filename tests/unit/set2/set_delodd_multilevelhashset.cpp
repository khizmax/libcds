//$$CDS-header$$

#include "set2/set_delodd.h"
#include "set2/set_type_multilevel_hashset.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Set_DelOdd::X() { run_test_extract<typename set_type< TAG, key_type, value_type>::X>(); }
#include "set2/set_defs.h"

namespace set2 {
    CDSUNIT_DECLARE_MultiLevelHashSet_fixed
    CDSUNIT_DECLARE_MultiLevelHashSet_city
} // namespace set2
