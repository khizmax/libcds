//$$CDS-header$$

#include "map2/map_insdelfind.h"
#include <cds/os/topology.h>

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDelFind );

    size_t  Map_InsDelFind::c_nInitialMapSize = 500000;
    size_t  Map_InsDelFind::c_nThreadCount = 8;
    size_t  Map_InsDelFind::c_nMaxLoadFactor = 8;
    unsigned int Map_InsDelFind::c_nInsertPercentage = 5;
    unsigned int Map_InsDelFind::c_nDeletePercentage = 5;
    unsigned int Map_InsDelFind::c_nDuration = 30;
    bool    Map_InsDelFind::c_bPrintGCState = true;

    void Map_InsDelFind::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nInitialMapSize = cfg.getSizeT("InitialMapSize", c_nInitialMapSize );
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_nInsertPercentage = cfg.getUInt("InsertPercentage", c_nInsertPercentage );
        c_nDeletePercentage = cfg.getUInt("DeletePercentage", c_nDeletePercentage );
        c_nDuration = cfg.getUInt("Duration", c_nDuration );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        if ( c_nThreadCount == 0 )
            c_nThreadCount = cds::OS::topology::processor_count() * 2;

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

    void Map_InsDelFind::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_InsDelFind" ));

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
