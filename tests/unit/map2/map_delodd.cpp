//$$CDS-header$$

#include "map2/map_delodd.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_DelOdd );

    void Map_DelOdd::setUpParams( const CppUnitMini::TestCfg& cfg ) {
        c_nMapSize = cfg.getULong("MapSize", static_cast<unsigned long>(c_nMapSize) );
        c_nInsThreadCount = cfg.getULong("InsThreadCount", static_cast<unsigned long>(c_nInsThreadCount) );
        c_nDelThreadCount = cfg.getULong("DelThreadCount", static_cast<unsigned long>(c_nDelThreadCount) );
        c_nExtractThreadCount = cfg.getULong("ExtractThreadCount", static_cast<unsigned long>(c_nExtractThreadCount) );
        c_nMaxLoadFactor = cfg.getULong("MaxLoadFactor", static_cast<unsigned long>(c_nMaxLoadFactor) );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );
        c_nMultiLevelMap_HeadBits = cfg.getULong("MultiLevelMap_HeadBits", static_cast<unsigned long>(c_nMultiLevelMap_HeadBits) );
        c_nMultiLevelMap_ArrayBits = cfg.getULong("MultiLevelMap_ArrayBits", static_cast<unsigned long>(c_nMultiLevelMap_ArrayBits) );


        if ( c_nInsThreadCount == 0 )
            c_nInsThreadCount = cds::OS::topology::processor_count();
        if ( c_nDelThreadCount == 0 && c_nExtractThreadCount == 0 ) {
            c_nExtractThreadCount = cds::OS::topology::processor_count() / 2;
            c_nDelThreadCount = cds::OS::topology::processor_count() - c_nExtractThreadCount;
        }

        m_arrInsert.resize( c_nMapSize );
        m_arrRemove.resize( c_nMapSize );
        for ( size_t i = 0; i < c_nMapSize; ++i ) {
            m_arrInsert[i] = i;
            m_arrRemove[i] = i;
        }
        shuffle( m_arrInsert.begin(), m_arrInsert.end() );
        shuffle( m_arrRemove.begin(), m_arrRemove.end() );
    }

} // namespace map2
