//$$CDS-header$$

#include "set2/set_insdel_func.h"
#include "set2/set_type_skip_list.h"

#undef TEST_CASE
#define TEST_CASE(TAG, X)  void Set_InsDel_func::X() { run_test<typename set_type< TAG, key_type, value_type>::X>(); }
#include "set2/set_defs.h"

namespace set2 {
    CDSUNIT_DECLARE_SkipListSet
} // namespace set2
