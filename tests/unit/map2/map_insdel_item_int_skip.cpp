//$$CDS-header$$

#include "map2/map_insdel_item_int.h"
#include "map2/map_type_skip_list.h"

namespace map2 {
    CDSUNIT_DEFINE_SkipListMap( cc::skip_list::implementation_tag, Map_InsDel_Item_int)

    CPPUNIT_TEST_SUITE_PART( Map_InsDel_Item_int, run_SkipListMap )
        CDSUNIT_TEST_SkipListMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
