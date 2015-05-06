//$$CDS-header$$

#include "map2/map_insfind_int.h"
#include "map2/map_type_split_list.h"

namespace map2 {
    CDSUNIT_DEFINE_SplitList( cc::split_list::implementation_tag, Map_InsFind_int )
    CDSUNIT_DEFINE_SplitList_nogc( cc::split_list::implementation_tag, Map_InsFind_int )

    CPPUNIT_TEST_SUITE_PART( Map_InsFind_int, run_SplitList )
        CDSUNIT_TEST_SplitList
        CDSUNIT_TEST_SplitList_nogc
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2
