//$$CDS-header$$

#include "map2/map_find_string.h"
#include "map2/map_type_striped.h"

namespace map2 {
    CDSUNIT_DEFINE_RefinableMap(cc::striped_set::implementation_tag, Map_find_string)

    CPPUNIT_TEST_SUITE_PART( Map_find_string, run_RefinableMap )
        CDSUNIT_TEST_RefinableMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
