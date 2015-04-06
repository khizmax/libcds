//$$CDS-header$$

#include "set2/set_insdel_string.h"
#include "set2/set_type_skip_list.h"

namespace set2 {
    CDSUNIT_DEFINE_SkipListSet(cc::skip_list::implementation_tag, Set_InsDel_string)

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_string, run_SkipListSet )
        CDSUNIT_TEST_SkipListSet
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace set2
