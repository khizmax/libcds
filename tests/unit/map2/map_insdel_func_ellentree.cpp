//$$CDS-header$$

#include "map2/map_insdel_func.h"
#include "map2/map_type_ellen_bintree.h"

namespace map2 {
    CDSUNIT_DEFINE_EllenBinTreeMap( cc::ellen_bintree::implementation_tag, Map_InsDel_func)

    CPPUNIT_TEST_SUITE_PART( Map_InsDel_func, run_EllenBinTreeMap )
        CDSUNIT_TEST_EllenBinTreeMap
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace map2

