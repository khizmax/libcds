// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_insdelfind.h"

namespace set {

    size_t Set_InsDelFind::s_nSetSize = 500000;      // initial set size
    size_t Set_InsDelFind::s_nThreadCount = 8;       // thread count
    size_t Set_InsDelFind::s_nMaxLoadFactor = 8;     // maximum load factor
    unsigned int Set_InsDelFind::s_nInsertPercentage = 5;
    unsigned int Set_InsDelFind::s_nDeletePercentage = 5;
    unsigned int Set_InsDelFind::s_nDuration = 30;   // test duration, seconds

    size_t Set_InsDelFind::s_nCuckooInitialSize = 1024;// initial size for CuckooSet
    size_t Set_InsDelFind::s_nCuckooProbesetSize = 16; // CuckooSet probeset size (only for list-based probeset)
    size_t Set_InsDelFind::s_nCuckooProbesetThreshold = 0; // CUckooSet probeset threshold (0 - use default)

    size_t Set_InsDelFind::s_nFeldmanSet_HeadBits = 10;
    size_t Set_InsDelFind::s_nFeldmanSet_ArrayBits = 4;

    size_t Set_InsDelFind::s_nLoadFactor = 2;
    Set_InsDelFind::actions Set_InsDelFind::s_arrShuffle[Set_InsDelFind::c_nShuffleSize];

    void Set_InsDelFind::SetUpTestCase()
    {
        cds_test::config const& cfg = get_config( "map_insdelfind" );

        s_nSetSize = cfg.get_size_t( "InitialMapSize", s_nSetSize );
        if ( s_nSetSize < 1000 )
            s_nSetSize = 1000;

        s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
        if ( s_nThreadCount == 0 )
            s_nThreadCount = 2;

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        s_nInsertPercentage = cfg.get_uint( "InsertPercentage", s_nInsertPercentage );
        if ( s_nInsertPercentage == 0 || s_nInsertPercentage > 90 )
            s_nInsertPercentage = 5;

        s_nDeletePercentage = cfg.get_uint( "DeletePercentage", s_nDeletePercentage );
        if ( s_nDeletePercentage == 0 || s_nDeletePercentage > 90 )
            s_nDeletePercentage = 5;

        s_nDuration = cfg.get_uint( "Duration", s_nDuration );
        if ( s_nDuration == 0 )
            s_nDuration = 5;

        s_nCuckooInitialSize = cfg.get_size_t( "CuckooInitialSize", s_nCuckooInitialSize );
        if ( s_nCuckooInitialSize < 256 )
            s_nCuckooInitialSize = 256;

        s_nCuckooProbesetSize = cfg.get_size_t( "CuckooProbesetSize", s_nCuckooProbesetSize );
        if ( s_nCuckooProbesetSize < 8 )
            s_nCuckooProbesetSize = 8;

        s_nCuckooProbesetThreshold = cfg.get_size_t( "CuckooProbesetThreshold", s_nCuckooProbesetThreshold );

        s_nFeldmanSet_HeadBits = cfg.get_size_t( "FeldmanMapHeadBits", s_nFeldmanSet_HeadBits );
        if ( s_nFeldmanSet_HeadBits == 0 )
            s_nFeldmanSet_HeadBits = 2;

        s_nFeldmanSet_ArrayBits = cfg.get_size_t( "FeldmanMapArrayBits", s_nFeldmanSet_ArrayBits );
        if ( s_nFeldmanSet_ArrayBits == 0 )
            s_nFeldmanSet_ArrayBits = 2;

        actions * pFirst = s_arrShuffle;
        actions * pLast = s_arrShuffle + s_nInsertPercentage;
        std::fill( pFirst, pLast, do_insert );
        pFirst = pLast;
        pLast += s_nDeletePercentage;
        std::fill( pFirst, pLast, do_delete );
        pFirst = pLast;
        pLast = s_arrShuffle + sizeof( s_arrShuffle ) / sizeof( s_arrShuffle[0] );
        if ( pFirst < pLast )
            std::fill( pFirst, pLast, do_find );
        shuffle( s_arrShuffle, pLast );
    }

    std::vector<size_t> Set_InsDelFind_LF::get_load_factors()
    {
        cds_test::config const& cfg = get_config( "map_delodd" );

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        std::vector<size_t> lf;
        for ( size_t n = 1; n <= s_nMaxLoadFactor; n *= 2 )
            lf.push_back( n );

        return lf;
    }

#ifdef CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG
    static std::string get_test_parameter_name( testing::TestParamInfo<size_t> const& p )
    {
        return std::to_string( p.param );
    }
    INSTANTIATE_TEST_CASE_P( a, Set_InsDelFind_LF, ::testing::ValuesIn( Set_InsDelFind_LF::get_load_factors()), get_test_parameter_name );
#else
    INSTANTIATE_TEST_CASE_P( a, Set_InsDelFind_LF, ::testing::ValuesIn( Set_InsDelFind_LF::get_load_factors()));
#endif

} // namespace set
