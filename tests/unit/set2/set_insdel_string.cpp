//$$CDS-header$$

#include "set2/set_insdel_string.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDel_string );

    void Set_InsDel_string::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nSetSize = cfg.getSizeT("MapSize", c_nSetSize );
        c_nInsertThreadCount = cfg.getSizeT("InsertThreadCount", c_nInsertThreadCount );
        c_nDeleteThreadCount = cfg.getSizeT("DeleteThreadCount", c_nDeleteThreadCount );
        c_nThreadPassCount = cfg.getSizeT("ThreadPassCount", c_nThreadPassCount );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize );
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize );
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold );

        if ( c_nInsertThreadCount == 0 )
            c_nInsertThreadCount = std::thread::hardware_concurrency();
        if ( c_nDeleteThreadCount == 0 )
            c_nDeleteThreadCount = std::thread::hardware_concurrency();
    }
} // namespace set2
