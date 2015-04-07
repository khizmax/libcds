//$$CDS-header$$

#include "map2/map_delodd.h"
#include "map2/map_type_michael.h"

namespace map2 {
    CDSUNIT_DEFINE_MichaelMap( cc::michael_map::implementation_tag, Map_DelOdd )

    CPPUNIT_TEST_SUITE_PART( Map_DelOdd, run_MichaelMap )
        CDSUNIT_TEST_MichaelMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
