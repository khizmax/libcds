//$$CDS-header$$

#include "set2/set_insdel_string.h"
#include "set2/set_type_multilevel_hashset.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Set_InsDel_string::X() { run_test_extract<typename set_type< TAG, key_type, value_type>::X>(); }
#include "set2/set_defs.h"

namespace set2 {
    CDSUNIT_DECLARE_MultiLevelHashSet
} // namespace set2
