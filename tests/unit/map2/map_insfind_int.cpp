//$$CDS-header$$

#include "map2/map_insfind_int.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsFind_int );

    size_t Map_InsFind_int::c_nMapSize = 1000000    ;  // map size
    size_t Map_InsFind_int::c_nThreadCount = 4      ;  // count of insertion thread
    size_t Map_InsFind_int::c_nMaxLoadFactor = 8    ;  // maximum load factor
    bool   Map_InsFind_int::c_bPrintGCState = true;

    void Map_InsFind_int::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nThreadCount = cfg.getULong("ThreadCount", 0 );
        c_nMapSize = cfg.getULong("MapSize", static_cast<unsigned long>(c_nMapSize));
        c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", static_cast<unsigned long>(c_nMaxLoadFactor));
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
        if ( c_nThreadCount == 0 )
            c_nThreadCount = cds::OS::topology::processor_count();
    }

    void Map_InsFind_int::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_InsFind_int" ));

        run_MichaelMap(in_name, invert);
        run_SplitList(in_name, invert);
        run_SkipListMap(in_name, invert);
        run_EllenBinTreeMap(in_name, invert);
        run_BronsonAVLTreeMap(in_name, invert);
        run_StripedMap(in_name, invert);
        run_RefinableMap(in_name, invert);
        run_CuckooMap(in_name, invert);
        run_StdMap(in_name, invert);

        endTestCase();
    }

    /*
    CPPUNIT_TEST_SUITE( Map_InsFind_int )
        CDSUNIT_TEST_MichaelMap
        CDSUNIT_TEST_MichaelMap_nogc
        CDSUNIT_TEST_SplitList
        CDSUNIT_TEST_SplitList_nogc
        CDSUNIT_TEST_SkipListMap
        CDSUNIT_TEST_SkipListMap_nogc
        CDSUNIT_TEST_EllenBinTreeMap
        CDSUNIT_TEST_BronsonAVLTreeMap
        CDSUNIT_TEST_StripedMap
        CDSUNIT_TEST_RefinableMap
        CDSUNIT_TEST_CuckooMap
        CDSUNIT_TEST_StdMap
    CPPUNIT_TEST_SUITE_END()
    */
} // namespace map2
