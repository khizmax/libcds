//$$CDS-header$$

#include "set2/set_delodd.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_DelOdd );

    void Set_DelOdd::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nSetSize = cfg.getSizeT("MapSize", c_nSetSize );
        c_nInsThreadCount = cfg.getSizeT("InsThreadCount", c_nInsThreadCount);
        c_nDelThreadCount = cfg.getSizeT("DelThreadCount", c_nDelThreadCount);
        c_nExtractThreadCount = cfg.getSizeT("ExtractThreadCount", c_nExtractThreadCount);
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor);
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize );
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize );
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold );

        c_nFeldmanSet_HeadBits = cfg.getSizeT("FeldmanMapHeadBits", c_nFeldmanSet_HeadBits);
        c_nFeldmanSet_ArrayBits = cfg.getSizeT("FeldmanMapArrayBits", c_nFeldmanSet_ArrayBits);

        if ( c_nInsThreadCount == 0 )
            c_nInsThreadCount = std::thread::hardware_concurrency();
        if ( c_nDelThreadCount == 0 && c_nExtractThreadCount == 0 ) {
            c_nExtractThreadCount = std::thread::hardware_concurrency() / 2;
            c_nDelThreadCount = std::thread::hardware_concurrency() - c_nExtractThreadCount;
        }

        m_arrData.resize( c_nSetSize );
        for ( size_t i = 0; i < c_nSetSize; ++i )
            m_arrData[i] = i;
        shuffle( m_arrData.begin(), m_arrData.end() );
    }

    void Set_DelOdd::endTestCase()
    {
        m_arrData.resize( 0 );
    }

} // namespace set2
