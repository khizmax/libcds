/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#include "map_find_string.h"
#include <cds_test/hash_func.h>

namespace map {
    
    size_t Map_find_string::s_nThreadCount = 8;
    size_t Map_find_string::s_nMapSize = 10000000;
    size_t Map_find_string::s_nMaxLoadFactor = 8;
    size_t Map_find_string::s_nPercentExists = 50;
    size_t Map_find_string::s_nPassCount = 2;

    size_t Map_find_string::s_nCuckooInitialSize = 1024;
    size_t Map_find_string::s_nCuckooProbesetSize = 16;
    size_t Map_find_string::s_nCuckooProbesetThreshold = 0;

    size_t Map_find_string::s_nFeldmanMap_HeadBits = 10;
    size_t Map_find_string::s_nFeldmanMap_ArrayBits = 4;


    size_t Map_find_string::s_nLoadFactor = 1;
    Map_find_string::value_vector Map_find_string::s_Data;
    std::vector<std::string> Map_find_string::s_arrString;

    void Map_find_string::setup_test_case()
    {
        cds_test::config const& cfg = get_config( "map_find_string" );

        s_nMapSize = cfg.get_size_t( "MapSize", s_nMapSize );
        if ( s_nMapSize < 1000 )
            s_nMapSize = 1000;

        s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
        if ( s_nThreadCount == 0 )
            s_nThreadCount = 1;

        s_nPercentExists = cfg.get_size_t( "PercentExists", s_nPercentExists );
        if ( s_nPercentExists > 100 )
            s_nPercentExists = 100;
        else if ( s_nPercentExists < 1 )
            s_nPercentExists = 1;

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

        s_arrString = load_dictionary();
    }

    void Map_find_string::TearDownTestCase()
    {
        s_arrString.clear();
        s_Data.clear();
    }

    void Map_find_string::SetUpTestCase()
    {
        setup_test_case();

        s_Data.clear();

        size_t nSize = s_arrString.size();
        if ( nSize > s_nMapSize )
            nSize = s_nMapSize;
        s_Data.reserve( nSize );

        size_t nActualSize = 0;
        for ( size_t i = 0; i < nSize; ++i ) {
            bool bExists = rand( 100 ) <= s_nPercentExists;
            if ( bExists )
                ++nActualSize;
            s_Data.push_back( { &s_arrString.at( i ), bExists } );
        }
        s_nMapSize = nActualSize;
    }

    template <typename Hash>
    void Map_find_string::fill_string_array()
    {
        typedef Hash hasher;
        typedef typename hasher::result_type hash_type;

        std::map<hash_type, size_t> mapHash;
        s_Data.clear();

        size_t nSize = s_arrString.size();
        if ( nSize > s_nMapSize )
            nSize = s_nMapSize;
        s_Data.reserve( nSize );

        size_t nActualSize = 0;
        size_t nDiffHash = 0;
        hasher h;
        for ( size_t i = 0; i < s_arrString.size(); ++i ) {
            hash_type hash = h( s_arrString.at( i ));
            if ( mapHash.insert( std::make_pair( hash, i ) ).second ) {
                if ( ++nDiffHash >= nSize )
                    break;
                bool bExists = rand( 100 ) <= s_nPercentExists;
                if ( bExists )
                    ++nActualSize;
                s_Data.push_back( { &s_arrString.at( i ), bExists } );
            }
        }
        s_nMapSize = nActualSize;

    }

    void Map_find_string_stdhash::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<std::hash<std::string>>();
    }

#if CDS_BUILD_BITS == 64
    void Map_find_string_city32::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<cds_test::city32>();
    }

    void Map_find_string_city64::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<cds_test::city64>();
    }

    void Map_find_string_city128::SetUpTestCase()
    {
        setup_test_case();
        fill_string_array<cds_test::city128>();
    }

#endif

    std::vector<size_t> Map_find_string::get_load_factors()
    {
        cds_test::config const& cfg = get_config( "map_find_string" );

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        std::vector<size_t> lf;
        for ( size_t n = 1; n <= s_nMaxLoadFactor; n *= 2 )
            lf.push_back( n );

        return lf;
    }

    INSTANTIATE_TEST_CASE_P( a, Map_find_string_LF, ::testing::ValuesIn( Map_find_string::get_load_factors() ) );
} // namespace map
