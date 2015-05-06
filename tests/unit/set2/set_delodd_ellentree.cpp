//$$CDS-header$$

#include "set2/set_delodd.h"
#include "set2/set_type_ellen_bintree.h"

namespace set2 {
    CDSUNIT_DEFINE_EllenBinTreeSet( cc::ellen_bintree::implementation_tag, Set_DelOdd )

    CPPUNIT_TEST_SUITE_PART( Set_DelOdd, run_EllenBinTreeSet )
        CDSUNIT_TEST_EllenBinTreeSet
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
