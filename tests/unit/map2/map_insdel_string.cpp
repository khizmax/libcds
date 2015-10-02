//$$CDS-header$$

#include "map2/map_insdel_string.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_string );

    void Map_InsDel_string::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nInsertThreadCount = cfg.getSizeT("InsertThreadCount", c_nInsertThreadCount );
        c_nDeleteThreadCount = cfg.getSizeT("DeleteThreadCount", c_nDeleteThreadCount );
        c_nThreadPassCount = cfg.getSizeT("ThreadPassCount", c_nThreadPassCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize);
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize);
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold);

        c_nMultiLevelMap_HeadBits = cfg.getSizeT("MultiLevelMapHeadBits", c_nMultiLevelMap_HeadBits);
        c_nMultiLevelMap_ArrayBits = cfg.getSizeT("MultiLevelMapArrayBits", c_nMultiLevelMap_ArrayBits);

        if ( c_nInsertThreadCount == 0 )
            c_nInsertThreadCount = std::thread::hardware_concurrency();
        if ( c_nDeleteThreadCount == 0 )
            c_nDeleteThreadCount = std::thread::hardware_concurrency();
    }
} // namespace map2
