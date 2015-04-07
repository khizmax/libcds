//$$CDS-header$$

#include "map2/map_insdelfind.h"
#include "map2/map_type_split_list.h"

namespace map2 {
    CDSUNIT_DEFINE_SplitList( cc::split_list::implementation_tag, Map_InsDelFind )

    CPPUNIT_TEST_SUITE_PART( Map_InsDelFind, run_SplitList )
        CDSUNIT_TEST_SplitList
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
