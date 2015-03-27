//$$CDS-header$$

#include "map2/map_find_string.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_find_string );

    size_t Map_find_string::c_nThreadCount = 8      ;  // thread count
    size_t Map_find_string::c_nMapSize = 20000000   ;  // map size (count of searching item)
    size_t Map_find_string::c_nPercentExists = 50   ;  // percent of existing keys in searching sequence
    size_t Map_find_string::c_nPassCount = 2;
    size_t Map_find_string::c_nMaxLoadFactor = 8    ;  // maximum load factor
    bool   Map_find_string::c_bPrintGCState = true;

    void Map_find_string::generateSequence()
    {
        size_t nPercent = c_nPercentExists;

        if ( nPercent > 100 )
            nPercent = 100;
        else if ( nPercent < 1 )
            nPercent = 1;

        m_nRealMapSize = 0;

        std::vector<std::string> const & arrString = CppUnitMini::TestCase::getTestStrings();
        size_t nSize = arrString.size();
        if ( nSize > c_nMapSize )
            nSize = c_nMapSize;
        m_Arr.resize( nSize );
        for ( size_t i = 0; i < nSize; ++i ) {
            m_Arr[i].pKey = &( arrString[i] );
            m_Arr[i].bExists = CppUnitMini::Rand( 100 ) <= nPercent;
            if ( m_Arr[i].bExists )
                ++m_nRealMapSize;
        }
    }

    void Map_find_string::initTestSequence()
    {
        if ( !m_bSeqInit ) {
            m_bSeqInit = true;

            CPPUNIT_MSG( "Generating test data...");
            cds::OS::Timer    timer;
            generateSequence();
            CPPUNIT_MSG( "   Duration=" << timer.duration() );
            CPPUNIT_MSG( "Map size=" << m_nRealMapSize << " find key loop=" << m_Arr.size() << " (" << c_nPercentExists << "% success)" );
            CPPUNIT_MSG( "Thread count=" << c_nThreadCount << " Pass count=" << c_nPassCount );
        }
    }

    void Map_find_string::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nPercentExists = cfg.getSizeT("PercentExists", c_nPercentExists );
        c_nPassCount = cfg.getSizeT("PassCount", c_nPassCount );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
    }

    void Map_find_string::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_find_string" ));

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
} // namespace map2
