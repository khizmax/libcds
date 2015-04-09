//$$CDS-header$$

#include "set2/set_insdelfind.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDelFind );

    size_t Set_InsDelFind::c_nInitialMapSize = 500000;
    size_t Set_InsDelFind::c_nThreadCount = 8;
    size_t Set_InsDelFind::c_nMaxLoadFactor = 8;
    unsigned int Set_InsDelFind::c_nInsertPercentage = 5;
    unsigned int Set_InsDelFind::c_nDeletePercentage = 5;
    unsigned int Set_InsDelFind::c_nDuration = 30;
    bool Set_InsDelFind::c_bPrintGCState = true;

    void Set_InsDelFind::setUpParams( const CppUnitMini::TestCfg& cfg )
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

    void Set_InsDelFind::myRun(const char *in_name, bool invert /*= false*/)
    {
        setUpParams( m_Cfg.get( "Map_InsDelFind" ));

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
