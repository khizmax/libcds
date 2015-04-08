//$$CDS-header$$

#include "map2/map_insdel_item_string.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_Item_string );

    size_t Map_InsDel_Item_string::c_nMapSize = 1000000;
    size_t Map_InsDel_Item_string::c_nThreadCount = 4;
    size_t Map_InsDel_Item_string::c_nGoalItem = c_nMapSize / 2;
    size_t Map_InsDel_Item_string::c_nAttemptCount = 100000;
    size_t Map_InsDel_Item_string::c_nMaxLoadFactor = 8;
    bool   Map_InsDel_Item_string::c_bPrintGCState = true;

    void Map_InsDel_Item_string::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nGoalItem = cfg.getSizeT("GoalItemIndex", c_nGoalItem);
        c_nAttemptCount = cfg.getSizeT("AttemptCount", c_nAttemptCount );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
    }

    void Map_InsDel_Item_string::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_InsDel_Item_string" ));

        run_MichaelMap(in_name, invert);
        run_SplitList(in_name, invert);
        run_SkipListMap(in_name, invert);
        run_EllenBinTreeMap(in_name, invert);
        run_BronsonAVLTreeMap(in_name, invert);
        run_StripedMap(in_name, invert);
        run_RefinableMap(in_name, invert);
        run_CuckooMap(in_name, invert);

        endTestCase();
    }
} // namespace map2
