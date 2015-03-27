//$$CDS-header$$

#include "map2/map_insdel_int.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_int );

    size_t Map_InsDel_int::c_nMapSize = 1000000;
    size_t Map_InsDel_int::c_nInsertThreadCount = 4;
    size_t Map_InsDel_int::c_nDeleteThreadCount = 4;
    size_t Map_InsDel_int::c_nThreadPassCount = 4;
    size_t Map_InsDel_int::c_nMaxLoadFactor = 8;
    bool   Map_InsDel_int::c_bPrintGCState = true;


    void Map_InsDel_int::setUpParams( const CppUnitMini::TestCfg& cfg ) {
        c_nInsertThreadCount = cfg.getSizeT("InsertThreadCount", c_nInsertThreadCount );
        c_nDeleteThreadCount = cfg.getSizeT("DeleteThreadCount", c_nDeleteThreadCount );
        c_nThreadPassCount = cfg.getSizeT("ThreadPassCount", c_nThreadPassCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );
    }

    void Map_InsDel_int::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_InsDel_int" ));

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
