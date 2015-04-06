//$$CDS-header$$

#include "set2/set_insdelfind.h"
#include "set2/set_type_striped.h"

namespace set2 {
    CDSUNIT_DEFINE_RefinableSet( cc::striped_set::implementation_tag, Set_InsDelFind )

    CPPUNIT_TEST_SUITE_PART( Set_InsDelFind, run_RefinableSet )
        CDSUNIT_TEST_RefinableSet
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace set2
