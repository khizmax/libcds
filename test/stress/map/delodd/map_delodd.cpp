/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#include "map_delodd.h"

namespace map {

    size_t  Map_DelOdd::s_nMapSize = 10000;
    size_t  Map_DelOdd::s_nInsThreadCount = 4;
    size_t  Map_DelOdd::s_nDelThreadCount = 4;
    size_t  Map_DelOdd::s_nExtractThreadCount = 4;
    size_t  Map_DelOdd::s_nFindThreadCount = 2;
    size_t  Map_DelOdd::s_nMaxLoadFactor = 8;
    size_t  Map_DelOdd::s_nInsertPassCount = 100;

    size_t  Map_DelOdd::s_nCuckooInitialSize = 1024;
    size_t  Map_DelOdd::s_nCuckooProbesetSize = 16;
    size_t  Map_DelOdd::s_nCuckooProbesetThreshold = 0;

    size_t Map_DelOdd::s_nFeldmanMap_HeadBits = 10;
    size_t Map_DelOdd::s_nFeldmanMap_ArrayBits = 4;

    size_t Map_DelOdd::s_nLoadFactor = 1;
    std::vector<size_t> Map_DelOdd::m_arrElements;

    void Map_DelOdd::SetUpTestCase()
    {
        cds_test::config const& cfg = get_config( "map_delodd" );

        s_nMapSize = cfg.get_size_t( "MapSize", s_nMapSize );
        if ( s_nMapSize < 1000 )
            s_nMapSize = 1000;

        s_nInsThreadCount = cfg.get_size_t( "InsThreadCount", s_nInsThreadCount );
        if ( s_nInsThreadCount == 0 )
            s_nInsThreadCount = 1;

        s_nDelThreadCount = cfg.get_size_t( "DelThreadCount", s_nDelThreadCount );
        s_nExtractThreadCount = cfg.get_size_t( "ExtractThreadCount", s_nExtractThreadCount );
        s_nFindThreadCount = cfg.get_size_t( "FindThreadCount", s_nFindThreadCount );

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        s_nInsertPassCount = cfg.get_size_t( "PassCount", s_nInsertPassCount );
        if ( s_nInsertPassCount == 0 )
            s_nInsertPassCount = 100;

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

        m_arrElements.resize( s_nMapSize );
        for ( size_t i = 0; i < s_nMapSize; ++i )
            m_arrElements[i] = i;;
        shuffle( m_arrElements.begin(), m_arrElements.end());
    }

    void Map_DelOdd::TearDownTestCase()
    {
        m_arrElements.clear();
    }

    std::vector<size_t> Map_DelOdd_LF::get_load_factors()
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

    INSTANTIATE_TEST_CASE_P( a, Map_DelOdd_LF, ::testing::ValuesIn( Map_DelOdd_LF::get_load_factors()));
} // namespace map
