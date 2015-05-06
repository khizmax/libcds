//$$CDS-header$$

#include "map2/map_find_string.h"
#include "map2/map_type_michael.h"

namespace map2 {
    CDSUNIT_DEFINE_MichaelMap( cc::michael_map::implementation_tag, Map_find_string )
    CDSUNIT_DEFINE_MichaelMap_nogc( cc::michael_map::implementation_tag, Map_find_string )

    CPPUNIT_TEST_SUITE_PART( Map_find_string, run_MichaelMap )
        CDSUNIT_TEST_MichaelMap
        CDSUNIT_TEST_MichaelMap_nogc
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
