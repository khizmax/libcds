//$$CDS-header$$

#include "set2/set_insdel_string.h"
#include "set2/set_type_striped.h"

namespace set2 {
    CDSUNIT_DEFINE_StripedSet( cc::striped_set::implementation_tag, Set_InsDel_string )

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_string, run_StripedSet )
        CDSUNIT_TEST_StripedSet
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace set2
