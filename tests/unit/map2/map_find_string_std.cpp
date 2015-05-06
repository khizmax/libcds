//$$CDS-header$$

#include "map2/map_find_string.h"
#include "map2/map_type_std.h"

namespace map2 {
    CDSUNIT_DEFINE_StdMap( map2::std_implementation_tag, Map_find_string)

    CPPUNIT_TEST_SUITE_PART( Map_find_string, run_StdMap )
        CDSUNIT_TEST_StdMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
