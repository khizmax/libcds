//$$CDS-header$$

#include "map2/map_find_string.h"
#include "map2/map_type_cuckoo.h"

namespace map2 {
    CDSUNIT_DEFINE_CuckooMap(cds::intrusive::cuckoo::implementation_tag, Map_find_string)

    CPPUNIT_TEST_SUITE_PART( Map_find_string, run_CuckooMap )
        CDSUNIT_TEST_CuckooMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
