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

#include "map_insdelfind.h"

namespace map {

    size_t Map_InsDelFind::s_nMapSize = 500000;
    size_t Map_InsDelFind::s_nThreadCount = 8;
    size_t Map_InsDelFind::s_nMaxLoadFactor = 8;
    unsigned int Map_InsDelFind::s_nInsertPercentage = 5;
    unsigned int Map_InsDelFind::s_nDeletePercentage = 5;
    unsigned int Map_InsDelFind::s_nDuration = 30;


    size_t Map_InsDelFind::s_nCuckooInitialSize = 1024;// initial size for CuckooSet
    size_t Map_InsDelFind::s_nCuckooProbesetSize = 16; // CuckooSet probeset size (only for list-based probeset)
    size_t Map_InsDelFind::s_nCuckooProbesetThreshold = 0; // CUckooSet probeset threshold (0 - use default)

    size_t Map_InsDelFind::s_nFeldmanMap_HeadBits = 10;
    size_t Map_InsDelFind::s_nFeldmanMap_ArrayBits = 4;

    size_t Map_InsDelFind::s_nLoadFactor = 1;
    Map_InsDelFind::actions Map_InsDelFind::s_arrShuffle[Map_InsDelFind::c_nShuffleSize];

    void Map_InsDelFind::SetUpTestCase()
    {
        cds_test::config const& cfg = get_config( "map_insdelfind" );

        s_nMapSize = cfg.get_size_t( "InitialMapSize", s_nMapSize );
        if ( s_nMapSize < 1000 )
            s_nMapSize = 1000;

        s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
        if ( s_nThreadCount == 0 )
            s_nThreadCount = std::thread::hardware_concurrency() * 2;

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        s_nInsertPercentage = cfg.get_uint( "InsertPercentage", s_nInsertPercentage );
        if ( s_nInsertPercentage >= 100 )
            s_nInsertPercentage = 99;

        s_nDeletePercentage = cfg.get_uint( "DeletePercentage", s_nDeletePercentage );
        if ( s_nDeletePercentage >= 100 )
            s_nDeletePercentage = 99;

        if ( s_nInsertPercentage + s_nDeletePercentage > 100 ) {
            unsigned int total = s_nInsertPercentage + s_nDeletePercentage;
            s_nInsertPercentage = s_nInsertPercentage * 100 / total;
            s_nDeletePercentage = s_nDeletePercentage * 100 / total;
        }

        s_nDuration = cfg.get_uint( "Duration", s_nDuration );
        if ( s_nDuration < 5 )
            s_nDuration = 5;

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

    std::vector<size_t> Map_InsDelFind_LF::get_load_factors()
    {
        cds_test::config const& cfg = get_config( "map_insdelfind" );

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        std::vector<size_t> lf;
        for ( size_t n = 1; n <= s_nMaxLoadFactor; n *= 2 )
            lf.push_back( n );

        return lf;
    }

    INSTANTIATE_TEST_CASE_P( a, Map_InsDelFind_LF, ::testing::ValuesIn( Map_InsDelFind_LF::get_load_factors()));
} // namespace map
