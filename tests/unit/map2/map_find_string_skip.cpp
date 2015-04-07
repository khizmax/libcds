//$$CDS-header$$

#include "map2/map_find_string.h"
#include "map2/map_type_skip_list.h"

namespace map2 {
    CDSUNIT_DEFINE_SkipListMap( cc::skip_list::implementation_tag, Map_find_string)
    CDSUNIT_DEFINE_SkipListMap_nogc( cc::skip_list::implementation_tag, Map_find_string)

    CPPUNIT_TEST_SUITE_PART( Map_find_string, run_SkipListMap )
        CDSUNIT_TEST_SkipListMap
        CDSUNIT_TEST_SkipListMap_nogc
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
