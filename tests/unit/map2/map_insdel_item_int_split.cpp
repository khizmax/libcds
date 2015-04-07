//$$CDS-header$$

#include "map2/map_insdel_item_int.h"
#include "map2/map_type_split_list.h"

namespace map2 {
    CDSUNIT_DEFINE_SplitList( cc::split_list::implementation_tag, Map_InsDel_Item_int )

    CPPUNIT_TEST_SUITE_PART( Map_InsDel_Item_int, run_SplitList )
        CDSUNIT_TEST_SplitList
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
