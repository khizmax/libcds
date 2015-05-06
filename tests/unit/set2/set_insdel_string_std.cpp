//$$CDS-header$$

#include "set2/set_insdel_string.h"
#include "set2/set_type_std.h"

namespace set2 {
    CDSUNIT_DEFINE_StdSet(std_implementation_tag, Set_InsDel_string)

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_string, run_StdSet )
        CDSUNIT_TEST_StdSet
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace set2
