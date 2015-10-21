//$$CDS-header$$

#include "set2/set_insdelfind.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDelFind );

    void Set_InsDelFind::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nSetSize = cfg.getSizeT("InitialMapSize", c_nSetSize );
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_nInsertPercentage = cfg.getUInt("InsertPercentage", c_nInsertPercentage );
        c_nDeletePercentage = cfg.getUInt("DeletePercentage", c_nDeletePercentage );
        c_nDuration = cfg.getUInt("Duration", c_nDuration );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize );
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize );
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold );

        c_nFeldmanSet_HeadBits = cfg.getSizeT("FeldmanMapHeadBits", c_nFeldmanSet_HeadBits);
        c_nFeldmanSet_ArrayBits = cfg.getSizeT("FeldmanMapArrayBits", c_nFeldmanSet_ArrayBits);

        if ( c_nThreadCount == 0 )
            c_nThreadCount = std::thread::hardware_concurrency();

        CPPUNIT_ASSERT( c_nInsertPercentage + c_nDeletePercentage <= 100 );

        actions * pFirst = m_arrShuffle;
        actions * pLast = m_arrShuffle + c_nInsertPercentage;
        std::fill( pFirst, pLast, do_insert );
        pFirst = pLast;
        pLast += c_nDeletePercentage;
        std::fill( pFirst, pLast, do_delete );
        pFirst = pLast;
        pLast = m_arrShuffle + sizeof(m_arrShuffle)/sizeof(m_arrShuffle[0]);
        std::fill( pFirst, pLast, do_find );
        shuffle( m_arrShuffle, pLast );
    }
} // namespace set2
