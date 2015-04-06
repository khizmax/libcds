//$$CDS-header$$

#include "set2/set_insdel_func.h"
#include "set2/set_type_striped.h"

namespace set2 {
    CDSUNIT_DEFINE_RefinableSet( cc::striped_set::implementation_tag, Set_InsDel_func )

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_func, run_RefinableSet )
        CDSUNIT_TEST_RefinableSet
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
