//$$CDS-header$$

#include "set2/set_insdel_func.h"
#include "set2/set_type_cuckoo.h"

namespace set2 {
    CDSUNIT_DEFINE_CuckooSet( cc::cuckoo::implementation_tag, Set_InsDel_func )

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_func, run_CuckooSet )
        CDSUNIT_TEST_CuckooSet
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
