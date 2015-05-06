//$$CDS-header$$

#include "map2/map_insfind_int.h"
#include "map2/map_type_michael.h"

namespace map2 {
    CDSUNIT_DEFINE_MichaelMap( cc::michael_map::implementation_tag, Map_InsFind_int )
    CDSUNIT_DEFINE_MichaelMap_nogc( cc::michael_map::implementation_tag, Map_InsFind_int )

    CPPUNIT_TEST_SUITE_PART( Map_InsFind_int, run_MichaelMap )
        CDSUNIT_TEST_MichaelMap
        CDSUNIT_TEST_MichaelMap_nogc
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
