//$$CDS-header$$

#include "set2/set_insdel_func.h"
#include "set2/set_type_michael.h"

namespace set2 {

    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDel_func );

    size_t  Set_InsDel_func::c_nMapSize = 1000000    ;  // map size
    size_t  Set_InsDel_func::c_nInsertThreadCount = 4;  // count of insertion thread
    size_t  Set_InsDel_func::c_nDeleteThreadCount = 4;  // count of deletion thread
    size_t  Set_InsDel_func::c_nEnsureThreadCount = 4;  // count of ensure thread
    size_t  Set_InsDel_func::c_nThreadPassCount = 4  ;  // pass count for each thread
    size_t  Set_InsDel_func::c_nMaxLoadFactor = 8    ;  // maximum load factor
    bool    Set_InsDel_func::c_bPrintGCState = true;

    CDSUNIT_DEFINE_MichaelSet( cc::michael_set::implementation_tag, Set_InsDel_func )

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_func, run_MichaelSet )
        CDSUNIT_TEST_MichaelSet
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
