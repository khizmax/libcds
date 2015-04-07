//$$CDS-header$$

#include "map2/map_insdel_item_int.h"
#include "map2/map_type_striped.h"

namespace map2 {
    CDSUNIT_DEFINE_RefinableMap(cc::striped_set::implementation_tag, Map_InsDel_Item_int)

    CPPUNIT_TEST_SUITE_PART( Map_InsDel_Item_int, run_RefinableMap )
        CDSUNIT_TEST_RefinableMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
