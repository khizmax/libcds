//$$CDS-header$$

#include "map2/map_find_string.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_find_string );

    void Map_find_string::generateSequence()
    {
        size_t nPercent = c_nPercentExists;

        if ( nPercent > 100 )
            nPercent = 100;
        else if ( nPercent < 1 )
            nPercent = 1;

        std::vector<std::string> const& arrString = CppUnitMini::TestCase::getTestStrings();
        size_t nSize = arrString.size();
        if ( nSize > c_nMapSize )
            nSize = c_nMapSize;
        m_Arr.reserve( nSize );

        nSize = 0;
        for ( size_t i = 0; i < nSize && i < arrString.size(); ++i ) {
        bool bExists = CppUnitMini::Rand( 100 ) <= nPercent;
            m_Arr.push_back( { &arrString.at(i), bExists } );
            if ( bExists )
                ++nSize;
        }
        c_nMapSize = nSize;
    }

    void Map_find_string::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize);
        c_nPercentExists = cfg.getSizeT("PercentExists", c_nPercentExists);
        c_nPassCount = cfg.getSizeT("PassCount", c_nPassCount);
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor);
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize);
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize);
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold);

        c_nFeldmanMap_HeadBits = cfg.getSizeT("FeldmanMapHeadBits", c_nFeldmanMap_HeadBits);
        c_nFeldmanMap_ArrayBits = cfg.getSizeT("FeldmanMapArrayBits", c_nFeldmanMap_ArrayBits);

        if ( c_nThreadCount == 0 )
            c_nThreadCount = std::thread::hardware_concurrency();

        CPPUNIT_MSG( "Generating test data...\n");
        cds::OS::Timer    timer;
        generateSequence();
        CPPUNIT_MSG( "   Duration=" << timer.duration() << "\n" );

    }
} // namespace map2
