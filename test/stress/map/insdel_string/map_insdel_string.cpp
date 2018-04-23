// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_insdel_string.h"
#include <cds_test/hash_func.h>

namespace map {

    size_t Map_InsDel_string::s_nMapSize = 1000000;      // map size
    size_t Map_InsDel_string::s_nInsertThreadCount = 4;  // count of insertion thread
    size_t Map_InsDel_string::s_nDeleteThreadCount = 4;  // count of deletion thread
    size_t Map_InsDel_string::s_nThreadPassCount = 4;    // pass count for each thread
    size_t Map_InsDel_string::s_nMaxLoadFactor = 8;      // maximum load factor

    size_t Map_InsDel_string::s_nCuckooInitialSize = 1024;// initial size for CuckooSet
    size_t Map_InsDel_string::s_nCuckooProbesetSize = 16; // CuckooSet probeset size (only for list-based probeset)
    size_t Map_InsDel_string::s_nCuckooProbesetThreshold = 0; // CuckooSet probeset threshold (0 - use default)

    size_t Map_InsDel_string::s_nFeldmanMap_HeadBits = 10;
    size_t Map_InsDel_string::s_nFeldmanMap_ArrayBits = 4;

    size_t Map_InsDel_string::s_nLoadFactor = 1;
    std::vector<std::string> Map_InsDel_string::s_arrKeys;

    void Map_InsDel_string::setup_test_case()
    {
        cds_test::config const& cfg = get_config( "map_insdel_string" );

        s_nMapSize = cfg.get_size_t( "MapSize", s_nMapSize );
        if ( s_nMapSize < 1000 )
            s_nMapSize = 1000;

        s_nInsertThreadCount = cfg.get_size_t( "InsertThreadCount", s_nInsertThreadCount );
        if ( s_nInsertThreadCount == 0 )
            s_nInsertThreadCount = 2;

        s_nDeleteThreadCount = cfg.get_size_t( "DeleteThreadCount", s_nDeleteThreadCount );
        if ( s_nDeleteThreadCount == 0 )
            s_nDeleteThreadCount = 2;

        s_nThreadPassCount = cfg.get_size_t( "ThreadPassCount", s_nThreadPassCount );
        if ( s_nThreadPassCount == 0 )
            s_nThreadPassCount = 4;

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        s_nCuckooInitialSize = cfg.get_size_t( "CuckooInitialSize", s_nCuckooInitialSize );
        if ( s_nCuckooInitialSize < 256 )
            s_nCuckooInitialSize = 256;

        s_nCuckooProbesetSize = cfg.get_size_t( "CuckooProbesetSize", s_nCuckooProbesetSize );
        if ( s_nCuckooProbesetSize < 8 )
            s_nCuckooProbesetSize = 8;

        s_nCuckooProbesetThreshold = cfg.get_size_t( "CuckooProbesetThreshold", s_nCuckooProbesetThreshold );

        s_nFeldmanMap_HeadBits = cfg.get_size_t( "FeldmanMapHeadBits", s_nFeldmanMap_HeadBits );
        if ( s_nFeldmanMap_HeadBits == 0 )
            s_nFeldmanMap_HeadBits = 2;

        s_nFeldmanMap_ArrayBits = cfg.get_size_t( "FeldmanMapArrayBits", s_nFeldmanMap_ArrayBits );
        if ( s_nFeldmanMap_ArrayBits == 0 )
            s_nFeldmanMap_ArrayBits = 2;
    }

    void Map_InsDel_string::SetUpTestCase()
    {
        setup_test_case();

        s_arrKeys.clear();
        s_arrKeys.reserve( s_nMapSize );
        std::vector<std::string> dict = load_dictionary();
        for ( size_t i = 0; i < s_nMapSize; ++i )
            s_arrKeys.push_back( std::move( dict.at(i)));
    }

    void Map_InsDel_string::TearDownTestCase()
    {
        s_arrKeys.clear();
    }

    std::vector<size_t> Map_InsDel_string::get_load_factors()
    {
        cds_test::config const& cfg = get_config( "map_insdel_string" );

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        std::vector<size_t> lf;
        for ( size_t n = 1; n <= s_nMaxLoadFactor; n *= 2 )
            lf.push_back( n );

        return lf;
    }

    template <typename Hash>
    void Map_InsDel_string::fill_string_array()
    {
        typedef Hash hasher;
        typedef decltype( hasher()( std::string())) hash_type;

        std::map<hash_type, size_t> mapHash;
        s_arrKeys.clear();
        std::vector<std::string> dict = load_dictionary();

        size_t nSize = dict.size();
        if ( nSize > s_nMapSize )
            nSize = s_nMapSize;
        s_arrKeys.reserve( nSize );

        size_t nDiffHash = 0;
        hasher h;
        for ( size_t i = 0; i < dict.size(); ++i ) {
            hash_type hash = h( dict.at( i ));
            if ( mapHash.insert( std::make_pair( hash, i )).second ) {
                if ( ++nDiffHash >= nSize )
                    break;
                s_arrKeys.push_back( std::move( dict.at( i )));
            }
        }
        s_nMapSize = dict.size();
    }

    void Map_InsDel_string_stdhash::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<std::hash<std::string>>();
    }

#if CDS_BUILD_BITS == 64
    void Map_InsDel_string_city32::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<cds_test::city32>();
    }

    void Map_InsDel_string_city64::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<cds_test::city64>();
    }

    void Map_InsDel_string_city128::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<cds_test::city128>();
    }

#endif

#ifdef CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG
    static std::string get_test_parameter_name( testing::TestParamInfo<size_t> const& p )
    {
        return std::to_string( p.param );
    }
    INSTANTIATE_TEST_CASE_P( a, Map_InsDel_string_LF, ::testing::ValuesIn( Map_InsDel_string::get_load_factors()), get_test_parameter_name );
#else
    INSTANTIATE_TEST_CASE_P( a, Map_InsDel_string_LF, ::testing::ValuesIn( Map_InsDel_string::get_load_factors()));
#endif

} // namespace map
