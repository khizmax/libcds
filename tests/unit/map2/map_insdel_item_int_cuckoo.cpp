//$$CDS-header$$

#include "map2/map_insdel_item_int.h"
#include "map2/map_type_cuckoo.h"

namespace map2 {
    CDSUNIT_DEFINE_CuckooMap(cds::intrusive::cuckoo::implementation_tag, Map_InsDel_Item_int)

    CPPUNIT_TEST_SUITE_PART( Map_InsDel_Item_int, run_CuckooMap )
        CDSUNIT_TEST_CuckooMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
