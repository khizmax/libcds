//$$CDS-header$$

#include "set2/set_insdel_string.h"
#include "set2/set_type_cuckoo.h"

namespace set2 {
    CDSUNIT_DEFINE_CuckooSet( cc::cuckoo::implementation_tag, Set_InsDel_string )

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_string, run_CuckooSet )
        CDSUNIT_TEST_CuckooSet
    CPPUNIT_TEST_SUITE_END_PART()
} // namespace set2
