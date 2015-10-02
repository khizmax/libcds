//$$CDS-header$$

#include "map2/map_insdel_func.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_func );

    void Map_InsDel_func::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nInsertThreadCount = cfg.getULong("InsertThreadCount", static_cast<unsigned long>(c_nInsertThreadCount));
        c_nDeleteThreadCount = cfg.getULong("DeleteThreadCount", static_cast<unsigned long>(c_nDeleteThreadCount));
        c_nUpdateThreadCount = cfg.getULong("UpdateThreadCount", static_cast<unsigned long>(c_nUpdateThreadCount));
        c_nThreadPassCount = cfg.getULong("ThreadPassCount", static_cast<unsigned long>(c_nThreadPassCount));
        c_nMapSize = cfg.getULong("MapSize", static_cast<unsigned long>(c_nMapSize));
        c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", static_cast<unsigned long>(c_nMaxLoadFactor));
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );

        c_nCuckooInitialSize = cfg.getULong("CuckooInitialSize", static_cast<unsigned long>(c_nCuckooInitialSize) );
        c_nCuckooProbesetSize = cfg.getULong("CuckooProbesetSize", static_cast<unsigned long>(c_nCuckooProbesetSize) );
        c_nCuckooProbesetThreshold = cfg.getULong("CuckooProbesetThreshold", static_cast<unsigned long>(c_nCuckooProbesetThreshold) );

        c_nMultiLevelMap_HeadBits = cfg.getULong("MultiLevelMapHeadBits", static_cast<unsigned long>(c_nMultiLevelMap_HeadBits) );
        c_nMultiLevelMap_ArrayBits = cfg.getULong("MultiLevelMapArrayBits", static_cast<unsigned long>(c_nMultiLevelMap_ArrayBits) );

    }
} // namespace map2
