//$$CDS-header$$

#include "map2/map_insdel_func.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_func );

    static const size_t def_nMapSize = 1000000;
    static const size_t def_nInsertThreadCount = 4;
    static const size_t def_nDeleteThreadCount = 4;
    static const size_t def_nEnsureThreadCount = 4;
    static const size_t def_nThreadPassCount = 4;
    static const size_t def_nMaxLoadFactor = 8;

    size_t  Map_InsDel_func::c_nMapSize = def_nMapSize    ;  // map size
    size_t  Map_InsDel_func::c_nInsertThreadCount = def_nInsertThreadCount;  // count of insertion thread
    size_t  Map_InsDel_func::c_nDeleteThreadCount = def_nDeleteThreadCount;  // count of deletion thread
    size_t  Map_InsDel_func::c_nEnsureThreadCount = def_nEnsureThreadCount;  // count of ensure thread
    size_t  Map_InsDel_func::c_nThreadPassCount = def_nThreadPassCount;  // pass count for each thread
    size_t  Map_InsDel_func::c_nMaxLoadFactor = def_nMaxLoadFactor;  // maximum load factor
    bool    Map_InsDel_func::c_bPrintGCState = true;

    void Map_InsDel_func::setUpParams( const CppUnitMini::TestCfg& cfg ) 
    {
        c_nInsertThreadCount = cfg.getULong("InsertThreadCount", def_nInsertThreadCount );
        c_nDeleteThreadCount = cfg.getULong("DeleteThreadCount", def_nDeleteThreadCount );
        c_nEnsureThreadCount = cfg.getULong("EnsureThreadCount", def_nEnsureThreadCount );
        c_nThreadPassCount = cfg.getULong("ThreadPassCount", def_nThreadPassCount );
        c_nMapSize = cfg.getULong("MapSize", def_nMapSize );
        c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", def_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
    }

    void Map_InsDel_func::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_InsDel_func" ));

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
