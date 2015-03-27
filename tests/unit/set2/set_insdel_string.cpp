//$$CDS-header$$

#include "set2/set_insdel_string.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDel_string );

    size_t Set_InsDel_string::c_nMapSize = 1000000;
    size_t Set_InsDel_string::c_nInsertThreadCount = 4;
    size_t Set_InsDel_string::c_nDeleteThreadCount = 4;
    size_t Set_InsDel_string::c_nThreadPassCount = 4;
    size_t Set_InsDel_string::c_nMaxLoadFactor = 8;
    bool   Set_InsDel_string::c_bPrintGCState = true;

    void Set_InsDel_string::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nInsertThreadCount = cfg.getSizeT("InsertThreadCount", c_nInsertThreadCount );
        c_nDeleteThreadCount = cfg.getSizeT("DeleteThreadCount", c_nDeleteThreadCount );
        c_nThreadPassCount = cfg.getSizeT("ThreadPassCount", c_nThreadPassCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
    }

    void Set_InsDel_string::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_InsDel_string" ));

        run_MichaelSet(in_name, invert);
        run_SplitList(in_name, invert);
        run_SkipListSet(in_name, invert);
        run_EllenBinTreeSet(in_name, invert);
        run_StripedSet(in_name, invert);
        run_RefinableSet(in_name, invert);
        run_CuckooSet(in_name, invert);
        run_StdSet(in_name, invert);

        endTestCase();
    }
} // namespace set2
