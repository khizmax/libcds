//$$CDS-header$$

#include "map2/map_insdel_string.h"
#include "map2/map_type_striped.h"

namespace map2 {
    CDSUNIT_DEFINE_StripedMap(cc::striped_set::implementation_tag, Map_InsDel_string)

    CPPUNIT_TEST_SUITE_PART( Map_InsDel_string, run_StripedMap )
        CDSUNIT_TEST_StripedMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
