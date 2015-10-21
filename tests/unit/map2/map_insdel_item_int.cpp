//$$CDS-header$$

#include "map2/map_insdel_item_int.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_Item_int );

    void Map_InsDel_Item_int::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nGoalItem = cfg.getSizeT("GoalItem", c_nMapSize / 2 );
        c_nAttemptCount = cfg.getSizeT("AttemptCount", c_nAttemptCount );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize);
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize);
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold);

        c_nFeldmanMap_HeadBits = cfg.getSizeT("FeldmanMapHeadBits", c_nFeldmanMap_HeadBits);
        c_nFeldmanMap_ArrayBits = cfg.getSizeT("FeldmanMapArrayBits", c_nFeldmanMap_ArrayBits);

        if ( c_nThreadCount == 0 )
            c_nThreadCount = std::thread::hardware_concurrency() * 2;
    }
} // namespace map2
