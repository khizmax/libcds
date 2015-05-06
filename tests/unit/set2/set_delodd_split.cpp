//$$CDS-header$$

#include "set2/set_delodd.h"
#include "set2/set_type_split_list.h"

namespace set2 {
    CDSUNIT_DEFINE_SplitList(cc::split_list::implementation_tag, Set_DelOdd)

    CPPUNIT_TEST_SUITE_PART( Set_DelOdd, run_SplitList )
        CDSUNIT_TEST_SplitList
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
