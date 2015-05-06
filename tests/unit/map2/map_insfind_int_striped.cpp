//$$CDS-header$$

#include "map2/map_insfind_int.h"
#include "map2/map_type_striped.h"

namespace map2 {
    CDSUNIT_DEFINE_StripedMap(cc::striped_set::implementation_tag, Map_InsFind_int)

    CPPUNIT_TEST_SUITE_PART( Map_InsFind_int, run_StripedMap )
        CDSUNIT_TEST_StripedMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
