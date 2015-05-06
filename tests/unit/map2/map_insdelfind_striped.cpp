//$$CDS-header$$

#include "map2/map_insdelfind.h"
#include "map2/map_type_striped.h"

namespace map2 {
    CDSUNIT_DEFINE_StripedMap(cc::striped_set::implementation_tag, Map_InsDelFind)

    CPPUNIT_TEST_SUITE_PART( Map_InsDelFind, run_StripedMap )
        CDSUNIT_TEST_StripedMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
