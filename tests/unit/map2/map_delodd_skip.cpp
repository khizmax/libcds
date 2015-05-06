//$$CDS-header$$

#include "map2/map_delodd.h"
#include "map2/map_type_skip_list.h"

namespace map2 {
    CDSUNIT_DEFINE_SkipListMap( cc::skip_list::implementation_tag, Map_DelOdd)

    CPPUNIT_TEST_SUITE_PART( Map_DelOdd, run_SkipListMap )
        CDSUNIT_TEST_SkipListMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
