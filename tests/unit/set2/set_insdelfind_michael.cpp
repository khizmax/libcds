//$$CDS-header$$

#include "set2/set_insdelfind.h"
#include "set2/set_type_michael.h"


namespace set2 {
    CDSUNIT_DEFINE_MichaelSet( cc::michael_set::implementation_tag, Set_InsDelFind )

    CPPUNIT_TEST_SUITE_PART( Set_InsDelFind, run_MichaelSet )
        CDSUNIT_TEST_MichaelSet
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace set2
