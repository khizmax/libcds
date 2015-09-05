//$$CDS-header$$

#include "map2/map_insdel_int.h"
#include <cds/os/topology.h>

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_int );


    void Map_InsDel_int::setUpParams( const CppUnitMini::TestCfg& cfg ) {
        c_nInsertThreadCount = cfg.getSizeT("InsertThreadCount", c_nInsertThreadCount );
        c_nDeleteThreadCount = cfg.getSizeT("DeleteThreadCount", c_nDeleteThreadCount );
        c_nThreadPassCount = cfg.getSizeT("ThreadPassCount", c_nThreadPassCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );

        c_nCuckooInitialSize = cfg.getULong("CuckooInitialSize", static_cast<unsigned long>(c_nCuckooInitialSize) );
        c_nCuckooProbesetSize = cfg.getULong("CuckooProbesetSize", static_cast<unsigned long>(c_nCuckooProbesetSize) );
        c_nCuckooProbesetThreshold = cfg.getULong("CuckooProbesetThreshold", static_cast<unsigned long>(c_nCuckooProbesetThreshold) );

        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        if ( c_nInsertThreadCount == 0 )
            c_nInsertThreadCount = cds::OS::topology::processor_count();
        if ( c_nDeleteThreadCount == 0 )
            c_nDeleteThreadCount = cds::OS::topology::processor_count();

        m_arrValues.clear();
        m_arrValues.reserve( c_nMapSize );
        for ( size_t i = 0; i < c_nMapSize; ++i )
            m_arrValues.push_back( i );
        shuffle( m_arrValues.begin(), m_arrValues.end() );
    }

} // namespace map2
