//$$CDS-header$$

#include "set2/set_insdel_func.h"
#include "set2/set_type_split_list.h"

namespace set2 {
    CDSUNIT_DEFINE_SplitList(cc::split_list::implementation_tag, Set_InsDel_func)

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_func, run_SplitList )
        CDSUNIT_TEST_SplitList
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
