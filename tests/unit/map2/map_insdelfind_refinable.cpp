//$$CDS-header$$

#include "map2/map_insdelfind.h"
#include "map2/map_type_striped.h"

namespace map2 {
    CDSUNIT_DEFINE_RefinableMap(cc::striped_set::implementation_tag, Map_InsDelFind)

    CPPUNIT_TEST_SUITE_PART( Map_InsDelFind, run_RefinableMap )
        CDSUNIT_TEST_RefinableMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
