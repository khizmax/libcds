//$$CDS-header$$

#include "set2/set_delodd.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_DelOdd );

    size_t Set_DelOdd::c_nSetSize = 1000000;
    size_t Set_DelOdd::c_nInsThreadCount;
    size_t Set_DelOdd::c_nDelThreadCount;
    size_t Set_DelOdd::c_nExtractThreadCount;
    size_t Set_DelOdd::c_nMaxLoadFactor;
    bool   Set_DelOdd::c_bPrintGCState;

    void Set_DelOdd::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nSetSize = cfg.getSizeT("MapSize", c_nSetSize );
        c_nInsThreadCount = cfg.getSizeT("InsThreadCount", c_nInsThreadCount);
        c_nDelThreadCount = cfg.getSizeT("DelThreadCount", c_nDelThreadCount);
        c_nExtractThreadCount = cfg.getSizeT("ExtractThreadCount", c_nExtractThreadCount);
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor);
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );

        if ( c_nInsThreadCount == 0 )
            c_nInsThreadCount = cds::OS::topology::processor_count();
        if ( c_nDelThreadCount == 0 && c_nExtractThreadCount == 0 ) {
            c_nExtractThreadCount = cds::OS::topology::processor_count() / 2;
            c_nDelThreadCount = cds::OS::topology::processor_count() - c_nExtractThreadCount;
        }

        m_arrData.resize( c_nSetSize );
        for ( size_t i = 0; i < c_nSetSize; ++i )
            m_arrData[i] = i;
        shuffle( m_arrData.begin(), m_arrData.end() );
    }

    void Set_DelOdd::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_DelOdd" ));

        run_MichaelSet(in_name, invert);
        run_SplitList(in_name, invert);
        run_SkipListSet(in_name, invert);
        run_EllenBinTreeSet(in_name, invert);
        run_CuckooSet(in_name, invert);

        endTestCase();
    }
} // namespace set2
