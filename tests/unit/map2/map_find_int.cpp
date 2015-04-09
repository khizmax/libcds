//$$CDS-header$$

// defines concurrent access to map::nonconcurrent_iterator::Sequence::TValue::nAccess field

#include "map2/map_find_int.h"

// find int test in map<int> in mutithreaded mode
namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_find_int );

    size_t Map_find_int::c_nThreadCount = 8      ;  // thread count
    size_t Map_find_int::c_nMapSize = 20000000   ;  // map size (count of searching item)
    size_t Map_find_int::c_nPercentExists = 50   ;  // percent of existing keys in searching sequence
    size_t Map_find_int::c_nPassCount = 2;
    size_t Map_find_int::c_nMaxLoadFactor = 8    ;  // maximum load factor
    bool   Map_find_int::c_bPrintGCState = true;

    void Map_find_int::generateSequence()
    {
        size_t nPercent = c_nPercentExists;

        if ( nPercent > 100 )
            nPercent = 100;
        else if ( nPercent < 1 )
            nPercent = 1;

        m_nRealMapSize = 0;

        m_Arr.resize( c_nMapSize );
        for ( size_t i = 0; i < c_nMapSize; ++i ) {
            m_Arr[i].nKey = i * 13;
            m_Arr[i].bExists = CppUnitMini::Rand( 100 ) <= nPercent;
            if ( m_Arr[i].bExists )
                ++m_nRealMapSize;
        }
        shuffle( m_Arr.begin(), m_Arr.end() );
    }

    void Map_find_int::initTestSequence()
    {
        CPPUNIT_MSG( "Generating test data...");
        cds::OS::Timer    timer;
        generateSequence();
        CPPUNIT_MSG( "   Duration=" << timer.duration() );
        CPPUNIT_MSG( "Map size=" << m_nRealMapSize << " find key loop=" << m_Arr.size() << " (" << c_nPercentExists << "% success)" );
        CPPUNIT_MSG( "Thread count=" << c_nThreadCount << " Pass count=" << c_nPassCount );

        m_bSequenceInitialized = true;
    }

    void Map_find_int::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize);
        c_nPercentExists = cfg.getSizeT("PercentExists", c_nPercentExists);
        c_nPassCount = cfg.getSizeT("PassCount", c_nPassCount);
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor);
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );
    }

    void Map_find_int::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_find_int" ));

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

} // namespace map
